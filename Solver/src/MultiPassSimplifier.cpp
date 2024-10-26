#include "MultiPassSimplifier.h"

inline ExprNode* foldConstants(ExprNode* node) {
    try {
        // Ensure both left and right nodes are NUMBER type before folding
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

        // In-place modification of node to become a constant
        node->value = std::to_string(result);
        node->type = NUMBER;
        node->left = node->right = nullptr;
        return node;
    }
    catch (const std::invalid_argument& e) {
        throw SolverException("Invalid argument in constant folding: " + std::string(e.what()));
    }
    catch (const std::out_of_range& e) {
        throw SolverException("Out-of-range error in constant folding: " + std::string(e.what()));
    }
    catch (const SolverException& e) {
        throw; // Re-throw if it's an expected SolverException (e.g., Division by zero or Unknown operator)
    }
    catch (const std::exception& e) {
        throw SolverException("Unexpected error in constant folding: " + std::string(e.what()));
    }
}


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
          [](ExprNode* node) { return foldConstants(node); } }
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

    // Pass 4: Factorization and Distribution, with immediate folding
    passes.push_back({
        { 
            [](const ExprNode& node) { return node.value == "*" && node.right->value == "+"; },
            [](ExprNode* node) {
                std::cout << "Here on 1 " << std::endl;
                ExprNode* left = new ExprNode(OPERATOR, "*", node->left, node->right->left);
                ExprNode* right = new ExprNode(OPERATOR, "*", node->left, node->right->right);
                node->value = "+";
                node->left = (left->left->type == NUMBER && left->right->type == NUMBER) ? foldConstants(left) : left;
                node->right = (right->left->type == NUMBER && right->right->type == NUMBER) ? foldConstants(right) : right;
                return node;
            }
        },

        {   
            [](const ExprNode& node) { return node.value == "*" && node.left->value == "+"; },
            [](ExprNode* node) {
                std::cout << "Here on 2 " << std::endl;
                ExprNode* left = new ExprNode(OPERATOR, "*", node->left->left, node->right);
                ExprNode* right = new ExprNode(OPERATOR, "*", node->left->right, node->right);
                node->value = "+";
                node->left = (left->left->type == NUMBER && left->right->type == NUMBER) ? foldConstants(left) : left;
                node->right = (right->left->type == NUMBER && right->right->type == NUMBER) ? foldConstants(right) : right;
                return node;
            }
        },

        { 
            [](const ExprNode& node) { return node.value == "*" && node.left->value == "^" && node.right->value == "^" && node.left->left->value == node.right->left->value; },
            [](ExprNode* node) {
                std::cout << "Here on 3 " << std::endl;
                ExprNode* newExp = new ExprNode(OPERATOR, "+", node->left->right, node->right->right);
                node->value = "^"; 
                node->left = node->left->left; 
                node->right = foldConstants(newExp);
                return node;
            }
        }
    });

    // Final Pass: Constant Folding after all transformations
    passes.push_back({
        { 
            [](const ExprNode& node) { return node.type == OPERATOR && node.left->type == NUMBER && node.right->type == NUMBER; },
            [](ExprNode* node) {                 
                std::cout << "Here on 4 " << std::endl;
                return foldConstants(node); 
            } 
        }
    });
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
    int passCount = 1;
    for (const auto& pass : passes) {
        // std::cout << "On pass: " << passCount << root->value << std::endl;
        Simplification::RewriteEngine engine;
        for (const auto& rule : pass) {
            engine.addRule(rule);
        }
        root = engine.applyRules(root);  // Apply in-place rules from RewriteEngine
        passCount++;
    }

    return root;
}
