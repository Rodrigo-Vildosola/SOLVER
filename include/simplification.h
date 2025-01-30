#pragma once

#include "pch.h"
#include "token.h"
#include "function.h"
#include "exception.h"
#include "symbol_table.h"


namespace Simplification {


    std::vector<Token> fullySimplifyPostfix(const std::vector<Token> &postfix, const std::unordered_map<std::string, Function> &functions);


    /**
     * @brief Performs a single pass of local folding/simplification on a fully flattened postfix expression.
     * 
     * @param postfix The input postfix tokens (flattened).
     * @param functions The map of function names to Function definitions (for predefined funcs).
     * @param changed Set to true if any folding/simplification occurred during this pass, false otherwise.
     * @return A (possibly) simplified postfix sequence after one pass.
     */
    std::vector<Token> singlePassSimplify(const std::vector<Token> &postfix, const std::unordered_map<std::string, Function> &functions, bool &changed);


    /**
     * @brief Attempts to simplify a binary operation (leftExpr op rightExpr) using local rules.
     * 
     * @param leftExpr  Postfix tokens for the left operand (often a single token).
     * @param rightExpr Postfix tokens for the right operand (often a single token).
     * @param opToken   The operator token (e.g. +, -, *, /, ^).
     * @return A new postfix token vector that represents the simplified expression.
     */
    std::vector<Token> trySimplifyBinary(const std::vector<Token> &leftExpr, const std::vector<Token> &rightExpr, const Token &opToken, bool &changed);


    std::vector<Token> trySimplifyFunction(const std::vector<std::vector<Token>> &argExprs, const Token &funcToken, const std::unordered_map<std::string, Function> &functions, bool &changed);


}
