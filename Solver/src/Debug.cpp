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


void printTree(const ExprNode* node, std::ostream& out, const std::unordered_map<std::string, Function>& functions, int depth, const std::string& prefix, bool isLeft) {
    if (node) {
        // Print the current node
        out << prefix;

        // Add an appropriate connector (either a corner or a line)
        out << (depth == 0 ? "" : (isLeft ? "├── " : "└── "));

        // Print the node value, with special handling for functions
        out << node->value;

        // If the node is a function, show its arguments inline
        if (node->type == FUNCTION) {
            out << "(";
            for (size_t i = 0; i < node->arguments.size(); ++i) {
                out << node->arguments[i]->value;  // Print each argument value
                if (i < node->arguments.size() - 1) {
                    out << ", ";
                }
            }
            out << ")";
        }

        // Print node type (optional for clarity)
        if (node->type == FUNCTION) {
            out << " [FUNCTION]";
        } else if (node->type == NUMBER) {
            out << " [NUMBER]";
        } else if (node->type == VARIABLE) {
            out << " [VARIABLE]";
        }

        out << std::endl;

        // Prepare the prefix for the children
        std::string childPrefix = prefix + (depth == 0 ? "" : (isLeft ? "│   " : "    "));

        // If the node is a function and has an associated expression tree, print it
        if (node->type == FUNCTION) {
            auto functionIt = functions.find(node->value);
            if (functionIt != functions.end() && functionIt->second.exprTree) {
                out << childPrefix << (isLeft ? "│   " : "    ") << "└── Expression Tree for function '" << node->value << "':\n";
                printTree(functionIt->second.exprTree.get(), out, functions, depth + 1, childPrefix, true);
            }
        }

        // Print the left subtree (if it's an operator or other node with children)
        if (node->left) {
            printTree(node->left.get(), out, functions, depth + 1, childPrefix, true);
        }

        // Print the right subtree (if it's an operator or other node with children)
        if (node->right) {
            printTree(node->right.get(), out, functions, depth + 1, childPrefix, false);
        }
    }
}
