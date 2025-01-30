#pragma once

#include "pch.h"
#include "token.h"
#include "function.h"
#include "exception.h"
#include "symbol_table.h"
#include "ast.h"


namespace Simplification {

    /**
     * @brief Replaces tokens that refer to constants with NUMBER tokens in a postfix sequence.
     *
     * If a token is of type VARIABLE but the symbol table indicates it is actually a constant,
     * we convert that token’s type to NUMBER and set its value to the constant’s numeric string.
     *
     * @param postfix      The flattened postfix tokens (after flattenPostfix).
     * @param symbolTable  The symbol table (holds variables and constants).
     * @return A new vector of tokens where any constant references have been inlined as numbers.
     */
    std::vector<Token> replaceConstantSymbols(const std::vector<Token> &postfix, const SymbolTable &symbolTable);

    // /**
    //  * @brief Converts a single-token numeric expression into a double.
    //  *        Assumes isNumber(tokens) == true.
    //  */
    // static double asNumber(const std::vector<Token> &tokens);

    // /**
    //  * @brief Checks if a postfix sub-expression is exactly one numeric token (e.g. ["3.14"]).
    //  */
    // static bool isNumber(const std::vector<Token> &tokens);


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

    /**
     * @brief Recursively simplifies the AST in place, applying constant folding
     *        and basic algebraic identities.
     *
     * @param node       The current AST node to simplify (may be modified in place).
     * @param functions  A map of predefined functions (for function folding).
     * @return The (possibly replaced) pointer to the simplified AST node. If a node is replaced,
     *         the old pointer is deleted. 
     */
    ASTNode* simplifyAST(ASTNode* node, const std::unordered_map<std::string, Function> &functions);

}
