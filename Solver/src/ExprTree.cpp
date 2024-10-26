#include "pch.h"
#include "Token.h"
#include "Function.h"
#include "ExprNode.h"
#include "Exception.h"
#include "SymbolTable.h"
#include "MultiPassSimplifier.h"

namespace ExpressionTree {

#pragma region Helpers

size_t getFunctionArgCount(const std::string& functionName, const std::unordered_map<std::string, Function>& functions) {
    auto it = functions.find(functionName);
    if (it != functions.end()) {
        return it->second.argCount;
    }
    throw SolverException("Unknown function '" + functionName + "'");
}

double foldConstants(const std::string& op, double leftValue, double rightValue) {
    if (op == "+") {
        return leftValue + rightValue;
    } else if (op == "-") {
        return leftValue - rightValue;
    } else if (op == "*") {
        return leftValue * rightValue;
    } else if (op == "/") {
        if (rightValue == 0) {
            throw SolverException("Division by zero error.");
        }
        return leftValue / rightValue;
    } else if (op == "^") {
        return std::pow(leftValue, rightValue);
    }
    throw SolverException("Unknown operator: '" + op + "'");
}

ExprNode* applyBasicSimplifications(const std::string& op, ExprNode* leftNode, ExprNode* rightNode) {
    if (op == "+") {
        if (rightNode->type == NUMBER && rightNode->value == "0") {
            return new ExprNode(leftNode->type, leftNode->value); 
        }
        if (leftNode->type == NUMBER && leftNode->value == "0") {
            return new ExprNode(rightNode->type, rightNode->value);
        }
    } else if (op == "*") {
        if (rightNode->type == NUMBER && rightNode->value == "1") {
            return new ExprNode(leftNode->type, leftNode->value);
        }
        if (leftNode->type == NUMBER && leftNode->value == "1") {
            return new ExprNode(rightNode->type, rightNode->value);
        }
        if (rightNode->type == NUMBER && rightNode->value == "0") {
            return new ExprNode(NUMBER, "0");  // Multiplying by zero
        }
        if (leftNode->type == NUMBER && leftNode->value == "0") {
            return new ExprNode(NUMBER, "0");  // Multiplying by zero
        }
    }

    return nullptr;
}

#pragma endregion

#pragma region Parsing and Shunting Yard

int getPrecedence(const std::string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    if (op == "^") return 3;
    return 0;
}

bool isLeftAssociative(const std::string& op) {
    return op != "^";  // "^" is right-associative, all others are left-associative
}

void processOperatorStack(const Token& token, std::stack<Token>& operatorStack, std::queue<Token>& outputQueue) {
    while (!operatorStack.empty() &&
           ((isLeftAssociative(token.value) && getPrecedence(token.value) <= getPrecedence(operatorStack.top().value)) ||
            (!isLeftAssociative(token.value) && getPrecedence(token.value) < getPrecedence(operatorStack.top().value)))) {
        outputQueue.push(operatorStack.top());
        operatorStack.pop();
    }
    operatorStack.push(token);
}

void handleParentheses(std::stack<Token>& operatorStack, std::queue<Token>& outputQueue) {
    while (!operatorStack.empty() && operatorStack.top().value != "(") {
        outputQueue.push(operatorStack.top());
        operatorStack.pop();
    }
    if (operatorStack.empty()) {
        throw SolverException("Mismatched parentheses.");
    }
    operatorStack.pop();  // Pop the left parenthesis
}

void handleFunctionArgumentSeparator(std::stack<Token>& operatorStack, std::queue<Token>& outputQueue, std::stack<int>& argumentCounts) {
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

std::queue<Token> shuntingYard(const std::vector<Token>& tokens) {
    std::queue<Token> outputQueue;
    std::stack<Token> operatorStack;
    std::stack<int> argumentCounts;

    for (const auto& token : tokens) {
        if (token.type == NUMBER || token.type == VARIABLE) {
            outputQueue.push(token);
        } else if (token.type == FUNCTION) {
            operatorStack.push(token);
            argumentCounts.push(1);
        } else if (token.type == OPERATOR) {
            processOperatorStack(token, operatorStack, outputQueue);
        } else if (token.value == "(") {
            operatorStack.push(token);
        } else if (token.value == ")") {
            handleParentheses(operatorStack, outputQueue);
            if (!operatorStack.empty() && operatorStack.top().type == FUNCTION) {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
            }
        } else if (token.value == ",") {
            handleFunctionArgumentSeparator(operatorStack, outputQueue, argumentCounts);
        }
    }

    // Pop all remaining operators in the stack to the output queue
    while (!operatorStack.empty()) {
        if (operatorStack.top().value == "(" || operatorStack.top().value == ")") {
            throw SolverException("Mismatched parentheses.");
        }
        outputQueue.push(operatorStack.top());
        operatorStack.pop();
    }

    return outputQueue;
}

#pragma endregion

#pragma region Expression Tree Construction

ExprNode* processFunction(const Token& token, std::stack<ExprNode*>& nodeStack, const std::unordered_map<std::string, Function>& functions) {
    size_t argCount = getFunctionArgCount(token.value, functions);

    if (nodeStack.size() < argCount) {
        throw SolverException("Error: Not enough operands for function '" + token.value + "'.");
    }

    std::vector<ExprNode*> arguments(argCount);
    for (size_t i = 0; i < argCount; ++i) {
        arguments[argCount - i - 1] = nodeStack.top();  // Use raw pointers
        nodeStack.pop();
    }

    ExprNode* node = new ExprNode(token.type, token.value);
    node->arguments = std::move(arguments);
    return node;
}

ExprNode* processOperator(const Token& token, std::stack<ExprNode*>& nodeStack) {
    if (nodeStack.size() < 2) {
        throw SolverException("Error: Not enough operands for operator '" + token.value + "'");
    }

    ExprNode* rightNode = nodeStack.top();
    nodeStack.pop();
    ExprNode* leftNode = nodeStack.top();
    nodeStack.pop();

    ExprNode* node = new ExprNode(token.type, token.value);
    node->left = leftNode;
    node->right = rightNode;
    return node;
}

ExprNode* parseExpression(const std::vector<Token>& tokens, const std::unordered_map<std::string, Function>& functions) {
    auto postfixQueue = shuntingYard(tokens);
    std::stack<ExprNode*> nodeStack;

    while (!postfixQueue.empty()) {
        Token token = postfixQueue.front();
        postfixQueue.pop();

        if (token.type == NUMBER || token.type == VARIABLE) {
            nodeStack.push(new ExprNode(token.type, token.value));
        } else if (token.type == OPERATOR) {
            nodeStack.push(processOperator(token, nodeStack));
        } else if (token.type == FUNCTION) {
            nodeStack.push(processFunction(token, nodeStack, functions));
        }
    }

    if (nodeStack.size() != 1) {
        throw SolverException("Error: The expression could not be parsed into an expression tree.");
    }

    return nodeStack.top();
}

#pragma endregion

#pragma region Expression Tree Simplification

ExprNode* simplify(ExprNode* node, const SymbolTable& symbolTable) {
    MultiPassSimplifier simplifier(symbolTable);
    return simplifier.simplify(node);
}

#pragma endregion

}  // namespace ExpressionTree
