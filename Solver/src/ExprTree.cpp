#include "ExprTree.h"

size_t ExpressionTree::getFunctionArgCount(const std::string& functionName, const std::unordered_map<std::string, Function>& functions) {
    auto it = functions.find(functionName);
    if (it != functions.end()) {
        return it->second.argCount;
    }
    throw SolverException("Unknown function '" + functionName + "'");
}

// Shunting yard algorithm for parsing infix to postfix
std::queue<Token> ExpressionTree::shuntingYard(const std::vector<Token>& tokens) {
    std::queue<Token> outputQueue;
    std::stack<Token> operatorStack;
    std::stack<int> argumentCounts;

    auto precedence = [](const std::string& op) {
        if (op == "+" || op == "-") return 1;
        if (op == "*" || op == "/") return 2;
        if (op == "^") return 3;
        return 0;
    };

    auto isLeftAssociative = [](const std::string& op) {
        if (op == "^") return false; 
        return true; 
    };

    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        if (token.type == NUMBER || token.type == VARIABLE) {
            outputQueue.push(token);
        } else if (token.type == FUNCTION) {
            operatorStack.push(token);
            argumentCounts.push(1); 
        } else if (token.type == OPERATOR) {
            while (!operatorStack.empty() &&
                    ((isLeftAssociative(token.value) && precedence(token.value) <= precedence(operatorStack.top().value)) ||
                    (!isLeftAssociative(token.value) && precedence(token.value) < precedence(operatorStack.top().value)))) {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
            }
            operatorStack.push(token);
        } else if (token.value == "(") {
            operatorStack.push(token);
        } else if (token.value == ")") {
            // Pop until the left parenthesis is found
            while (!operatorStack.empty() && operatorStack.top().value != "(") {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
            }
            if (operatorStack.empty()) {
                throw SolverException("Mismatched parentheses.");
            }
            operatorStack.pop(); // Pop the left parenthesis

            // If a function is on top of the stack, add it to the output queue
            if (!operatorStack.empty() && operatorStack.top().type == FUNCTION) {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
            }
        } else if (token.value == ",") {
            // Comma should pop the operators until a left parenthesis is found
            while (!operatorStack.empty() && operatorStack.top().value != "(") {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
            }
            if (operatorStack.empty()) {
                throw SolverException("Mismatched parentheses or misplaced comma.");
            }
            // Increase the argument count for the current function
            if (!argumentCounts.empty()) {
                argumentCounts.top()++;
            }
        }
    }

    while (!operatorStack.empty()) {
        if (operatorStack.top().value == "(" || operatorStack.top().value == ")") {
            throw SolverException("Mismatched parentheses.");
        }
        outputQueue.push(operatorStack.top());
        operatorStack.pop();
    }

    return outputQueue;
}

std::unique_ptr<ExprNode> ExpressionTree::parseExpression(const std::vector<Token>& tokens, const std::unordered_map<std::string, Function>& functions) {
    auto postfixQueue = shuntingYard(tokens);
    std::stack<std::unique_ptr<ExprNode>> nodeStack;

    while (!postfixQueue.empty()) {
        Token token = postfixQueue.front();
        postfixQueue.pop();

        if (token.type == NUMBER || token.type == VARIABLE) {
            // Push number or variable directly into the node stack
            nodeStack.push(std::make_unique<ExprNode>(token.type, token.value));

        } else if (token.type == OPERATOR) {
            // For operators, pop the operands
            if (nodeStack.size() < 2) {
                throw SolverException("Error: Not enough operands for operator '" + token.value + "'");
            }
            auto rightNode = std::move(nodeStack.top());
            nodeStack.pop();
            auto leftNode = std::move(nodeStack.top());
            nodeStack.pop();

            // If both left and right nodes are constants, we can fold them into a single constant
            if (leftNode->type == NUMBER && rightNode->type == NUMBER) {
                double leftValue = std::stod(leftNode->value);
                double rightValue = std::stod(rightNode->value);
                double result;

                // Perform the operation based on the operator
                if (token.value == "+") {
                    result = leftValue + rightValue;
                } else if (token.value == "-") {
                    result = leftValue - rightValue;
                } else if (token.value == "*") {
                    result = leftValue * rightValue;
                } else if (token.value == "/") {
                    if (rightValue == 0) {
                        throw SolverException("Division by zero error.");
                    }
                    result = leftValue / rightValue;
                } else if (token.value == "^") {
                    result = std::pow(leftValue, rightValue);
                } else {
                    throw SolverException("Unknown operator: '" + token.value + "'");
                }

                // Push the computed constant as a new node
                nodeStack.push(std::make_unique<ExprNode>(NUMBER, std::to_string(result)));
            } else {
                // If not both are constants, create an operator node
                auto node = std::make_unique<ExprNode>(token.type, token.value);
                node->left = std::move(leftNode);
                node->right = std::move(rightNode);
                nodeStack.push(std::move(node));
            }

        } else if (token.type == FUNCTION) {
            // Handle functions
            size_t argCount = getFunctionArgCount(token.value, functions);
            if (nodeStack.size() < argCount) {
                throw SolverException("Error: Not enough operands for function '" + token.value + "'.");
            }

            std::vector<std::unique_ptr<ExprNode>> arguments(argCount);
            for (size_t i = 0; i < argCount; ++i) {
                arguments[argCount - i - 1] = std::move(nodeStack.top());
                nodeStack.pop();
            }

            auto node = std::make_unique<ExprNode>(token.type, token.value);
            node->arguments = std::move(arguments);
            nodeStack.push(std::move(node));
        }
    }

    if (nodeStack.size() != 1) {
        throw SolverException("Error: The expression could not be parsed into an expression tree.");
    }

    return std::move(nodeStack.top());
}


