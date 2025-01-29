#include "solver.h"
#include "exception.h"
#include "postfix.h"
#include "validator.h"
#include "debug.h"
#include "tokenizer.h"

Solver::Solver(size_t exprCacheSize)
    : expressionCache(exprCacheSize) {
    PROFILE_BEGIN_SESSION("SOLVER", "solver.json");
    registerBuiltInFunctions();
}

void Solver::setUseCache(bool useCache) {
    PROFILE_FUNCTION();
    cacheEnabled = useCache;
}

void Solver::invalidateCaches() {
    PROFILE_FUNCTION();
    if (cacheEnabled) {
        expressionCache.clear();
    }
}

void Solver::clearCache() {
    PROFILE_FUNCTION();
    expressionCache.clear();
}

void Solver::declareConstant(const std::string& name, double value) {
    PROFILE_FUNCTION();
    symbolTable.declareConstant(name, value);
    invalidateCaches();
}

void Solver::declareVariable(const std::string& name, double value) {
    PROFILE_FUNCTION();
    symbolTable.declareVariable(name, value);
    invalidateCaches();
}

#pragma region Evaluation

std::vector<Token> Solver::parse(const std::string& expression, bool debug) {
    PROFILE_FUNCTION();
    auto tokens = Tokenizer::tokenize(expression);  // Tokenize the input expression
    auto postfix = Postfix::shuntingYard(tokens);  // Convert to postfix using Shunting Yard
    printPostfix(postfix);
    auto flatened = Postfix::flattenPostfix(postfix, functions);

    if (debug) {
        std::cout << "Parsed expression into postfix notation: ";
        printPostfix(flatened);
    }

    return flatened;
}

double Solver::evaluate(const std::string& expression, bool debug) {
    PROFILE_FUNCTION();
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
    PROFILE_FUNCTION();
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

#pragma endregion

#pragma region Functions

void Solver::registerPredefinedFunction(const std::string& name, const FunctionCallback& callback, size_t argCount) {
    PROFILE_FUNCTION();
    auto result = functions.emplace(name, Function(callback, argCount));
    if (!result.second) {
        throw SolverException("Function '" + name + "' already exists.");
    }
}

void Solver::declareFunction(const std::string& name, const std::vector<std::string>& args, const std::string& expression) {
    PROFILE_FUNCTION();
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

// std::unordered_map<std::string, std::pair<std::vector<std::string>, bool>> Solver::listFunctions() const {
//     std::unordered_map<std::string, std::pair<std::vector<std::string>, bool>> functionList;
//     for (const auto& [name, function] : functions) {
//         functionList[name] = std::make_pair(function.args, function.isPredefined);
//     }
//     return functionList;
// }

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
