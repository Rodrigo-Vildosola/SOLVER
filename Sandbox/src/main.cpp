#include <iostream>
#include "SOLVER.h"

void printTokens(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        std::cout << "Token: " << token.value << " Type: " << token.type << std::endl;
    }
}

void printTree(const ExprNode* node, int depth = 0, const std::string& prefix = "", bool isLeft = true) {
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
            printTree(node->left, depth + 1, childPrefix, true);
        }

        // Print the right subtree
        if (node->right) {
            printTree(node->right, depth + 1, childPrefix, false);
        }
    }
}

void testEquation(Solver& solver, const std::string& equation) {
    std::cout << "Testing equation: " << equation << std::endl;

    auto tokens = solver.tokenize(equation);
    if (tokens.empty()) {
        std::cerr << "Error: Tokenization failed for equation: " << equation << std::endl;
        return;
    }

    auto exprTree = solver.parseExpression(tokens);
    if (!exprTree) {
        std::cerr << "Error: Parsing failed for equation: " << equation << std::endl;
        return;
    }

    printTree(exprTree);

    double evaluatedValue = solver.evaluate(equation);
    std::cout << "Evaluated expression value: " << evaluatedValue << std::endl;
    std::cout << "--------------------------" << std::endl << std::endl;
}

int main() {
    Solver solver;

    // Declare variables
    solver.declareVariable("x", 1);
    solver.declareVariable("y", 2);
    solver.declareVariable("z", 3);
    solver.declareVariable("w", 4);

    // Declare custom functions
    solver.declareFunction("sh", {"y"}, "y + 3");
    solver.declareFunction("f", {"x"}, "x * 2 + 1");

    // Test expressions
    std::vector<std::string> expressions = {
        "sh(x) * sh(y)",
        "f(z) + sh(w)",
        // "3 * (x + y) + 2 * z",
        // "sin(x) + cos(y) - log(z)",
        "(x^2 + 4 * y) / (3 - z)",
        // "x * (sin(y) + cos(z)) + exp(w)",
        // "(x + y + z + w) / (x * y - z + w)",
        // "2 + 2"
    };

    for (const auto& expression : expressions) {
        testEquation(solver, expression);
    }

    return 0;
}
