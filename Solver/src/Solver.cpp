#include "Solver.h"
#include "Exception.h"
#include "ExprTree.h"
#include "Validator.h"
#include "Debug.h"
#include "Tokenizer.h"

Solver::Solver(size_t exprCacheSize, size_t funcCacheSize)
    : expressionCache(exprCacheSize), functionCache(funcCacheSize), currentExprTree(nullptr)  {
    registerBuiltInFunctions();
}

void Solver::setUseCache(bool useCache) {
    cacheEnabled = useCache;
}

void Solver::invalidateCaches() {
    if (cacheEnabled) {
        expressionCache.clear();
        functionCache.clear();
    }
}

void Solver::clearCache() {
    expressionCache.clear();
    functionCache.clear();
}

void Solver::declareConstant(const std::string& name, double value) {
    symbolTable.declareConstant(name, value);
    invalidateCaches();
}

void Solver::declareVariable(const std::string& name, double value) {
    symbolTable.declareVariable(name, value);
    invalidateCaches();
}

#pragma region Evaluation

std::unique_ptr<ExprNode> Solver::parse(const std::string& expression, bool debug) {

    auto tokens = Tokenizer::tokenize(expression);

    auto exprTree = ExpressionTree::parseExpression(tokens, functions);

    exprTree = ExpressionTree::simplify(std::move(exprTree), symbolTable);

    if (debug) {
        std::cout << std::endl;
        std::cout << "Expression tree after parsing and simplifying:\n";
        printTree(exprTree.get(), std::cout, functions);
        std::cout << std::endl;
    }

    return exprTree; 
}



double Solver::evaluate(const std::string& expression, bool debug) {
    setCurrentExpression(expression, debug);

    std::string cacheKey = generateCacheKey(expression, {});

    if (cacheEnabled) {
        if (double* cachedResult = expressionCache.get(cacheKey)) {
            return *cachedResult;  // Return cached result if found
        }
    }

    double result = evaluateNode(currentExprTree);

    if (cacheEnabled) {
        expressionCache.put(cacheKey, result);  // Cache the result
    }

    return result;
}


std::vector<double> Solver::evaluateForRange(const std::string& variable, const std::vector<double>& values, const std::string& expression, bool debug) {
    std::vector<double> results;

    auto exprTree = parse(expression, debug);

    for (double value : values) {
        declareVariable(variable, value);

        try {
            double result = evaluateNode(exprTree);
            results.push_back(result);

        } catch (const SolverException& e) {
            std::cerr << RED << "Error evaluating expression with '" << variable << "' = " << value << ": " << e.what() << RESET << std::endl;
            results.push_back(std::nan(""));
        }
    }

    return results;
}

double Solver::evaluateNode(const std::unique_ptr<ExprNode>& node) {
    if (node->type == NUMBER) {
        double val = std::stod(node->value);
        return val;
    } else if (node->type == VARIABLE) {
        return symbolTable.lookupSymbol(node->value);
    }

    if (node->type == FUNCTION) {
        std::vector<double> args;
        for (const auto& argNode : node->arguments) {
            args.push_back(evaluateNode(argNode));
        }
        return evaluateFunction(node->value, args);
    }

    if (node->type == OPERATOR) {
        double leftValue = evaluateNode(node->left);
        double rightValue = evaluateNode(node->right);

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

#pragma endregion

#pragma region Functions

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
        auto exprTree = parse(expression);

        auto result = functions.emplace(name, Function(args, expression));
        if (!result.second) {
            throw SolverException("Function '" + name + "' already exists.");
        }
        result.first->second.exprTree = std::move(exprTree);

    } catch (const std::exception& e) {
        throw SolverException("Invalid expression for function '" + name + "': " + e.what());
    }
}

double Solver::evaluateFunction(const std::string& func, const std::vector<double>& args) {
    std::string cacheKey = generateCacheKey(func, args);

    if (cacheEnabled) {
        if (double* cachedResult = functionCache.get(cacheKey)) {
            return *cachedResult;  // Return cached result if found
        }
    }


    auto it = functions.find(func);
    if (it == functions.end()) {
        throw SolverException("Unknown function: '" + func + "'.");
    }

    const Function& function = it->second;

    if (function.isPredefined) {
        if (!function.callback) {
            throw SolverException("Invalid predefined function: '" + func + "'.");
        }
        double result = function.callback(args);
        if (cacheEnabled) {
            functionCache.put(cacheKey, result);  // Cache the result
        }
        return result;
    }

    if (function.args.size() != args.size()) {
        throw SolverException("Function argument count mismatch for '" + func + "'. Expected " +
                              std::to_string(function.args.size()) + " but got " + std::to_string(args.size()) + ".");
    }

    auto savedVariables = symbolTable.getVariables();
    symbolTable.clearVariables();
    for (size_t i = 0; i < function.args.size(); ++i) {
        symbolTable.declareVariable(function.args[i], args[i]);
    }

    double result = evaluateNode(function.exprTree);
    symbolTable.restoreVariables(savedVariables);

    if (cacheEnabled) {
        functionCache.put(cacheKey, result);  // Cache the result
    }

    return result;
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

std::unordered_map<std::string, std::pair<std::vector<std::string>, bool>> Solver::listFunctions() const {
    std::unordered_map<std::string, std::pair<std::vector<std::string>, bool>> functionList;
    for (const auto& [name, function] : functions) {
        functionList[name] = std::make_pair(function.args, function.isPredefined);
    }
    return functionList;
}

void Solver::setCurrentExpression(const std::string& expression, bool debug) {
    // If the new expression is the same as the current one, return early
    if (expression == currentExpression && currentExprTree) {
        return;
    }

    // Otherwise, parse the new expression and update the current expression and tree
    currentExpression = expression;
    currentExprTree = parse(expression, debug); 

    if (debug) {
        std::cout << "Current expression set to: " << expression << std::endl;
    }
}


#pragma endregion
