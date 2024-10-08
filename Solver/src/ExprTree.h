#pragma once

#include "pch.h"
#include "Token.h"
#include "Function.h"
#include "ExprNode.h"
#include "Exception.h"

class ExpressionTree {
public:
    // Static methods for parsing, simplifying, and managing expressions
    static std::unique_ptr<ExprNode> parseExpression(const std::vector<Token> &tokens, const std::unordered_map<std::string, Function> &functions);
    static std::unique_ptr<ExprNode> simplify(std::unique_ptr<ExprNode> node);

private:
    static std::queue<Token> shuntingYard(const std::vector<Token>& tokens);

    // Static helper methods for simplification
    static bool isZero(const std::unique_ptr<ExprNode>& node);
    static bool isOne(const std::unique_ptr<ExprNode>& node);
    static bool areEqual(const std::unique_ptr<ExprNode>& left, const std::unique_ptr<ExprNode>& right);
    static std::unique_ptr<ExprNode> makeMulNode(double coefficient, std::unique_ptr<ExprNode> node);
    static size_t getFunctionArgCount(const std::string &functionName, const std::unordered_map<std::string, Function> &functions);
    static std::unique_ptr<ExprNode> makeConstNode(double value);


};
