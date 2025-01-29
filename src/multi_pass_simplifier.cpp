#include "multi_pass_simplifier.h"

inline ExprNode* foldConstants(ExprNode* node) {
    try {
        if (node->left->type != NUMBER || node->right->type != NUMBER) {
            throw SolverException("Non-numeric operand in constant folding");
        }

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

        node->value = std::to_string(result);
        node->type = NUMBER;
        node->left = node->right = nullptr;
        return node;
    } catch (const std::exception& e) {
        throw SolverException("Error in constant folding: " + std::string(e.what()));
    }
}

void MultiPassSimplifier::initializePasses() {
    if (!passes.empty()) return;  // Avoid re-initialization

    // Pass 1: Basic Arithmetic Simplifications
    // Simplifies trivial operations for more concise expressions
    passes.push_back({
        // Rule: x + 0 -> x and 0 + x -> x
        { [](const ExprNode& node) { return node.value == "+" && (node.right->value == "0" || node.left->value == "0"); },
          [](ExprNode* node) { return node->right->value == "0" ? node->left : node->right; } },

        // Rule: x * 1 -> x and 1 * x -> x
        { [](const ExprNode& node) { return node.value == "*" && (node.right->value == "1" || node.left->value == "1"); },
          [](ExprNode* node) { return node->right->value == "1" ? node->left : node->right; } },

        // Rule: x * 0 -> 0 (Multiplication by zero results in zero)
        { [](const ExprNode& node) { return node.value == "*" && (node.right->value == "0" || node.left->value == "0"); },
          [](ExprNode* node) { node->value = "0"; node->type = NUMBER; node->left = node->right = nullptr; return node; } },

        // Rule: x - 0 -> x (Subtraction of zero has no effect)
        { [](const ExprNode& node) { return node.value == "-" && node.right->value == "0"; },
          [](ExprNode* node) { return node->left; } },

        // Rule: x / 1 -> x (Division by one has no effect)
        { [](const ExprNode& node) { return node.value == "/" && node.right->value == "1"; },
          [](ExprNode* node) { return node->left; } }
    });

    // Pass 2: Constant Folding
    // Collapses expressions with numeric constants into single constant nodes
    passes.push_back({
        // Rule: Fold constants for operators (+, -, *, /, ^)
        { [](const ExprNode& node) { return node.type == OPERATOR && node.left->type == NUMBER && node.right->type == NUMBER; },
          [](ExprNode* node) { return foldConstants(node); } }
    });

    // Pass 3: Power Rules
    // Simplifies expressions involving powers
    passes.push_back({
        // Rule: x ^ 1 -> x (Raising to the power of 1 has no effect)
        { [](const ExprNode& node) { return node.value == "^" && node.right->value == "1"; },
          [](ExprNode* node) { return node->left; 
        } },

        // Rule: x ^ 0 -> 1 (Any number raised to 0 is 1, except when x is 0)
        { [](const ExprNode& node) { return node.value == "^" && node.right->value == "0"; },
          [](ExprNode* node) { node->value = "1"; node->type = NUMBER; node->left = node->right = nullptr; return node; 
        } },

        // Rule: x^a * x^b -> x^(a + b)
        { [](const ExprNode& node) { return node.value == "*" && node.left->value == "^" && node.right->value == "^" && node.left->left->value == node.right->left->value; },
          [](ExprNode* node) {
              ExprNode* newExp = new ExprNode(OPERATOR, "+", node->left->right, node->right->right);
              node->value = "^"; 
              node->left = node->left->left; 
              node->right = foldConstants(newExp);
              return node;
        } },

        // Rule: (x^a)^b -> x^(a * b)
        { [](const ExprNode& node) { return node.value == "^" && node.left->value == "^"; },
          [](ExprNode* node) {
              ExprNode* newExp = new ExprNode(OPERATOR, "*", node->left->right, node->right);
              node->left = node->left->left;
              node->right = foldConstants(newExp);
              return node;
        } }


    });

    // Pass 4: Factorization and Distribution
    // Distributes multiplication over addition and simplifies nested multiplications
    passes.push_back({
        // Rule: x * (y + z) -> (x * y) + (x * z) and (y + z) * x -> (y * x) + (z * x)
        { [](const ExprNode& node) { return node.value == "*" && (node.right->value == "+" || node.left->value == "+"); },
          [](ExprNode* node) {
              if (node->right->value == "+") {  // Case: x * (y + z)
                  ExprNode* left = new ExprNode(OPERATOR, "*", node->left, node->right->left);
                  ExprNode* right = new ExprNode(OPERATOR, "*", node->left, node->right->right);
                  node->value = "+";
                  node->left = (left->left->type == NUMBER && left->right->type == NUMBER) ? foldConstants(left) : left;
                  node->right = (right->left->type == NUMBER && right->right->type == NUMBER) ? foldConstants(right) : right;
              } else {  // Case: (y + z) * x
                  ExprNode* left = new ExprNode(OPERATOR, "*", node->left->left, node->right);
                  ExprNode* right = new ExprNode(OPERATOR, "*", node->left->right, node->right);
                  node->value = "+";
                  node->left = (left->left->type == NUMBER && left->right->type == NUMBER) ? foldConstants(left) : left;
                  node->right = (right->left->type == NUMBER && right->right->type == NUMBER) ? foldConstants(right) : right;
              }
              return node;
          }
        },

    });

    // Pass 5: Division Simplification
    passes.push_back({
        // Rule: (a * b) / b -> a
        { [](const ExprNode& node) { return node.value == "/" && node.left->value == "*" && node.left->right->value == node.right->value; },
          [](ExprNode* node) { return node->left->left; } },

        // Rule: b / (a * b) -> 1 / a
        { [](const ExprNode& node) { return node.value == "/" && node.right->value == "*" && node.right->right->value == node.left->value; },
          [](ExprNode* node) {
              node->value = "/"; 
              node->left->value = "1"; 
              node->left->type = NUMBER;
              node->right = node->right->left; 
              return node;
          } }
    });

    // Pass 6: Trigonometric Identities
    passes.push_back({
        // Rule: sin^2(x) + cos^2(x) -> 1 (Works for cos^2(x) + sin^2(x) as well)
        { [](const ExprNode& node) { return node.value == "+" && 
            ((node.left->value == "^" && node.left->left->value == "sin" && node.right->left->value == "cos") ||
            (node.left->value == "^" && node.left->left->value == "cos" && node.right->left->value == "sin")) &&
            node.left->left->arguments[0]->value == node.right->left->arguments[0]->value; },
          [](ExprNode* node) { node->value = "1"; node->type = NUMBER; node->left = node->right = nullptr; return node; } }
    });

    // Pass 7: Logarithmic Simplifications
    passes.push_back({
        // Rule: log(a) + log(b) -> log(a * b)
        { [](const ExprNode& node) { return node.value == "+" && node.left->value == "log" && node.right->value == "log"; },
          [](ExprNode* node) {
              ExprNode* product = new ExprNode(OPERATOR, "*", node->left->arguments[0], node->right->arguments[0]);
              node->value = "log";
              node->arguments = { product };
              node->left = node->right = nullptr;
              return node;
          }
        }
    });

    // Final Pass: Constant Folding after all transformations
    passes.push_back({
        { [](const ExprNode& node) { return node.type == OPERATOR && node.left->type == NUMBER && node.right->type == NUMBER; },
          [](ExprNode* node) { return foldConstants(node); } }
    });
}

ExprNode* MultiPassSimplifier::simplify(ExprNode* root) {
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
        root = engine.applyRules(root);
    }

    return root;
}
