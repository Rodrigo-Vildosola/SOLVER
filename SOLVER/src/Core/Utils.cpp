#include "Solver.h"
#include <iostream>

void Solver::printTree(const ExprNode* node, int depth, const std::string& prefix, bool isLeft) const {
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

// Register standard math functions
void Solver::registerBuiltInFunctions() {
    registerPredefinedFunction("sin", [](const std::vector<double>& args) -> double {
        return std::sin(args[0]);
    }, 1);

    registerPredefinedFunction("cos", [](const std::vector<double>& args) -> double {
        return std::cos(args[0]);
    }, 1);

    registerPredefinedFunction("tan", [](const std::vector<double>& args) -> double {
        return std::tan(args[0]);
    }, 1);

    registerPredefinedFunction("exp", [](const std::vector<double>& args) -> double {
        return std::exp(args[0]);
    }, 1);

    // Natural logarithm function (ln)
    registerPredefinedFunction("ln", [](const std::vector<double>& args) -> double {
        return std::log(args[0]);
    }, 1);

    // Logarithm with specified base
    registerPredefinedFunction("log", [](const std::vector<double>& args) -> double {
        return std::log(args[0]) / std::log(args[1]); // log base args[1] of args[0]
    }, 2);

    registerPredefinedFunction("sqrt", [](const std::vector<double>& args) -> double {
        return std::sqrt(args[0]);
    }, 1);

    registerPredefinedFunction("abs", [](const std::vector<double>& args) -> double {
        return std::abs(args[0]);
    }, 1);

    registerPredefinedFunction("max", [](const std::vector<double>& args) -> double {
        return std::max(args[0], args[1]);
    }, 2);

    registerPredefinedFunction("min", [](const std::vector<double>& args) -> double {
        return std::min(args[0], args[1]);
    }, 2);

    // Add more predefined functions as needed
}

