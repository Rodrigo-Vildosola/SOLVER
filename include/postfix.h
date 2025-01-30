#pragma once

#include "pch.h"
#include "token.h"
#include "function.h"
#include "exception.h"
#include "symbol_table.h"

namespace Postfix {

    /**
     * @brief Converts an infix expression represented by tokens to postfix notation using the Shunting Yard algorithm.
     * @param tokens A vector of tokens representing the expression in infix notation.
     * @return A queue of tokens in postfix notation.
     */
    std::vector<Token> shuntingYard(const std::vector<Token>& tokens);

    double evaluatePostfix(const std::vector<Token>& postfixQueue, const SymbolTable& symbolTable, const std::unordered_map<std::string, Function>& functions);

    std::vector<Token> flattenPostfix(const std::vector<Token>& postfixQueue, const std::unordered_map<std::string, Function>& functions);

    /**
     * @brief Simplifies a flattened postfix expression by constant folding and basic algebraic identities.
     * 
     * @param postfix A fully flattened postfix token sequence (no FUNCTION tokens).
     * @return A simplified postfix token sequence.
     * @throws SolverException If the postfix is malformed (e.g., insufficient operands).
     */
    std::vector<Token> simplifyPostfix(const std::vector<Token> &postfix, const std::unordered_map<std::string, Function> &functions);


    /**
     * @brief Attempts to simplify a binary operation (leftExpr op rightExpr) using local rules.
     * 
     * @param leftExpr  Postfix tokens for the left operand (often a single token).
     * @param rightExpr Postfix tokens for the right operand (often a single token).
     * @param opToken   The operator token (e.g. +, -, *, /, ^).
     * @return A new postfix token vector that represents the simplified expression.
     */
    std::vector<Token> trySimplifyBinary(const std::vector<Token> &leftExpr, const std::vector<Token> &rightExpr, const Token &opToken);


    std::vector<Token> trySimplifyFunction(const std::vector<std::vector<Token>> &argExprs, const Token &funcToken, const std::unordered_map<std::string, Function> &functions);


    /**
     * @brief Converts a single-token numeric expression into a double.
     *        Assumes isNumber(tokens) == true.
     */
    static double asNumber(const std::vector<Token> &tokens);

    /**
     * @brief Checks if a postfix sub-expression is exactly one numeric token (e.g. ["3.14"]).
     */
    static bool isNumber(const std::vector<Token> &tokens);

    /**
     * @brief Manages the operator stack according to precedence and associativity rules.
     * @param token The current operator token.
     * @param operatorStack Stack of operators.
     * @param outputQueue Queue for output tokens in postfix notation.
     */
    void processOperatorStack(const Token& token, std::stack<Token>& operatorStack, std::vector<Token>& outputVector);

    /**
     * @brief Retrieves the precedence level of a given operator.
     * @param op The operator as a string.
     * @return Integer representing the precedence level.
     */
    int getPrecedence(const std::string& op);

    /**
     * @brief Determines if an operator is left-associative.
     * @param op The operator as a string.
     * @return True if the operator is left-associative, false otherwise.
     */
    bool isLeftAssociative(const std::string& op);

    /**
     * @brief Handles closing parentheses in the Shunting Yard algorithm.
     *        Pops operators from the stack until an opening parenthesis is found.
     * @param operatorStack Stack of operators.
     * @param outputQueue Queue for output tokens in postfix notation.
     * @throws SolverException if parentheses are mismatched.
     */
    void handleParentheses(std::stack<Token>& operatorStack, std::vector<Token>& outputVector);

    /**
     * @brief Processes a function argument separator (comma) in the Shunting Yard algorithm.
     *        Ensures operators are correctly handled within function arguments.
     * @param operatorStack Stack of operators.
     * @param outputQueue Queue for output tokens in postfix notation.
     * @param argumentCounts Stack tracking the argument counts for nested functions.
     * @throws SolverException if parentheses or comma placement is invalid.
     */
    void handleFunctionArgumentSeparator(std::stack<Token>& operatorStack, std::vector<Token>& outputVector, std::stack<int>& argumentCounts);

    /**
     * @brief Retrieves the expected argument count for a given function.
     * @param functionName The name of the function.
     * @param functions Map of available functions with argument details.
     * @return Number of arguments the function expects.
     * @throws SolverException if the function is not found.
     */
    size_t getFunctionArgCount(const std::string& functionName, const std::unordered_map<std::string, Function>& functions);


}  // namespace ExpressionTree
