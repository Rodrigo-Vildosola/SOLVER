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


    static std::unique_ptr<ExprNode> processFunction(const Token &token, std::stack<std::unique_ptr<ExprNode>> &nodeStack, const std::unordered_map<std::string, Function> &functions);
    static std::unique_ptr<ExprNode> processOperator(const Token &token, std::stack<std::unique_ptr<ExprNode>> &nodeStack);

    // Helpers
    static size_t getFunctionArgCount(const std::string &functionName, const std::unordered_map<std::string, Function> &functions);
    static double foldConstants(const std::string &op, double leftValue, double rightValue);

    static std::unique_ptr<ExprNode> applyBasicSimplifications(const std::string &op, const std::unique_ptr<ExprNode> &leftNode, const std::unique_ptr<ExprNode> &rightNode);
};
