#pragma once

#include "pch.h"

class ExprNode {
public:
    std::string value;
    std::unique_ptr<ExprNode> left;
    std::unique_ptr<ExprNode> right;
    std::vector<std::unique_ptr<ExprNode>> arguments;

    // Constructor
    ExprNode(const std::string& val) : value(val), left(nullptr), right(nullptr) {}

    // Method to check if the node's value is a number
    bool isNumber() const {
        if (value.empty()) return false;
        for (char c : value) {
            if (!std::isdigit(c) && c != '.' && c != '-') return false;
        }
        return true;
    }
};
