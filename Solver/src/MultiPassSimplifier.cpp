// MultiPassSimplifier.cpp
#include "MultiPassSimplifier.h"

void MultiPassSimplifier::initializePasses() {
    // Basic simplification rules (Pass 1)
    passes.push_back({
        // Rule: x + 0 -> x
        { [](const ExprNode& node) { return node.value == "+" && node.right->value == "0"; },
          [](ExprNode* node) { return new ExprNode(*node->left); } },

        // Rule: 0 + x -> x
        { [](const ExprNode& node) { return node.value == "+" && node.left->value == "0"; },
          [](ExprNode* node) { return new ExprNode(*node->right); } },

        // Rule: x * 1 -> x
        { [](const ExprNode& node) { return node.value == "*" && node.right->value == "1"; },
          [](ExprNode* node) { return new ExprNode(*node->left); } },

        // Rule: 1 * x -> x
        { [](const ExprNode& node) { return node.value == "*" && node.left->value == "1"; },
          [](ExprNode* node) { return new ExprNode(*node->right); } },

        // Rule: x * 0 -> 0
        { [](const ExprNode& node) { return node.value == "*" && (node.right->value == "0" || node.left->value == "0"); },
          [](ExprNode* node) { return new ExprNode(NUMBER, "0"); } }
    });

    // Constant folding and other arithmetic rules (Pass 2)
    passes.push_back({
        // Rule: x - 0 -> x
        { [](const ExprNode& node) { return node.value == "-" && node.right->value == "0"; },
          [](ExprNode* node) { return new ExprNode(*node->left); } },

        // Rule: x / 1 -> x
        { [](const ExprNode& node) { return node.value == "/" && node.right->value == "1"; },
          [](ExprNode* node) { return new ExprNode(*node->left); } },

        // Rule: x ^ 1 -> x
        { [](const ExprNode& node) { return node.value == "^" && node.right->value == "1"; },
          [](ExprNode* node) { return new ExprNode(*node->left); } },

        // Rule: x ^ 0 -> 1 (assuming x^0 is defined as 1)
        { [](const ExprNode& node) { return node.value == "^" && node.right->value == "0"; },
          [](ExprNode* node) { return new ExprNode(NUMBER, "1"); } }
    });

    // Additional passes can be added here for more complex transformations
}

ExprNode* MultiPassSimplifier::simplify(ExprNode* root) {
    initializePasses();

    for (const auto& pass : passes) {
        Simplification::RewriteEngine engine;
        for (const auto& rule : pass) {
            engine.addRule(rule);
        }
        root = engine.applyRules(root);
    }
    return root;
}
