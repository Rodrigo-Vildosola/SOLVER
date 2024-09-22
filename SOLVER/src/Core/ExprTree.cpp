#include "ExprTree.h"

// Shunting yard algorithm for parsing infix to postfix
std::queue<Token> ExpressionTree::shuntingYard(const std::vector<Token>& tokens) {
    std::queue<Token> outputQueue;
    std::stack<Token> operatorStack;
    
    auto precedence = [](const std::string& op) {
        if (op == "+" || op == "-") return 1;
        if (op == "*" || op == "/") return 2;
        if (op == "^") return 3;
        return 0;
    };

    auto isLeftAssociative = [](const std::string& op) {
        return (op != "^");
    };

    for (const auto& token : tokens) {
        if (token.type == NUMBER || token.type == VARIABLE) {
            outputQueue.push(token);
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
            while (!operatorStack.empty() && operatorStack.top().value != "(") {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
            }
            operatorStack.pop(); // Remove '('
        }
    }

    while (!operatorStack.empty()) {
        outputQueue.push(operatorStack.top());
        operatorStack.pop();
    }

    return outputQueue;
}

// Parse tokens into an expression tree
std::unique_ptr<ExprNode> ExpressionTree::parseExpression(const std::vector<Token>& tokens) {
    auto postfixQueue = shuntingYard(tokens);
    std::stack<std::unique_ptr<ExprNode>> nodeStack;

    while (!postfixQueue.empty()) {
        Token token = postfixQueue.front();
        postfixQueue.pop();

        if (token.type == NUMBER || token.type == VARIABLE) {
            nodeStack.push(std::make_unique<ExprNode>(token.value));
        } else if (token.type == OPERATOR) {
            if (nodeStack.size() < 2) {
                throw SolverException("Error: Not enough operands for operator '" + token.value + "'");
            }
            auto node = std::make_unique<ExprNode>(token.value);
            node->right = std::move(nodeStack.top());
            nodeStack.pop();
            node->left = std::move(nodeStack.top());
            nodeStack.pop();
            nodeStack.push(std::move(node));
        }
    }

    if (nodeStack.empty()) {
        throw SolverException("Error: Expression could not be parsed.");
    }

    return std::move(nodeStack.top());
}

// Simplify expression tree
std::unique_ptr<ExprNode> ExpressionTree::simplify(std::unique_ptr<ExprNode> node) {
    if (!node) return nullptr;

    node->left = simplify(std::move(node->left));
    node->right = simplify(std::move(node->right));

    if (node->value == "+") {
        if (isZero(node->right)) return std::move(node->left);
        if (isZero(node->left)) return std::move(node->right);
    } else if (node->value == "*") {
        if (isOne(node->right)) return std::move(node->left);
        if (isOne(node->left)) return std::move(node->right);
        if (isZero(node->right) || isZero(node->left)) return makeConstNode(0);
    }

    return node;
}

bool ExpressionTree::isZero(const std::unique_ptr<ExprNode>& node) {
    return node && node->value == "0";
}

bool ExpressionTree::isOne(const std::unique_ptr<ExprNode>& node) {
    return node && node->value == "1";
}

bool ExpressionTree::areEqual(const std::unique_ptr<ExprNode>& left, const std::unique_ptr<ExprNode>& right) {
    return left && right && left->value == right->value;
}

std::unique_ptr<ExprNode> ExpressionTree::makeConstNode(double value) {
    return std::make_unique<ExprNode>(std::to_string(value));
}

std::unique_ptr<ExprNode> ExpressionTree::makeMulNode(double coefficient, std::unique_ptr<ExprNode> node) {
    auto mulNode = std::make_unique<ExprNode>("*");
    mulNode->left = makeConstNode(coefficient);
    mulNode->right = std::move(node);
    return mulNode;
}
