#pragma once

#include "pch.h"
#include "Token.h"  // Assuming TokenType is defined in Token.h

class ExprNode {
public:
    TokenType type;                // Type of the node (NUMBER, VARIABLE, OPERATOR, FUNCTION, etc.)
    std::string value;             // The value of the node (e.g., "42", "x", "+", "sin")
    std::unique_ptr<ExprNode> left;  // Left child (if applicable)
    std::unique_ptr<ExprNode> right; // Right child (if applicable)
    std::vector<std::unique_ptr<ExprNode>> arguments; // Arguments (for function nodes)

    // Constructor
    ExprNode(TokenType nodeType, const std::string& val)
        : type(nodeType), value(val), left(nullptr), right(nullptr) {}

    // Method to check if the node is a number
    bool isNumber() const {
        return type == NUMBER;
    }

    // Stringify method to represent the node with its type
    std::string stringify() {
        std::string result = "Node(type: '" + tokenTypeToString(type) + "', value: '" + value + "'";

        // Indicate if the node has a left child
        if (left) {
            result += ", left: Yes";
        } else {
            result += ", left: No";
        }

        // Indicate if the node has a right child
        if (right) {
            result += ", right: Yes";
        } else {
            result += ", right: No";
        }

        // Indicate if the node has arguments (for function calls)
        if (!arguments.empty()) {
            result += ", arguments: [";
            for (size_t i = 0; i < arguments.size(); ++i) {
                result += arguments[i]->value;  // Just show the argument values
                if (i < arguments.size() - 1) {
                    result += ", ";
                }
            }
            result += "]";
        } else {
            result += ", arguments: None";
        }

        result += ")";
        return result;
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
