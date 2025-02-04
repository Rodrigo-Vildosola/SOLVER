#include "solver.h"
#include "exception.h"
#include "postfix.h"
#include "validator.h"
#include "debug.h"
#include "tokenizer.h"
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

std::vector<Token> Solver::parse(const std::string& expression, bool debug) {
    auto tokens   = Tokenizer::tokenize(expression);
    auto postfix  = Postfix::shuntingYard(tokens);
    auto flattened = Postfix::flattenPostfix(postfix, functions);
    auto inlined = Simplification::replaceConstantSymbols(flattened, symbolTable);

    // Now do a simplification pass
    auto simplified = Simplification::simplifyPostfix(inlined, functions);

    if (debug) {
        std::cout << "Simplified postfix: ";
        printPostfix(simplified);
    }

    return simplified; 
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

    EvalFunc compiledExpr = compilePostfix(currentPostfix, functions);

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

    EvalFunc compiledExpr = compilePostfix(currentPostfix, functions);

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
    EvalFunc compiledExpr = compilePostfix(currentPostfix, functions);

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
        auto tokens = Tokenizer::tokenize(expression);
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


void Solver::setCurrentExpression(const std::string& expression, bool debug) {
    // Check if the expression is the same and the currentPostfix is not empty
    if (expression == currentExpressionPostfix && !currentPostfix.empty()) {
        return;
    }

    // Otherwise, parse the new expression into postfix
    currentExpressionPostfix = expression;
    currentPostfix = parse(expression, debug); 

    if (debug) {
        std::cout << "Current expression set to: " << expression << std::endl;
    }
}

#pragma endregion
