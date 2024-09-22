#include "Solver.h"
#include "Exception.h"
#include <stack>
#include <stdexcept>

std::queue<Token> Solver::shuntingYard(const std::vector<Token>& tokens) {
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
