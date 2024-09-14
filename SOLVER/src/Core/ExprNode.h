#pragma once

#include <string>
#include <vector>
#include <memory> 

class ExprNode {
public:
    std::string value;
    ExprNode* left;
    ExprNode* right;
    std::vector<ExprNode*> arguments;

    // Constructor
    ExprNode(const std::string& val) : value(val), left(nullptr), right(nullptr) {}
};
