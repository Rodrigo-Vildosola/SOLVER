#include "Solver.h"
#include "Exception.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

// Initialize the solver with built-in functions
Solver::Solver() {
    registerBuiltInFunctions();
}

// Register a predefined function callback
void Solver::registerPredefinedFunction(const std::string& name, const FunctionCallback& callback) {
    auto result = functions.emplace(name, Function(callback));
    if (!result.second) {
        // The function already exists; you can choose to overwrite or raise an error
        throw SolverException("Function '" + name + "' already exists.");
    }
}

// Declare constants through the SymbolTable
void Solver::declareConstant(const std::string& name, double value) {
    symbolTable.declareConstant(name, value);
}

// Declare variables through the SymbolTable
void Solver::declareVariable(const std::string& name, double value) {
    symbolTable.declareVariable(name, value);
}

// Declare user-defined functions
void Solver::declareFunction(const std::string& name, const std::vector<std::string>& args, const std::string& expression) {
    validateFunctionExpression(expression, args);
    auto result = functions.emplace(name, Function(args, expression));
    if (!result.second) {
        throw SolverException("Function '" + name + "' already exists.");
    }
}

// Validate a function expression (make sure all variables/constants are declared)
void Solver::validateFunctionExpression(const std::string& expression, const std::vector<std::string>& args) {
    auto tokens = tokenize(expression);
    for (const auto& token : tokens) {
        if (token.type == VARIABLE) {
            if (std::find(args.begin(), args.end(), token.value) == args.end() && !symbolTable.isConstant(token.value)) {
                throw SolverException("Variable '" + token.value + "' is not declared in the function scope or as a constant.");
            }
        }
    }
}

// Evaluate a function or call a predefined function
double Solver::evaluateFunction(const std::string& func, const std::vector<double>& args) {
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
        return function.callback(args);
    }

    // Handle user-defined functions
    if (function.args.size() != args.size()) {
        throw SolverException("Function argument count mismatch for '" + func + "'. Expected " +
                              std::to_string(function.args.size()) + " but got " + std::to_string(args.size()) + ".");
    }

    // Backup variables and set function arguments as variables
    auto savedVariables = symbolTable.getVariables();
    symbolTable.clearVariables();
    for (size_t i = 0; i < function.args.size(); ++i) {
        symbolTable.declareVariable(function.args[i], args[i]);
    }

    double result = evaluate(function.expression);
    symbolTable.restoreVariables(savedVariables);
    return result;
}

// Evaluate a mathematical expression
double Solver::evaluate(const std::string& expression, bool debug) {
    auto tokens = tokenize(expression);
    auto exprTree = ExpressionTree::parseExpression(tokens);
    exprTree = ExpressionTree::simplify(std::move(exprTree));

    if (debug) {
        std::cout << "Expression tree:\n";
        printTree(exprTree.get());
    }

    return evaluateNode(exprTree);
}

// Evaluate a range of values for an expression
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

// Helper to evaluate a node in the expression tree
double Solver::evaluateNode(const std::unique_ptr<ExprNode>& node) {
    if (!node->left && !node->right && node->arguments.empty()) {
        try {
            // Try looking up the symbol in the symbol table.
            return symbolTable.lookupSymbol(node->value);
        } catch (const SolverException&) {
            // If not found in the symbol table, try converting the value to a number (using std::stod).
            try {
                return std::stod(node->value);
            } catch (const std::invalid_argument&) {
                throw SolverException("Invalid argument: Unable to convert '" + node->value + "' to a number or find it as a variable/constant.");
            }
        }
    }
    
    if (!node->arguments.empty()) {
        std::vector<double> args;
        for (const auto& argNode : node->arguments) {
            args.push_back(evaluateNode(argNode));
        }
        return evaluateFunction(node->value, args);
    }

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
