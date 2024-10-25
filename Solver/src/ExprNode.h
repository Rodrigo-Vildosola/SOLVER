#pragma once

#include "pch.h"
#include "Token.h"  // Assuming TokenType is defined in Token.h
#include "MemoryPool.h"

class ExprNode;

// Static memory pool for ExprNode
static MemoryPool<ExprNode> exprNodePool(1024);  // You can adjust the initial pool size (1024)

class ExprNode {
public:
    TokenType type;                // Type of the node (NUMBER, VARIABLE, OPERATOR, FUNCTION, etc.)
    std::string value;             // The value of the node (e.g., "42", "x", "+", "sin")
    ExprNode* left;  // Left child (if applicable)
    ExprNode* right; // Right child (if applicable)
    std::vector<ExprNode*> arguments; // Arguments (for function nodes), now use raw pointers

    // Constructor
    ExprNode(TokenType nodeType, const std::string& val)
        : type(nodeType), value(val), left(nullptr), right(nullptr) {}

    // Method to check if the node is a number
    bool isNumber() const {
        return type == NUMBER;
    }

    // Override new and delete operators to use memory pool
    void* operator new(size_t) {
        return exprNodePool.allocate();  // Allocate from the memory pool
    }

    void operator delete(void* ptr) {
        exprNodePool.deallocate(static_cast<ExprNode*>(ptr));  // Deallocate back to the pool
    }

    // Explicit destroy method to handle cleanup and return the node to the pool
    void destroy() {
        if (left) {
            left->destroy();  // Recursively destroy left child
        }
        if (right) {
            right->destroy();  // Recursively destroy right child
        }
        for (auto* arg : arguments) {
            if (arg) arg->destroy();  // Destroy function arguments
        }
        exprNodePool.deallocate(this);  // Return this node to the pool
    }
};
