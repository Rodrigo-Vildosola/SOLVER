#include "Solver.h"
#include <iostream>
#include <sstream>

bool Solver::isZero(const std::unique_ptr<ExprNode>& node) {
    return node && node->value == "0";
}

bool Solver::isOne(const std::unique_ptr<ExprNode>& node) {
    return node && node->value == "1";
}

bool Solver::areEqual(const std::unique_ptr<ExprNode>& left, const std::unique_ptr<ExprNode>& right) {
    return left && right && left->value == right->value;
}

std::unique_ptr<ExprNode> Solver::makeMulNode(double coefficient, std::unique_ptr<ExprNode> node) {
    auto mulNode = std::make_unique<ExprNode>("*");
    mulNode->left = std::make_unique<ExprNode>(std::to_string(coefficient));
    mulNode->right = std::move(node);
    return mulNode;
}

std::unique_ptr<ExprNode> Solver::makeConstNode(double value) {
    return std::make_unique<ExprNode>(std::to_string(value));
}


void Solver::printTokens(const std::vector<Token>& tokens) const {
    for (const auto& token : tokens) {
        std::cout << "Token: " << token.value << " Type: ";
        switch (token.type) {
            case NUMBER:
                std::cout << "NUMBER";
                break;
            case VARIABLE:
                std::cout << "VARIABLE";
                break;
            case OPERATOR:
                std::cout << "OPERATOR";
                break;
            case FUNCTION:
                std::cout << "FUNCTION";
                break;
            case PAREN:
                std::cout << "PAREN";
                break;
            default:
                std::cout << "UNKNOWN";
        }
        std::cout << std::endl;
    }
}


void Solver::printTree(const ExprNode* node, int depth, const std::string& prefix, bool isLeft) const {
    if (node) {
        // Print the current node
        std::cout << prefix;

        // Add an appropriate connector (either a corner or a line)
        std::cout << (depth == 0 ? "" : (isLeft ? "├── " : "└── "));

        std::cout << node->value << std::endl;

        // Prepare the prefix for the children
        std::string childPrefix = prefix + (depth == 0 ? "" : (isLeft ? "│   " : "    "));

        // Print the left subtree first
        if (node->left) {
            printTree(node->left.get(), depth + 1, childPrefix, true);
        }

        // Print the right subtree
        if (node->right) {
            printTree(node->right.get(), depth + 1, childPrefix, false);
        }
    }
}
