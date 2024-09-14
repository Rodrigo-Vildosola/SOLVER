#pragma once

#include <string>
#include <vector>
#include <memory> 

class ExprNode {
public:
    std::string value;
    std::unique_ptr<ExprNode> left;
    std::unique_ptr<ExprNode> right;
    std::vector<std::unique_ptr<ExprNode>> arguments;

    // Constructor
    ExprNode(const std::string& val) : value(val), left(nullptr), right(nullptr) {}
};
