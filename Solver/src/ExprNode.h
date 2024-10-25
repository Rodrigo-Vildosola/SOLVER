#pragma once

#include "pch.h"
#include "Token.h"  


class ExprNode {
public:
    TokenType type;                                     // Type of the node (NUMBER, VARIABLE, OPERATOR, FUNCTION, etc.)
    std::string value;                                  // The value of the node (e.g., "42", "x", "+", "sin")
    std::unique_ptr<ExprNode> left;                     // Left child (if applicable)
    std::unique_ptr<ExprNode> right;                    // Right child (if applicable)
    std::vector<std::unique_ptr<ExprNode>> arguments;   // Arguments (for function nodes)

    // Constructor
    ExprNode(TokenType nodeType, const std::string& val)
        : type(nodeType), value(val), left(nullptr), right(nullptr) {}

    // Method to check if the node is a number
    bool isNumber() const {
        return type == NUMBER;
    }

};

