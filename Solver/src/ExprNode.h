#pragma once

#include "pch.h"
#include "Token.h"  // Assuming TokenType is defined in Token.h
#include "MemoryPool.h"


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

private:
    // Utility function to convert TokenType enum to string for display purposes
    std::string tokenTypeToString(TokenType type) const {
        switch (type) {
            case NUMBER: return "NUMBER";
            case VARIABLE: return "VARIABLE";
            case OPERATOR: return "OPERATOR";
            case FUNCTION: return "FUNCTION";
            case PAREN: return "PAREN";
            case SEPARATOR: return "SEPARATOR";
            case UNARY_OPERATOR: return "UNARY_OPERATOR";
            default: return "UNKNOWN";
        }
    }
};

// static MemoryPool<ExprNode> exprNodePool(1024);  // Pool with initial size of 1024
