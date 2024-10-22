#pragma once

#include "pch.h"
#include "Token.h"
#include "ExprNode.h"


void printTokens(const std::vector<Token>& tokens);


/**
 * @brief Print the expression tree for debugging purposes.
 * 
 * @param node The current node of the tree.
 * @param depth The depth of the node in the tree.
 * @param prefix The string prefix for the node (used for tree formatting).
 * @param isLeft Whether the current node is a left child.
 */
void printTree(const ExprNode* node, std::ostream& out, int depth = 0, const std::string& prefix = "", bool isLeft = true);
