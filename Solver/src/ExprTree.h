#pragma once

#include "pch.h"
#include "Token.h"
#include "Function.h"
#include "ExprNode.h"
#include "Exception.h"
#include "SymbolTable.h"

namespace ExpressionTree {
    
    /**
     * @brief Parses a list of tokens into an expression tree.
     * @param tokens A vector of tokens representing the expression.
     * @param functions A map of available functions with their details.
     * @return Pointer to the root of the parsed expression tree (ExprNode).
     */
    ExprNode* parseExpression(const std::vector<Token>& tokens, const std::unordered_map<std::string, Function>& functions);

    /**
     * @brief Simplifies an expression tree by applying algebraic rules.
     * @param node The root of the expression tree to simplify.
     * @param symbolTable Symbol table containing known constants and variables.
     * @return Simplified expression tree (ExprNode).
     */
    ExprNode* simplify(ExprNode* node, const SymbolTable& symbolTable);

    /**
     * @brief Converts an infix expression represented by tokens to postfix notation using the Shunting Yard algorithm.
     * @param tokens A vector of tokens representing the expression in infix notation.
     * @return A queue of tokens in postfix notation.
     */
    std::queue<Token> shuntingYard(const std::vector<Token>& tokens);

    /**
     * @brief Processes a function token and its arguments from the node stack, creating a function node in the expression tree.
     * @param token The function token to process.
     * @param nodeStack Stack of expression nodes to pop arguments from.
     * @param functions Map of available functions with argument details.
     * @return Pointer to the function expression node created.
     */
    ExprNode* processFunction(const Token& token, std::stack<ExprNode*>& nodeStack, const std::unordered_map<std::string, Function>& functions);

    /**
     * @brief Processes an operator token and its operands from the node stack, creating an operator node in the expression tree.
     * @param token The operator token to process.
     * @param nodeStack Stack of expression nodes to pop operands from.
     * @return Pointer to the operator expression node created.
     */
    ExprNode* processOperator(const Token& token, std::stack<ExprNode*>& nodeStack);

    /**
     * @brief Manages the operator stack according to precedence and associativity rules.
     * @param token The current operator token.
     * @param operatorStack Stack of operators.
     * @param outputQueue Queue for output tokens in postfix notation.
     */
    void processOperatorStack(const Token& token, std::stack<Token>& operatorStack, std::queue<Token>& outputQueue);

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
    void handleParentheses(std::stack<Token>& operatorStack, std::queue<Token>& outputQueue);

    /**
     * @brief Processes a function argument separator (comma) in the Shunting Yard algorithm.
     *        Ensures operators are correctly handled within function arguments.
     * @param operatorStack Stack of operators.
     * @param outputQueue Queue for output tokens in postfix notation.
     * @param argumentCounts Stack tracking the argument counts for nested functions.
     * @throws SolverException if parentheses or comma placement is invalid.
     */
    void handleFunctionArgumentSeparator(std::stack<Token>& operatorStack, std::queue<Token>& outputQueue, std::stack<int>& argumentCounts);

    /**
     * @brief Retrieves the expected argument count for a given function.
     * @param functionName The name of the function.
     * @param functions Map of available functions with argument details.
     * @return Number of arguments the function expects.
     * @throws SolverException if the function is not found.
     */
    size_t getFunctionArgCount(const std::string& functionName, const std::unordered_map<std::string, Function>& functions);

    /**
     * @brief Folds two constant values based on a given operator.
     * @param op The operator as a string.
     * @param leftValue The left operand.
     * @param rightValue The right operand.
     * @return Result of the operation.
     * @throws SolverException for unknown operators or division by zero.
     */
    double foldConstants(const std::string& op, double leftValue, double rightValue);

    /**
     * @brief Applies basic algebraic simplifications to an operator node.
     *        E.g., x + 0 becomes x, x * 1 becomes x, x * 0 becomes 0.
     * @param op The operator as a string.
     * @param leftNode The left operand node.
     * @param rightNode The right operand node.
     * @return Pointer to a simplified node or nullptr if no simplification is possible.
     */
    ExprNode* applyBasicSimplifications(const std::string& op, ExprNode* leftNode, ExprNode* rightNode);

}  // namespace ExpressionTree
