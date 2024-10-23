#include "ExprTree.h"

#pragma region Helpers

size_t ExpressionTree::getFunctionArgCount(const std::string& functionName, const std::unordered_map<std::string, Function>& functions) {
    auto it = functions.find(functionName);
    if (it != functions.end()) {
        return it->second.argCount;
    }
    throw SolverException("Unknown function '" + functionName + "'");
}

double ExpressionTree::foldConstants(const std::string& op, double leftValue, double rightValue) {
    if (op == "+") {
        return leftValue + rightValue;
    } else if (op == "-") {
        return leftValue - rightValue;
    } else if (op == "*") {
        return leftValue * rightValue;
    } else if (op == "/") {
        if (rightValue == 0) {
            throw SolverException("Division by zero error.");
        }
        return leftValue / rightValue;
    } else if (op == "^") {
        return std::pow(leftValue, rightValue);
    }
    throw SolverException("Unknown operator: '" + op + "'");
}

std::unique_ptr<ExprNode> ExpressionTree::applyBasicSimplifications(const std::string& op, const std::unique_ptr<ExprNode>& leftNode, const std::unique_ptr<ExprNode>& rightNode) {
    if (op == "+") {
        if (rightNode->type == NUMBER && rightNode->value == "0") return std::make_unique<ExprNode>(leftNode->type, leftNode->value);
        if (leftNode->type == NUMBER && leftNode->value == "0") return std::make_unique<ExprNode>(rightNode->type, rightNode->value);
    } else if (op == "*") {
        if (rightNode->type == NUMBER && rightNode->value == "1") return std::make_unique<ExprNode>(leftNode->type, leftNode->value);
        if (leftNode->type == NUMBER && leftNode->value == "1") return std::make_unique<ExprNode>(rightNode->type, rightNode->value);
        if (rightNode->type == NUMBER && rightNode->value == "0") return std::make_unique<ExprNode>(NUMBER, "0");
        if (leftNode->type == NUMBER && leftNode->value == "0") return std::make_unique<ExprNode>(NUMBER, "0");
    }
    return nullptr;  // No simplification possible
}

std::unique_ptr<ExprNode> ExpressionTree::processFunction(const Token& token, std::stack<std::unique_ptr<ExprNode>>& nodeStack, const std::unordered_map<std::string, Function>& functions) {
    size_t argCount = getFunctionArgCount(token.value, functions);

    if (nodeStack.size() < argCount) {
        throw SolverException("Error: Not enough operands for function '" + token.value + "'.");
    }

    std::vector<std::unique_ptr<ExprNode>> arguments(argCount);
    for (size_t i = 0; i < argCount; ++i) {
        arguments[argCount - i - 1] = std::move(nodeStack.top());
        nodeStack.pop();
    }

    auto node = std::make_unique<ExprNode>(token.type, token.value);
    node->arguments = std::move(arguments);
    return node;
}

std::unique_ptr<ExprNode> ExpressionTree::processOperator(const Token& token, std::stack<std::unique_ptr<ExprNode>>& nodeStack) {
    if (nodeStack.size() < 2) {
        throw SolverException("Error: Not enough operands for operator '" + token.value + "'");
    }

    auto rightNode = std::move(nodeStack.top());
    nodeStack.pop();
    auto leftNode = std::move(nodeStack.top());
    nodeStack.pop();

    // Otherwise, create a new operator node
    auto node = std::make_unique<ExprNode>(token.type, token.value);
    node->left = std::move(leftNode);
    node->right = std::move(rightNode);
    return node;
}

#pragma endregion


#pragma region Tree Parsers

std::queue<Token> ExpressionTree::shuntingYard(const std::vector<Token>& tokens) {
    std::queue<Token> outputQueue;
    std::stack<Token> operatorStack;
    std::stack<int> argumentCounts;

    auto precedence = [](const std::string& op) {
        if (op == "+" || op == "-") return 1;
        if (op == "*" || op == "/") return 2;
        if (op == "^") return 3;
        return 0;
    };

    auto isLeftAssociative = [](const std::string& op) {
        if (op == "^") return false;
        return true;
    };

    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        if (token.type == NUMBER || token.type == VARIABLE) {
            outputQueue.push(token);
        } else if (token.type == FUNCTION) {
            operatorStack.push(token);
            argumentCounts.push(1);
        } else if (token.type == OPERATOR) {
            while (!operatorStack.empty() &&
                   ((isLeftAssociative(token.value) && precedence(token.value) <= precedence(operatorStack.top().value)) ||
                    (!isLeftAssociative(token.value) && precedence(token.value) < precedence(operatorStack.top().value)))) {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
            }
            operatorStack.push(token);
        } else if (token.value == "(") {
            operatorStack.push(token);
        } else if (token.value == ")") {
            // Pop until the left parenthesis is found
            while (!operatorStack.empty() && operatorStack.top().value != "(") {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
            }
            if (operatorStack.empty()) {
                throw SolverException("Mismatched parentheses.");
            }
            operatorStack.pop(); // Pop the left parenthesis

            // If a function is on top of the stack, add it to the output queue
            if (!operatorStack.empty() && operatorStack.top().type == FUNCTION) {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
            }
        } else if (token.value == ",") {
            // Comma should pop the operators until a left parenthesis is found
            while (!operatorStack.empty() && operatorStack.top().value != "(") {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
            }
            if (operatorStack.empty()) {
                throw SolverException("Mismatched parentheses or misplaced comma.");
            }
            // Increase the argument count for the current function
            if (!argumentCounts.empty()) {
                argumentCounts.top()++;
            }
        }
    }

    while (!operatorStack.empty()) {
        if (operatorStack.top().value == "(" || operatorStack.top().value == ")") {
            throw SolverException("Mismatched parentheses.");
        }
        outputQueue.push(operatorStack.top());
        operatorStack.pop();
    }

    return outputQueue;
}

std::unique_ptr<ExprNode> ExpressionTree::parseExpression(const std::vector<Token>& tokens, const std::unordered_map<std::string, Function>& functions) {
    auto postfixQueue = shuntingYard(tokens);
    std::stack<std::unique_ptr<ExprNode>> nodeStack;

    while (!postfixQueue.empty()) {
        Token token = postfixQueue.front();
        postfixQueue.pop();

        if (token.type == NUMBER || token.type == VARIABLE) {
            nodeStack.push(std::make_unique<ExprNode>(token.type, token.value));
        } else if (token.type == OPERATOR) {
            nodeStack.push(processOperator(token, nodeStack));
        } else if (token.type == FUNCTION) {
            nodeStack.push(processFunction(token, nodeStack, functions));
        }
    }

    if (nodeStack.size() != 1) {
        throw SolverException("Error: The expression could not be parsed into an expression tree.");
    }

    return std::move(nodeStack.top());
}

#pragma endregion


#pragma region Post Tree Parsers

std::unique_ptr<ExprNode> ExpressionTree::simplify(std::unique_ptr<ExprNode> node, const SymbolTable& symbolTable) {
    if (!node) return nullptr;

    // Handle leaf nodes (NUMBER or VARIABLE)
    if (node->type == NUMBER || node->type == VARIABLE) {
        // For variables, check if they are constants and simplify
        if (node->type == VARIABLE && symbolTable.isConstant(node->value)) {
            double constantValue = symbolTable.lookupSymbol(node->value);
            node->value = std::to_string(constantValue);
            node->type = NUMBER;
        }
        return node;  // Return as-is for numbers or simplified variables
    }

    // Simplify function arguments recursively
    if (node->type == FUNCTION) {
        for (auto& arg : node->arguments) {
            arg = simplify(std::move(arg), symbolTable);
        }
        return node;
    }

    // Simplify left and right nodes recursively (for operators)
    if (node->type == OPERATOR) {
        node->left = simplify(std::move(node->left), symbolTable);
        node->right = simplify(std::move(node->right), symbolTable);

        // Apply basic algebraic simplifications
        if (node->left->type == NUMBER && node->right->type == NUMBER) {
            double leftValue = std::stod(node->left->value);
            double rightValue = std::stod(node->right->value);
            double foldedValue = foldConstants(node->value, leftValue, rightValue);
            return std::make_unique<ExprNode>(NUMBER, std::to_string(foldedValue));
        }

        // Handle additional cases for simplifications
        auto simplifiedNode = applyBasicSimplifications(node->value, node->left, node->right);
        if (simplifiedNode) {
            return simplifiedNode;
        }

        // If no simplifications, return node as-is
        return node;
    }

    return node;  // Return node as-is if it's not simplifiable
}

#pragma endregion
