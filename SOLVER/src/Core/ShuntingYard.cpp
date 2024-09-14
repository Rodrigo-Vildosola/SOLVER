#include "Solver.h"
#include <stack>
#include <stdexcept>

std::queue<Token> Solver::shuntingYard(const std::vector<Token>& tokens) {
    std::queue<Token> outputQueue;
    std::stack<Token> operatorStack;

    auto precedence = [](const std::string& op) {
        if (op == "+" || op == "-") return 1;
        if (op == "*" || op == "/") return 2;
        if (op == "^") return 3;
        return 0;
    };

    auto isLeftAssociative = [](const std::string& op) {
        if (op == "^") return false; // Right-associative
        return true; // All other operators are left-associative
    };

    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        if (token.type == NUMBER || token.type == VARIABLE) {
            outputQueue.push(token);
        } else if (token.type == FUNCTION) {
            operatorStack.push(token);
        } else if (token.type == OPERATOR) {
            // Handle unary minus
            if (token.value == "-" && (i == 0 || tokens[i-1].type == OPERATOR || tokens[i-1].value == "(")) {
                // This is a unary minus, treat it as a function
                operatorStack.push(Token{FUNCTION, "neg"});
            } else {
                while (!operatorStack.empty() &&
                       ((isLeftAssociative(token.value) && precedence(token.value) <= precedence(operatorStack.top().value)) ||
                       (!isLeftAssociative(token.value) && precedence(token.value) < precedence(operatorStack.top().value)))) {
                    outputQueue.push(operatorStack.top());
                    operatorStack.pop();
                }
                operatorStack.push(token);
            }
        } else if (token.value == "(") {
            operatorStack.push(token);
        } else if (token.value == ")") {
            while (!operatorStack.empty() && operatorStack.top().value != "(") {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
            }
            operatorStack.pop(); // Pop the "("
            if (!operatorStack.empty() && operatorStack.top().type == FUNCTION) {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
            }
        }
    }

    while (!operatorStack.empty()) {
        outputQueue.push(operatorStack.top());
        operatorStack.pop();
    }

    return outputQueue;
}
