#include "Solver.h"

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
