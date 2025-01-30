#include "solver.h"
#include "exception.h"
#include "postfix.h"
#include "validator.h"
#include "debug.h"
#include "tokenizer.h"
#include "ast.h"

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

void Solver::declareConstant(const std::string& name, double value) {
    PROFILE_FUNCTION()
    symbolTable.declareConstant(name, value);
    invalidateCaches();
}

void Solver::declareVariable(const std::string& name, double value) {
    PROFILE_FUNCTION()
    symbolTable.declareVariable(name, value);
    invalidateCaches();
}

#pragma region Evaluation

std::vector<Token> Solver::parse(const std::string& expression, bool debug) {
    auto tokens   = Tokenizer::tokenize(expression);
    auto postfix  = Postfix::shuntingYard(tokens);
    auto flattened = Postfix::flattenPostfix(postfix, functions);
    auto inlined = Simplification::replaceConstantSymbols(flattened, symbolTable);

    ASTNode * root = AST::buildASTFromPostfix(inlined, functions);

    AST::printAST(root);

    ASTNode * simp = Simplification::simplifyAST(root, functions);
    std::cout << "\n\nSimplified: \n";

    AST::printAST(simp);

    // Now do a simplification pass
    auto simplified = Simplification::fullySimplifyPostfix(inlined, functions);

    if (debug) {
        std::cout << "Flattened postfix: ";
        printPostfix(inlined);
        std::cout << "Simplified postfix: ";
        printPostfix(simplified);
    }

    return simplified; 
}

double Solver::evaluate(const std::string& expression, bool debug) {
    PROFILE_FUNCTION()
    setCurrentExpression(expression, debug);

    std::size_t cacheKey = generateCacheKey(expression, {});
    if (cacheEnabled) {
        if (double* cachedResult = expressionCache.get(cacheKey)) {
            return *cachedResult;  // Return cached result if found
        }
    }

    double result = Postfix::evaluatePostfix(currentPostfix, symbolTable, functions);

    if (cacheEnabled) {
        expressionCache.put(cacheKey, result);  // Cache the result
    }

    return result;
}

std::vector<double> Solver::evaluateForRange(const std::string& variable, const std::vector<double>& values, const std::string& expression, bool debug) {
    PROFILE_FUNCTION()
    setCurrentExpression(expression, debug);

    std::vector<double> results;
    results.reserve(values.size());

    if (!Validator::isValidName(variable)) {
        throw SolverException("Invalid variable name '" + variable + "'.");
    }

    for (double value : values) {
        PROFILE_SCOPE("EvaluateRangeLoop");
        symbolTable.declareVariable(variable, value, true);

        try {
            double result = Postfix::evaluatePostfix(currentPostfix, symbolTable, functions);
            results.push_back(result);
        } catch (const SolverException& e) {
            std::cerr << "Error evaluating expression for " << variable << " = " << value << ": " << e.what() << std::endl;
            results.push_back(std::nan(""));
        }
    }

    return results;
}


std::vector<double> Solver::evaluateForRanges(const std::vector<std::string>& variables, const std::vector<std::vector<double>>& valuesSets, const std::string& expression, bool debug) {
    PROFILE_FUNCTION()

    // Basic validation:
    if (variables.size() != valuesSets.size()) {
        throw SolverException("Mismatch in number of variables vs. value ranges.");
    }
    for (auto& var : variables) {
        if (!Validator::isValidName(var)) {
            throw SolverException("Invalid variable name '" + var + "'.");
        }
    }

    // Parse expression only once for efficiency
    setCurrentExpression(expression, debug);

    // Compute the total number of combinations in the cartesian product
    // E.g., if x has 3 values and y has 2, totalCombinations = 3 * 2 = 6.
    size_t totalCombinations = 1;
    for (const auto& vals : valuesSets) {
        totalCombinations *= vals.size();
    }

    // Prepare output vector
    std::vector<double> results;
    results.reserve(totalCombinations);

    // If there's only one variable, we can do a simple loop.
    // Otherwise, we'll do an iterative cartesian product approach.
    // We'll track an index array, one index per variable.
    const size_t nVars = variables.size();
    std::vector<size_t> indices(nVars, 0);  // current position in each valuesSets[i]

    // We'll iterate from combination #0 to combination #(totalCombinations-1).
    // For each iteration, assign symbolTable values and evaluate.
    for (size_t count = 0; count < totalCombinations; ++count) {
        // Assign each variable to its current index's value
        for (size_t i = 0; i < nVars; ++i) {
            symbolTable.declareVariable(variables[i], valuesSets[i][indices[i]], true);
        }

        // Evaluate and capture the result
        try {
            double val = Postfix::evaluatePostfix(currentPostfix, symbolTable, functions);
            results.push_back(val);
        } catch (const SolverException& e) {
            // On error, store NaN to keep indices consistent
            std::cerr << "Error evaluating expression for combination "
                      << (count+1) << " of " << totalCombinations << ": "
                      << e.what() << std::endl;
            results.push_back(std::nan(""));
        }

        // Increment the 'indices' in a multi-digit manner (the last variable changes fastest)
        for (int varIndex = static_cast<int>(nVars) - 1; varIndex >= 0; --varIndex) {
            indices[varIndex]++;
            if (indices[varIndex] < valuesSets[varIndex].size()) {
                // We successfully incremented without overflow; break
                break;
            } else {
                // Reset this index to 0 and carry over to the next variable
                indices[varIndex] = 0;
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
        
        // std::cout << "This is the postfix of the expression: " << expression << std::endl;
        // printPostfix(flattened);

        // Store the function with its inlined postfix and argument names
        functions[name] = Function(flattened, args);
    } catch (const std::exception& e) {
        throw SolverException("Error defining function '" + name + "': " + e.what());
    }
}

#pragma endregion

#pragma region Helpers

// Return the list of constants
std::unordered_map<std::string, double> Solver::listConstants() const {
    return symbolTable.getConstants();
}

// Return the list of variables
std::unordered_map<std::string, double> Solver::listVariables() const {
    return symbolTable.getVariables();
}

void Solver::setCurrentExpression(const std::string& expression, bool debug) {
    // Check if the expression is the same and the currentPostfix is not empty
    if (expression == currentExpression && !currentPostfix.empty()) {
        return;
    }

    // Otherwise, parse the new expression into postfix
    currentExpression = expression;
    currentPostfix = parse(expression, debug); 

    if (debug) {
        std::cout << "Current expression set to: " << expression << std::endl;
    }
}


#pragma endregion
