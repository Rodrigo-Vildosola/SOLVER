#include "Debug.h"

void printTokens(const std::vector<Token>& tokens) {
    std::cout << "Tokens:\n";
    std::cout << std::left << std::setw(15) << "Type" << "Value\n";
    std::cout << "-------------------------\n";
    for (const auto& token : tokens) {
        std::string typeStr;
        switch (token.type) {
            case NUMBER: typeStr = "NUMBER"; break;
            case VARIABLE: typeStr = "VARIABLE"; break;
            case FUNCTION: typeStr = "FUNCTION"; break;
            case OPERATOR: typeStr = "OPERATOR"; break;
            case PAREN: typeStr = "PAREN"; break;
            case SEPARATOR: typeStr = "SEPARATOR"; break;
            default: typeStr = "UNKNOWN"; break;
        }
        std::cout << std::left << std::setw(15) << typeStr << token.value << "\n";
    }
    std::cout << "-------------------------\n";
}


void printTree(const ExprNode* node, int depth, const std::string& prefix, bool isLeft) {
    if (node) {
        // Print the current node
        std::cout << prefix;

        // Add an appropriate connector (either a corner or a line)
        std::cout << (depth == 0 ? "" : (isLeft ? "├── " : "└── "));

        std::cout << node->value << std::endl;

        // Prepare the prefix for the children
        std::string childPrefix = prefix + (depth == 0 ? "" : (isLeft ? "│   " : "    "));

        // Print the left subtree first
        if (node->left) {
            printTree(node->left.get(), depth + 1, childPrefix, true);
        }

        // Print the right subtree
        if (node->right) {
            printTree(node->right.get(), depth + 1, childPrefix, false);
        }
    }
}
