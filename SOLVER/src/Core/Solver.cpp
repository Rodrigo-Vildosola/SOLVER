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

std::unique_ptr<ExprNode> Solver::parseExpression(std::vector<Token> tokens) {
    auto postfixQueue = shuntingYard(tokens);
    std::stack<std::unique_ptr<ExprNode>> nodeStack;

    while (!postfixQueue.empty()) {
        Token token = postfixQueue.front();
        postfixQueue.pop();

        if (token.type == NUMBER || token.type == VARIABLE) {
            nodeStack.push(std::make_unique<ExprNode>(token.value));
        } else if (token.type == OPERATOR) {
            if (nodeStack.size() < 2) {
                std::cerr << "Error: Not enough operands for operator: " << token.value << std::endl;
                return nullptr; // Error: not enough operands
            }
            auto node = std::make_unique<ExprNode>(token.value);
            node->right = std::move(nodeStack.top());
            nodeStack.pop();
            node->left = std::move(nodeStack.top());
            nodeStack.pop();
            nodeStack.push(std::move(node));
        } else if (token.type == FUNCTION) {
            if (token.value == "neg") {
                // Handle unary minus
                if (nodeStack.empty()) {
                    std::cerr << "Error: Not enough operands for function: neg" << std::endl;
                    return nullptr; // Error: no operand
                }
                auto node = std::make_unique<ExprNode>("-");
                node->left = std::make_unique<ExprNode>("0"); // Negation is treated as 0 - x
                node->right = std::move(nodeStack.top());
                nodeStack.pop();
                nodeStack.push(std::move(node));
            } else {
                if (nodeStack.empty()) {
                    std::cerr << "Error: No operands for function: " << token.value << std::endl;
                    return nullptr; // Error: no operand
                }
                auto node = std::make_unique<ExprNode>(token.value);
                node->arguments.push_back(std::move(nodeStack.top()));
                nodeStack.pop();
                nodeStack.push(std::move(node));
            }
        }
    }

    if (nodeStack.empty()) {
        std::cerr << "Error: The expression could not be parsed into an expression tree." << std::endl;
        return nullptr; // Error: empty stack
    }

    return std::move(nodeStack.top());  // Return the unique_ptr, ownership transferred
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

double Solver::evaluateNode(const std::unique_ptr<ExprNode>& node) {
    if (!node) {
        std::cerr << "Error: Trying to evaluate a null node." << std::endl;
        throw std::runtime_error("Error: Trying to evaluate a null node.");
    }

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
        if (rightValue == 0) {
            std::cerr << "Error: Division by zero." << std::endl;
            throw std::runtime_error("Division by zero.");
        }
        return leftValue / rightValue;
    } else if (node->value == "^") {
        return std::pow(leftValue, rightValue);
    } else {
        throw std::invalid_argument("Unknown operator: " + node->value);
    }
}


double Solver::evaluate(const std::string& expression) {
    auto tokens = tokenize(expression);
    auto exprTree = parseExpression(tokens);
    if (!exprTree) {
        std::cerr << "Error: Failed to parse the expression into a tree." << std::endl;
        throw std::runtime_error("Failed to parse the expression.");
    }
    double result = evaluateNode(exprTree);
    return result;
}


void Solver::extractLinearTerms(const std::string& expression, double& coefficient, double& constant) {
    auto tokens = tokenize(expression);
    auto postfixQueue = shuntingYard(tokens);
    std::stack<double> values;
    std::stack<double> xCoefficients;

    // Initialize coefficient and constant
    coefficient = 0;
    constant = 0;

    while (!postfixQueue.empty()) {
        Token token = postfixQueue.front();
        postfixQueue.pop();

        if (token.type == NUMBER) {
            // Push numeric value onto the values stack
            values.push(std::stod(token.value));
        } else if (token.type == VARIABLE) {
            if (token.value == "x") {
                // Variable 'x' is treated as coefficient 1 for 'x'
                xCoefficients.push(1);
            }
        } else if (token.type == OPERATOR) {
            if (token.value == "+") {
                if (!xCoefficients.empty()) {
                    coefficient += xCoefficients.top();
                    xCoefficients.pop();
                } else {
                    if (values.size() < 2) throw std::runtime_error("Invalid expression: not enough operands for addition.");
                    double rightValue = values.top();
                    values.pop();
                    double leftValue = values.top();
                    values.pop();
                    values.push(leftValue + rightValue);
                }
            } else if (token.value == "-") {
                if (!xCoefficients.empty()) {
                    coefficient -= xCoefficients.top();
                    xCoefficients.pop();
                } else {
                    if (values.size() < 2) throw std::runtime_error("Invalid expression: not enough operands for subtraction.");
                    double rightValue = values.top();
                    values.pop();
                    double leftValue = values.top();
                    values.pop();
                    values.push(leftValue - rightValue);
                }
            } else if (token.value == "*") {
                if (xCoefficients.size() == 1 && values.size() >= 1) {
                    double multiplier = values.top();
                    values.pop();
                    xCoefficients.top() *= multiplier;
                } else if (values.size() >= 2) {
                    double rightValue = values.top();
                    values.pop();
                    double leftValue = values.top();
                    values.pop();
                    values.push(leftValue * rightValue);
                } else {
                    throw std::runtime_error("Invalid expression: not enough operands for multiplication.");
                }
            } else {
                throw std::invalid_argument("Unsupported operator in linear extraction.");
            }
        }
    }

    // Summing up values and coefficients
    while (!values.empty()) {
        constant += values.top();
        values.pop();
    }
    while (!xCoefficients.empty()) {
        coefficient += xCoefficients.top();
        xCoefficients.pop();
    }
}

double Solver::solveLinearEquation(const std::string& equation) {
    size_t equalsPos = equation.find('=');
    if (equalsPos == std::string::npos) {
        throw std::invalid_argument("Equation must contain '='.");
    }

    std::string lhs = equation.substr(0, equalsPos);
    std::string rhs = equation.substr(equalsPos + 1);

    double lhsCoefficient = 0, lhsConstant = 0;
    double rhsCoefficient = 0, rhsConstant = 0;

    extractLinearTerms(lhs, lhsCoefficient, lhsConstant);
    extractLinearTerms(rhs, rhsCoefficient, rhsConstant);

    // Bring all x terms to one side and constants to the other
    double totalCoefficient = lhsCoefficient - rhsCoefficient;
    double totalConstant = rhsConstant - lhsConstant;

    // Check for infinite solutions or no solution
    if (totalCoefficient == 0) {
        if (totalConstant == 0) {
            throw std::runtime_error("Infinite solutions.");
        } else {
            throw std::runtime_error("No solution.");
        }
    }

    // Solve for x
    double solution = totalConstant / totalCoefficient;
    return solution;
}
