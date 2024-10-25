#pragma once

#include "pch.h"
#include "Token.h"
#include "Function.h"
#include "ExprNode.h"
#include "Exception.h"
#include "SymbolTable.h"

class ExpressionTree {
public:
    // Static methods for parsing, simplifying, and managing expressions
    static ExprNode* parseExpression(const std::vector<Token> &tokens, const std::unordered_map<std::string, Function> &functions);
    static ExprNode* simplify(ExprNode* node, const SymbolTable &symbolTable);

private:
    static std::queue<Token> shuntingYard(const std::vector<Token>& tokens);


    // static std::unique_ptr<ExprNode> processFunction(const Token &token, std::stack<std::unique_ptr<ExprNode>> &nodeStack, const std::unordered_map<std::string, Function> &functions);
    static ExprNode* processFunction(const Token& token, std::stack<ExprNode*>& nodeStack, const std::unordered_map<std::string, Function>& functions);

    // static std::unique_ptr<ExprNode> processOperator(const Token &token, std::stack<std::unique_ptr<ExprNode>> &nodeStack);
    static ExprNode* processOperator(const Token& token, std::stack<ExprNode*>& nodeStack);


    // Helpers
    static size_t getFunctionArgCount(const std::string &functionName, const std::unordered_map<std::string, Function> &functions);
    static double foldConstants(const std::string &op, double leftValue, double rightValue);

    static ExprNode* applyBasicSimplifications(const std::string& op, ExprNode* leftNode, ExprNode* rightNode);
};
