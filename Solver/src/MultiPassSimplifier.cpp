#include "MultiPassSimplifier.h"

void MultiPassSimplifier::initializePasses() {
    if (!passes.empty()) return;  // Avoid re-initialization

    // Pass 1: Basic Arithmetic Simplifications
    passes.push_back({
        // Rule: x + 0 -> x and 0 + x -> x
        { [](const ExprNode& node) { return node.value == "+" && (node.right->value == "0" || node.left->value == "0"); },
          [](ExprNode* node) { return node->right->value == "0" ? node->left : node->right; } },

        // Rule: x * 1 -> x and 1 * x -> x
        { [](const ExprNode& node) { return node.value == "*" && (node.right->value == "1" || node.left->value == "1"); },
          [](ExprNode* node) { return node->right->value == "1" ? node->left : node->right; } },

        // Rule: x * 0 -> 0
        { [](const ExprNode& node) { return node.value == "*" && (node.right->value == "0" || node.left->value == "0"); },
          [](ExprNode* node) { node->value = "0"; node->type = NUMBER; node->left = node->right = nullptr; return node; } },

        // Rule: x - 0 -> x
        { [](const ExprNode& node) { return node.value == "-" && node.right->value == "0"; },
          [](ExprNode* node) { return node->left; } },

        // Rule: x / 1 -> x
        { [](const ExprNode& node) { return node.value == "/" && node.right->value == "1"; },
          [](ExprNode* node) { return node->left; } }
    });

    // Pass 2: Constant Folding
    passes.push_back({
        // Rule: Constant folding for operators (+, -, *, /, ^)
        { [](const ExprNode& node) { return node.type == OPERATOR && node.left->type == NUMBER && node.right->type == NUMBER; },
          [](ExprNode* node) {
              double leftValue = std::stod(node->left->value);
              double rightValue = std::stod(node->right->value);
              double result;

              if (node->value == "+") result = leftValue + rightValue;
              else if (node->value == "-") result = leftValue - rightValue;
              else if (node->value == "*") result = leftValue * rightValue;
              else if (node->value == "/") {
                  if (rightValue == 0) throw SolverException("Division by zero");
                  result = leftValue / rightValue;
              } else if (node->value == "^") result = std::pow(leftValue, rightValue);
              else throw SolverException("Unknown operator in constant folding");

              // Modify node in-place to become a constant
              node->value = std::to_string(result);
              node->type = NUMBER;
              node->left = node->right = nullptr;
              return node;
          }
        }
    });

    // Pass 3: Power Rules
    passes.push_back({
        // Rule: x ^ 1 -> x
        { [](const ExprNode& node) { return node.value == "^" && node.right->value == "1"; },
          [](ExprNode* node) { return node->left; } },

        // Rule: x ^ 0 -> 1
        { [](const ExprNode& node) { return node.value == "^" && node.right->value == "0"; },
          [](ExprNode* node) { node->value = "1"; node->type = NUMBER; node->left = node->right = nullptr; return node; } }
    });

    // Additional passes can be added here by adding more sets of rules to `passes`.
}


ExprNode* MultiPassSimplifier::simplify(ExprNode* root) {
    // Constant folding or constant resolution pass
    if (root->type == VARIABLE && symbolTable.isConstant(root->value)) {
        root->value = std::to_string(symbolTable.lookupSymbol(root->value));
        root->type = NUMBER;
    }

    // Recursive simplification for children (if operator or function)
    if (root->left) root->left = simplify(root->left);
    if (root->right) root->right = simplify(root->right);
    for (auto& arg : root->arguments) {
        arg = simplify(arg);
    }

    // Apply each pass's rules on the node
    for (const auto& pass : passes) {
        Simplification::RewriteEngine engine;
        for (const auto& rule : pass) {
            engine.addRule(rule);
        }
        root = engine.applyRules(root);  // Apply in-place rules from RewriteEngine
    }

    return root;
}
