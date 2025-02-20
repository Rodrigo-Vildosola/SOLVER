#include "solver.h"
#include "exception.h"
#include "postfix.h"
#include "validator.h"
#include "debug.h"
#include "tokenizer.h"
#include "tokenizer_new.h"

#include "ast.h"
#include "compiler.h"

Solver::Solver(size_t exprCacheSize)
    : expressionCache(exprCacheSize) {
    PROFILE_BEGIN_SESSION("SOLVER", "solver.json");
    registerBuiltInFunctions();
}

void Solver::setUseCache(bool useCache) {
    PROFILE_FUNCTION()
    cacheEnabled = useCache;
}

void Solver::invalidateCaches() {
    PROFILE_FUNCTION()
    if (cacheEnabled) {
        expressionCache.clear();
    }
}

void Solver::clearCache() {
    PROFILE_FUNCTION()
    expressionCache.clear();
}

void Solver::declareConstant(const std::string& name, NUMBER_TYPE value) {
    PROFILE_FUNCTION()
    symbolTable.declareConstant(name, value);
    invalidateCaches();
}

void Solver::declareVariable(const std::string& name, NUMBER_TYPE value) {
    PROFILE_FUNCTION()
    symbolTable.declareVariable(name, value);
    invalidateCaches();
}

#pragma region Parsing

EvalFunc Solver::compileExpression(const std::string &expression, bool debug) {
    // Tokenize the expression
    auto tokens = TokenizerNew::tokenize(expression);
    // Convert tokens to postfix using the shunting-yard algorithm
    auto postfix = Postfix::shuntingYard(tokens);
    // Flatten any user-defined functions
    auto flattened = Postfix::flattenPostfix(postfix, functions);
    // Replace constant symbols with their values from the symbol table
    auto inlined = Simplification::replaceConstantSymbols(flattened, symbolTable);
    // Simplify the resulting postfix expression
    auto simplified = Simplification::simplifyPostfix(inlined, functions);

    if (debug) {
        std::cout << "Simplified postfix: ";
        printPostfix(simplified);
    }

    // Compile the simplified postfix into an EvalFunc and return it
    return compilePostfix(simplified, functions);
}

#pragma endregion

#pragma region Evaluation

NUMBER_TYPE Solver::evaluate(const std::string& expression, bool debug) {
    PROFILE_FUNCTION()
    setCurrentExpression(expression, debug);

    std::size_t cacheKey = generateCacheKey(expression, {});
    if (cacheEnabled) {
        if (NUMBER_TYPE* cachedResult = expressionCache.get(cacheKey)) {
            return *cachedResult;
        }
    }

    EvalFunc compiledExpr = currentCompiledExpression;

    Env env = symbolTable.getVariables();

    NUMBER_TYPE result = compiledExpr(env);

    if (cacheEnabled) {
        expressionCache.put(cacheKey, result);
    }

    return result;
}

std::vector<NUMBER_TYPE> Solver::evaluateForRange(const std::string& variable, const std::vector<NUMBER_TYPE>& values, const std::string& expression, bool debug) {
    PROFILE_FUNCTION()
    setCurrentExpression(expression, debug);

    std::vector<NUMBER_TYPE> results;
    results.reserve(values.size());

    if (!Validator::isValidName(variable)) {
        throw SolverException("Invalid variable name '" + variable + "'.");
    }

    EvalFunc compiledExpr = currentCompiledExpression;

    Env env = symbolTable.getVariables();

    for (NUMBER_TYPE value : values) {
        PROFILE_SCOPE("EvaluateRangeLoop");
        env[variable] = value;

        try {
            NUMBER_TYPE result = compiledExpr(env);
            results.push_back(result);
        } catch (const SolverException& e) {
            std::cerr << "Error evaluating expression for " << variable << " = " << value << ": " << e.what() << std::endl;
            results.push_back(std::nan(""));
        }
    }

    return results;
}

std::vector<NUMBER_TYPE> Solver::evaluateForRanges(const std::vector<std::string>& variables, const std::vector<std::vector<NUMBER_TYPE>>& valuesSets, const std::string& expression, bool debug) {
    PROFILE_FUNCTION()

    // Basic validation:
    if (variables.size() != valuesSets.size()) {
        throw SolverException("Mismatch in number of variables vs. value ranges.");
    }
    for (const auto& var : variables) {
        if (!Validator::isValidName(var)) {
            throw SolverException("Invalid variable name '" + var + "'.");
        }
    }

    // Parse and compile the expression
    setCurrentExpression(expression, debug);
    EvalFunc compiledExpr = currentCompiledExpression;

    // Compute the total number of combinations in the cartesian product
    size_t totalCombinations = 1;
    for (const auto& vals : valuesSets) {
        totalCombinations *= vals.size();
    }

    // Prepare output vector
    std::vector<NUMBER_TYPE> results;
    results.reserve(totalCombinations);

    // Initialize an environment map with current variable values
    Env env = symbolTable.getVariables();

    // Indices for tracking cartesian product iteration
    std::vector<size_t> indices(variables.size(), 0);

    // Iterate through all combinations
    for (size_t count = 0; count < totalCombinations; ++count) {
        PROFILE_SCOPE("EvaluateRangesLoop");

        // Assign each variable to its current index's value
        for (size_t i = 0; i < variables.size(); ++i) {
            env[variables[i]] = valuesSets[i][indices[i]];
        }

        // Evaluate and capture the result
        try {
            NUMBER_TYPE val = compiledExpr(env);
            results.push_back(val);
        } catch (const SolverException& e) {
            std::cerr << "Error evaluating expression for combination " << (count + 1)
                      << " of " << totalCombinations << ": " << e.what() << std::endl;
            results.push_back(std::nan(""));
        }

        // Increment the indices in a multi-digit manner (last variable changes fastest)
        for (int varIndex = static_cast<int>(variables.size()) - 1; varIndex >= 0; --varIndex) {
            indices[varIndex]++;
            if (indices[varIndex] < valuesSets[varIndex].size()) {
                break; // Successfully incremented without overflow
            } else {
                indices[varIndex] = 0; // Reset this index and carry over to the next variable
            }
        }
    }

    return results;
}

#pragma endregion

#pragma region Functions

void Solver::registerPredefinedFunction(const std::string& name, const FunctionCallback& callback, size_t argCount) {
    PROFILE_FUNCTION()
    auto result = functions.emplace(name, Function(callback, argCount));
    if (!result.second) {
        throw SolverException("Function '" + name + "' already exists.");
    }
}

void Solver::declareFunction(const std::string& name, const std::vector<std::string>& args, const std::string& expression) {
    PROFILE_FUNCTION()
    if (!Validator::isValidName(name)) {
        throw SolverException("Invalid function name: '" + name + "'.");
    }

    Validator::isValidSyntax(expression);

    try {
        // Tokenize and convert the function body to postfix
        auto tokens = TokenizerNew::tokenize(expression);
        auto postfix = Postfix::shuntingYard(tokens);
        auto flattened = Postfix::flattenPostfix(postfix, functions);

        // Store the function with its inlined postfix and argument names
        functions[name] = Function(flattened, args);
    } catch (const std::exception& e) {
        throw SolverException("Error defining function '" + name + "': " + e.what());
    }
}

#pragma endregion

#pragma region Helpers


void Solver::setCurrentExpression(const std::string &expression, bool debug) {
    // If the expression is unchanged and we already have a compiled expression, do nothing.
    // (Note: since EvalFunc is likely a std::function, checking if it is nonempty works.)
    if (expression == currentExpression && currentCompiledExpression) {
        return;
    }

    // Update the current expression string
    currentExpression = expression;
    // Compile the expression and cache the resulting EvalFunc
    currentCompiledExpression = compileExpression(expression, debug);

    if (debug) {
        std::cout << "Current expression set to: " << expression << std::endl;
    }
}

#pragma endregion

std::tuple<NumberList, NumberList> Solver::generateAnimationData(
    const std::string &expression,
    const std::string &variable,
    NUMBER_TYPE start,
    NUMBER_TYPE end,
    size_t steps) 
{
    PROFILE_FUNCTION();

    // Generate x values
    NumberList x_values(steps);
    NUMBER_TYPE step_size = (steps > 1) ? (end - start) / (steps - 1) : 0;
    
    for (size_t i = 0; i < steps; ++i) {
        x_values[i] = start + i * step_size;
    }

    // Evaluate the function in batch using evaluateForRange (optimized)
    NumberList y_values = evaluateForRange(variable, x_values, expression, false);

    return std::make_tuple(x_values, y_values);
}


std::tuple<NumberList, NumberList, NumberMatrix> Solver::generateContourData(
    const std::string &expression,
    const std::string &variable1,
    const std::string &variable2,
    NUMBER_TYPE start1,
    NUMBER_TYPE end1,
    size_t steps1,
    NUMBER_TYPE start2,
    NUMBER_TYPE end2,
    size_t steps2) 
{
    PROFILE_FUNCTION();

    // Generate grid values for x and y
    NumberList x_values(steps1);
    NumberList y_values(steps2);

    NUMBER_TYPE step_size1 = (steps1 > 1) ? (end1 - start1) / (steps1 - 1) : 0;
    NUMBER_TYPE step_size2 = (steps2 > 1) ? (end2 - start2) / (steps2 - 1) : 0;

    for (size_t i = 0; i < steps1; ++i) {
        x_values[i] = start1 + i * step_size1;
    }
    for (size_t j = 0; j < steps2; ++j) {
        y_values[j] = start2 + j * step_size2;
    }

    // Use evaluateForRanges to compute all function values in a batch
    std::vector<std::vector<NUMBER_TYPE>> z_values(steps2, std::vector<NUMBER_TYPE>(steps1));
    
    std::vector<std::vector<NUMBER_TYPE>> valuesSets = {x_values, y_values};
    std::vector<NUMBER_TYPE> flat_results = evaluateForRanges({variable1, variable2}, valuesSets, expression, false);

    // Reshape flat_results into a matrix (row-major order)
    size_t index = 0;
    for (size_t j = 0; j < steps2; ++j) {
        for (size_t i = 0; i < steps1; ++i) {
            z_values[j][i] = flat_results[index++];
        }
    }

    return std::make_tuple(x_values, y_values, z_values);
}


#pragma region Calculus

NUMBER_TYPE Solver::evaluateDerivative(
    const std::string& expression,
    const std::string& variable,
    NUMBER_TYPE atValue,
    const std::string& method,
    NUMBER_TYPE epsilon) 
{
    PROFILE_FUNCTION();

    auto replaceVariable = [](const std::string& expr, const std::string& var, NUMBER_TYPE value) {
        std::string modifiedExpr = expr;
        size_t pos = modifiedExpr.find(var);
        if (pos != std::string::npos) {
            modifiedExpr.replace(pos, var.length(), std::to_string(value));
        }
        return modifiedExpr;
    };

    std::string expr_fwd = replaceVariable(expression, variable, atValue + epsilon);
    std::string expr_bwd = replaceVariable(expression, variable, atValue - epsilon);
    std::string expr_mid = replaceVariable(expression, variable, atValue);

    // Forward Difference: f'(x) ≈ (f(x + ε) - f(x)) / ε
    if (method == "forward") {
        return (evaluate(expr_fwd) - evaluate(expr_mid)) / epsilon;
    }
    // Backward Difference: f'(x) ≈ (f(x) - f(x - ε)) / ε
    else if (method == "backward") {
        return (evaluate(expr_mid) - evaluate(expr_bwd)) / epsilon;
    }
    // Central Difference (default): f'(x) ≈ (f(x + ε) - f(x - ε)) / (2ε)
    else {
        return (evaluate(expr_fwd) - evaluate(expr_bwd)) / (2 * epsilon);
    }
}


NUMBER_TYPE Solver::evaluateIntegral(
    const std::string& expression,
    const std::string& variable,
    NUMBER_TYPE lower,
    NUMBER_TYPE upper,
    size_t steps) 
{
    PROFILE_FUNCTION();

    NUMBER_TYPE stepSize = (upper - lower) / steps;
    NUMBER_TYPE integral = 0.0;

    auto replaceVariable = [](const std::string& expr, const std::string& var, NUMBER_TYPE value) {
        std::string modifiedExpr = expr;
        size_t pos = modifiedExpr.find(var);
        if (pos != std::string::npos) {
            modifiedExpr.replace(pos, var.length(), std::to_string(value));
        }
        return modifiedExpr;
    };

    for (size_t i = 0; i < steps; ++i) {
        NUMBER_TYPE x1 = lower + i * stepSize;
        NUMBER_TYPE x2 = lower + (i + 1) * stepSize;

        NUMBER_TYPE f_x1 = evaluate(replaceVariable(expression, variable, x1));
        NUMBER_TYPE f_x2 = evaluate(replaceVariable(expression, variable, x2));

        integral += 0.5 * (f_x1 + f_x2) * stepSize;
    }

    return integral;
}




#pragma endregion
