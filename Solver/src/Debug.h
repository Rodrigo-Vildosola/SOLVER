#pragma once

#include "pch.h"
#include "Token.h"
#include "ExprNode.h"
#include "Function.h"

// Color definitions
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"

/**
 * @brief Print the tokens with their types for debugging purposes.
 * 
 * @param tokens A vector of Token objects representing the expression.
 */
void printTokens(const std::vector<Token>& tokens);


/**
 * @brief Print the expression tree for debugging purposes, including function definitions.
 * 
 * This function recursively prints the tree structure and handles special cases for functions,
 * showing inline arguments and sub-trees for functions.
 * 
 * @param node The current node of the expression tree.
 * @param out The output stream to print to (usually std::cout).
 * @param functions The function map to handle user-defined functions.
 * @param depth The depth of the current node (used for indentation).
 * @param prefix The string prefix for the current node (used for formatting).
 * @param isLeft Whether the current node is a left child.
 */
void printTree(const ExprNode* node, std::ostream& out, const std::unordered_map<std::string, Function>& functions, int depth = 0, const std::string& prefix = "", bool isLeft = true);

/**
 * @brief Print a boxed header with colored output for debug mode.
 * 
 * @param title The title of the header to be displayed.
 */
void printBoxedHeader(const std::string& title, size_t boxWidth = 0);

/**
 * @brief Print a boxed content block for displaying debug information.
 * 
 * This method creates a dynamic box based on the length of the content.
 * 
 * @param content The content to be displayed inside the box.
 */
void printBoxedContent(const std::string& content, size_t boxWidth = 0);
