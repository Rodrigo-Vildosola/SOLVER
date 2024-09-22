#pragma once
#include <queue>
#include <stack>
#include <memory>
#include "Token.h"
#include "ExprNode.h"
#include "Exception.h"

class ExpressionTree {
public:
    std::unique_ptr<ExprNode> parseExpression(const std::vector<Token>& tokens);
    std::unique_ptr<ExprNode> simplify(std::unique_ptr<ExprNode> node);

private:
    std::queue<Token> shuntingYard(const std::vector<Token>& tokens);

    // Simplification helpers
    bool isZero(const std::unique_ptr<ExprNode>& node);
    bool isOne(const std::unique_ptr<ExprNode>& node);
    bool areEqual(const std::unique_ptr<ExprNode>& left, const std::unique_ptr<ExprNode>& right);
    std::unique_ptr<ExprNode> makeMulNode(double coefficient, std::unique_ptr<ExprNode> node);
    std::unique_ptr<ExprNode> makeConstNode(double value);

};
