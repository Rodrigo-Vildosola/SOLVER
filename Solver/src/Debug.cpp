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

        // Print the node value, with color
        if (node->type == FUNCTION) {
            out << YELLOW << node->value << "(";
            for (size_t i = 0; i < node->arguments.size(); ++i) {
                out << node->arguments[i]->value;  // Print each argument value
                if (i < node->arguments.size() - 1) {
                    out << ", ";
                }
            }
            out << ")" << RESET;
        } else if (node->type == NUMBER) {
            out << GREEN << node->value << " [NUMBER]" << RESET;
        } else if (node->type == VARIABLE) {
            out << CYAN << node->value << " [VARIABLE]" << RESET;
        } else {
            out << RED << node->value << " [OPERATOR]" << RESET;
        }

        out << std::endl;

        // Prepare the prefix for the children
        std::string childPrefix = prefix + (depth == 0 ? "" : (isLeft ? "│   " : "    "));

        // If the node is a function and has an associated expression tree, print it
        if (node->type == FUNCTION) {
            auto functionIt = functions.find(node->value);
            if (functionIt != functions.end() && functionIt->second.exprTree) {
                out << childPrefix << (isLeft ? "│   " : "    ") << "└── Expression Tree for function '" << node->value << "':\n";
                printTree(functionIt->second.exprTree, out, functions, depth + 1, childPrefix, true);
            }
        }

        // Print the left subtree (if it's an operator or other node with children)
        if (node->left) {
            printTree(node->left, out, functions, depth + 1, childPrefix, true);
        }

        // Print the right subtree (if it's an operator or other node with children)
        if (node->right) {
            printTree(node->right, out, functions, depth + 1, childPrefix, false);
        }
    }
}


void printBoxedHeader(const std::string& title, size_t boxWidth) {
    if (boxWidth == 0) {
        boxWidth = title.length() + 4; // Set default box width based on title length
    }

    std::string border = "+" + std::string(boxWidth, '-') + "+";
    std::cout << BLUE << border << RESET << std::endl;

    // Adjust padding to center the title within the box
    int paddingLeft = (boxWidth - title.length()) / 2;
    int paddingRight = boxWidth - title.length() - paddingLeft;
    
    std::cout << BLUE << "|" << std::string(paddingLeft, ' ') << CYAN << title << std::string(paddingRight, ' ') << BLUE << "|" << RESET << std::endl;
    std::cout << BLUE << border << RESET << std::endl;
}

void printBoxedContent(const std::string& content, size_t boxWidth ) {
    if (boxWidth == 0) {
        boxWidth = content.length() + 4; // Set default box width based on content length
    }

    std::string border = "+" + std::string(boxWidth, '-') + "+";
    std::cout << border << std::endl;

    // Adjust padding for content within the box
    int paddingLeft = (boxWidth - content.length()) / 2;
    int paddingRight = boxWidth - content.length() - paddingLeft;

    // Fix the width formatting by adjusting the total width within the box
    std::cout << "|" << std::string(paddingLeft, ' ') << std::setw(content.length()) << content << std::string(paddingRight, ' ') << "|" << std::endl;
    std::cout << border << std::endl;
}
