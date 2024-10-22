#include "Solver.h"
#include "Exception.h"
#include "ExprTree.h"
#include "Validator.h"
#include "Debug.h"

Solver::Solver() {
    registerBuiltInFunctions();
}

void Solver::registerPredefinedFunction(const std::string& name, const FunctionCallback& callback, size_t argCount) {
    auto result = functions.emplace(name, Function(callback, argCount));
    if (!result.second) {
        throw SolverException("Function '" + name + "' already exists.");
    }
}

void Solver::declareFunction(const std::string& name, const std::vector<std::string>& args, const std::string& expression) {
    if (!Validator::isValidName(name)) {
        throw SolverException("Invalid function name: '" + name + "'.");
    }

    Validator::isValidSyntax(expression);

    try {
        auto tokens = tokenize(expression);
        auto exprTree = ExpressionTree::parseExpression(tokens, functions);
    } catch (const std::exception& e) {
        throw SolverException("Invalid expression for function '" + name + "': " + e.what());
    }

    // Register the function without checking dependencies
    auto result = functions.emplace(name, Function(args, expression));
    if (!result.second) {
        throw SolverException("Function '" + name + "' already exists.");
    }
}

void Solver::declareConstant(const std::string& name, double value) {
    symbolTable.declareConstant(name, value);
    invalidateCaches();  // Invalidate the cache since constants have changed
}

void Solver::declareVariable(const std::string& name, double value) {
    symbolTable.declareVariable(name, value);
    invalidateCaches();  // Invalidate the cache since variables have changed
}

void Solver::invalidateCaches() {
    expressionCache.clear();  // Clear cached expressions
    functionCache.clear();    // Clear cached function results
}

void Solver::clearCache() {
    expressionCache.clear();
    functionCache.clear();
}


double Solver::evaluateFunction(const std::string& func, const std::vector<double>& args) {
    // Generate the cache key using the helper function
    std::string cacheKey = generateCacheKey(func, args);

    // Check if the result is already cached
    auto cachedResult = functionCache.find(cacheKey);
    if (cachedResult != functionCache.end()) {
        return cachedResult->second;  
    }

    // Evaluate the function if not cached
    auto it = functions.find(func);
    if (it == functions.end()) {
        throw SolverException("Unknown function: '" + func + "'.");
    }

    const Function& function = it->second;

    // Handle predefined functions
    if (function.isPredefined) {
        if (!function.callback) {
            throw SolverException("Invalid predefined function: '" + func + "'.");
        }
        double result = function.callback(args);  // Call the predefined function
        functionCache[cacheKey] = result;  // Cache the result
        return result;
    }

    // Handle user-defined functions
    if (function.args.size() != args.size()) {
        throw SolverException("Function argument count mismatch for '" + func + "'. Expected " +
                              std::to_string(function.args.size()) + " but got " + std::to_string(args.size()) + ".");
    }

    // Backup and set new variables (arguments)
    auto savedVariables = symbolTable.getVariables();
    symbolTable.clearVariables();
    for (size_t i = 0; i < function.args.size(); ++i) {
        symbolTable.declareVariable(function.args[i], args[i]);
    }

    // Evaluate the function expression
    double result = evaluate(function.expression);
    symbolTable.restoreVariables(savedVariables);

    // Cache the result
    functionCache[cacheKey] = result;

    return result;
}

double Solver::evaluate(const std::string& expression, bool debug) {
    // Generate the cache key using the helper function
    std::string cacheKey = generateCacheKey(expression, {});

    // Check if the expression result is cached
    auto cachedResult = expressionCache.find(cacheKey);
    if (cachedResult != expressionCache.end()) {
        return cachedResult->second;  // Return cached result
    }

    // Tokenize and parse the expression
    auto tokens = tokenize(expression);
    auto exprTree = ExpressionTree::parseExpression(tokens, functions);

    if (debug) {
        std::cout << "Expression tree:\n";
        printTree(exprTree.get());
        std::cout << "-------------------------\n";
    }

    // Evaluate the expression tree
    double result = evaluateNode(exprTree);

    // Cache the result of the expression
    expressionCache[cacheKey] = result;

    return result;
}


std::vector<double> Solver::evaluateForRange(const std::string& variable, const std::vector<double>& values, const std::string& expression, bool debug) {
    std::vector<double> results;
    for (double value : values) {
        declareVariable(variable, value);
        try {
            double result = evaluate(expression, debug);
            results.push_back(result);
        } catch (const SolverException& e) {
            std::cerr << "Error evaluating expression with '" << variable << "' = " << value << ": " << e.what() << std::endl;
            results.push_back(std::nan(""));
        }
    }
    return results;
}

double Solver::evaluateNode(const std::unique_ptr<ExprNode>& node) {
    // Handle constants and variables
    if (node->type == NUMBER) {
        return std::stod(node->value);  // Directly convert string to double
    } else if (node->type == VARIABLE) {
        return symbolTable.lookupSymbol(node->value);  // Lookup variable value
    }

    // Handle function calls
    if (node->type == FUNCTION) {
        std::vector<double> args;
        for (const auto& argNode : node->arguments) {
            args.push_back(evaluateNode(argNode));  // Evaluate all arguments
        }
        return evaluateFunction(node->value, args);  // Evaluate the function
    }

    // Handle binary operators
    if (node->type == OPERATOR) {
        double leftValue = evaluateNode(node->left);   // Evaluate left child
        double rightValue = evaluateNode(node->right); // Evaluate right child

        if (node->value == "+") return leftValue + rightValue;
        if (node->value == "-") return leftValue - rightValue;
        if (node->value == "*") return leftValue * rightValue;
        if (node->value == "/") {
            if (rightValue == 0) throw SolverException("Division by zero error.");
            return leftValue / rightValue;
        }
        if (node->value == "^") return std::pow(leftValue, rightValue);

        throw SolverException("Unknown operator: '" + node->value + "'.");
    }

    throw SolverException("Unsupported node type.");
}


// Return the list of constants
std::unordered_map<std::string, double> Solver::listConstants() const {
    return symbolTable.getConstants();
}

// Return the list of variables
std::unordered_map<std::string, double> Solver::listVariables() const {
    return symbolTable.getVariables();
}

std::unordered_map<std::string, std::pair<std::vector<std::string>, bool>> Solver::listFunctions() const {
    std::unordered_map<std::string, std::pair<std::vector<std::string>, bool>> functionList;
    for (const auto& [name, function] : functions) {
        functionList[name] = std::make_pair(function.args, function.isPredefined);
    }
    return functionList;
}
