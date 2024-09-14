#include "Solver.h"
#include <stdexcept>
#include <cmath> // For mathematical functions like sin, cos, etc.
#include <stack>
#include <iostream>
#include <unordered_set>

Solver::Solver() {}

void Solver::declareVariable(const std::string& name, double value, bool isGlobal) {
    if (isGlobal) {
        globalSymbols[name] = value;
    } else {
        localSymbols[name] = value;
    }
}

void Solver::declareFunction(const std::string& name, const std::vector<std::string>& args, const std::string& expression) {
    validateFunctionExpression(expression, args);
    functions[name] = {args, expression};
}

void Solver::validateFunctionExpression(const std::string& expression, const std::vector<std::string>& args) {
    auto tokens = tokenize(expression);
    for (const auto& token : tokens) {
        if (token.type == VARIABLE) {
            if (std::find(args.begin(), args.end(), token.value) == args.end() &&
                globalSymbols.find(token.value) == globalSymbols.end()) {
                throw std::invalid_argument("Variable " + token.value + " is not declared in the function scope or globally.");
            }
        }
    }
}

ExprNode* Solver::parseExpression(const std::vector<Token> tokens) {

    auto postfixQueue = shuntingYard(tokens);
    std::stack<ExprNode*> nodeStack;

    while (!postfixQueue.empty()) {
        Token token = postfixQueue.front();
        postfixQueue.pop();

        if (token.type == NUMBER || token.type == VARIABLE) {
            nodeStack.push(new ExprNode(token.value));
        } else if (token.type == OPERATOR) {
            ExprNode* node = new ExprNode(token.value);
            node->right = nodeStack.top();  // Take ownership of the top node
            nodeStack.pop();
            node->left = nodeStack.top();   // Take ownership of the second node
            nodeStack.pop();
            nodeStack.push(node);
        } else if (token.type == FUNCTION) {
            ExprNode* node = new ExprNode(token.value);
            node->arguments.push_back(nodeStack.top());  // Take ownership of the top node
            nodeStack.pop();
            nodeStack.push(node);
        }
    }

    return nodeStack.top();  // The caller is responsible for freeing the returned pointer
}

double Solver::evaluateFunction(const std::string& func, const std::vector<double>& args) {
    if (standardFunctions.find(func) != standardFunctions.end()) {
        if (func == "sin") return std::sin(args[0]);
        if (func == "cos") return std::cos(args[0]);
        if (func == "tan") return std::tan(args[0]);
        if (func == "exp") return std::exp(args[0]);
        if (func == "log") return std::log(args[0]);
        if (func == "sqrt") return std::sqrt(args[0]);
    } else if (functions.find(func) != functions.end()) {
        auto funcDef = functions[func];
        if (funcDef.args.size() != args.size()) {
            throw std::invalid_argument("Function argument count mismatch: " + func);
        }

        auto savedLocalSymbols = localSymbols;

        localSymbols.clear();
        for (size_t i = 0; i < funcDef.args.size(); ++i) {
            localSymbols[funcDef.args[i]] = args[i];
        }

        double result = evaluate(funcDef.expression);

        localSymbols = savedLocalSymbols;

        return result;
    }
    throw std::invalid_argument("Unknown function: " + func);
}

double Solver::evaluateNode(const ExprNode* node) {
    if (!node->left && !node->right && node->arguments.empty()) {
        if (localSymbols.find(node->value) != localSymbols.end()) {
            return localSymbols.at(node->value);
        } else if (globalSymbols.find(node->value) != globalSymbols.end()) {
            return globalSymbols.at(node->value);
        }
        try {
            return std::stod(node->value);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << node->value << std::endl;
            throw;
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

    if (node->value == "+") {
        return leftValue + rightValue;
    } else if (node->value == "-") {
        return leftValue - rightValue;
    } else if (node->value == "*") {
        return leftValue * rightValue;
    } else if (node->value == "/") {
        return leftValue / rightValue;
    } else if (node->value == "^") {
        return std::pow(leftValue, rightValue);
    } else {
        throw std::invalid_argument("Unknown operator: " + node->value);
    }
}

double Solver::evaluate(const std::string& expression) {
    auto tokens = tokenize(expression);
    ExprNode* exprTree = parseExpression(tokens);
    double result = evaluateNode(exprTree);
    return result;
}
