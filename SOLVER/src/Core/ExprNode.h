#pragma once

#include <string>
#include <vector>

class ExprNode {
public:
    std::string value;
    ExprNode* left;
    ExprNode* right;
    std::vector<ExprNode*> arguments;

    // Constructor
    ExprNode(const std::string& val) : value(val), left(nullptr), right(nullptr) {}

    // Destructor to free child nodes
    // ~ExprNode() {
    //     delete left;
    //     delete right;
    //     for (auto* arg : arguments) {
    //         delete arg;
    //     }
    // }
};
