#include "pch.h"
#include "postfix.h"
#include "debug.h"

namespace Postfix {

#pragma region Helpers

size_t getFunctionArgCount(const std::string& functionName, const std::unordered_map<std::string, Function>& functions) {
    auto it = functions.find(functionName);
    if (it != functions.end()) {
        return it->second.argCount;
    }
    throw SolverException("Unknown function '" + functionName + "'");
}

#pragma endregion

#pragma region Parsing and Shunting Yard

int getPrecedence(const std::string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    if (op == "^") return 3;
    return 0;
}

bool isLeftAssociative(const std::string& op) {
    return op != "^";  // "^" is right-associative, all others are left-associative
}

void processOperatorStack(const Token& token, std::stack<Token>& operatorStack, std::vector<Token>& outputVector) {
    while (!operatorStack.empty() &&
           ((isLeftAssociative(token.value) && getPrecedence(token.value) <= getPrecedence(operatorStack.top().value)) ||
            (!isLeftAssociative(token.value) && getPrecedence(token.value) < getPrecedence(operatorStack.top().value)))) {
        outputVector.push_back(operatorStack.top());
        operatorStack.pop();
    }
    operatorStack.push(token);
}

void handleParentheses(std::stack<Token>& operatorStack, std::vector<Token>& outputVector) {
    while (!operatorStack.empty() && operatorStack.top().value != "(") {
        outputVector.push_back(operatorStack.top());
        operatorStack.pop();
    }
    if (operatorStack.empty()) {
        throw SolverException("Mismatched parentheses.");
    }
    operatorStack.pop();  // Pop the left parenthesis
}

void handleFunctionArgumentSeparator(std::stack<Token>& operatorStack, std::vector<Token>& outputVector, std::stack<int>& argumentCounts) {
    while (!operatorStack.empty() && operatorStack.top().value != "(") {
        outputVector.push_back(operatorStack.top());
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

std::vector<Token> shuntingYard(const std::vector<Token>& tokens) {
    PROFILE_FUNCTION();
    std::vector<Token> outputVector;       // Use a vector instead of queue
    std::stack<Token> operatorStack;
    std::stack<int> argumentCounts;

    for (const auto& token : tokens) {
        if (token.type == NUMBER || token.type == VARIABLE) {
            outputVector.push_back(token); // Push directly into the output vector
        } else if (token.type == FUNCTION) {
            operatorStack.push(token);
            argumentCounts.push(1);
        } else if (token.type == OPERATOR) {
            processOperatorStack(token, operatorStack, outputVector);
        } else if (token.value == "(") {
            operatorStack.push(token);
        } else if (token.value == ")") {
            handleParentheses(operatorStack, outputVector);
            if (!operatorStack.empty() && operatorStack.top().type == FUNCTION) {
                outputVector.push_back(operatorStack.top());
                operatorStack.pop();
            }
        } else if (token.value == ",") {
            handleFunctionArgumentSeparator(operatorStack, outputVector, argumentCounts);
        }
    }

    // Pop all remaining operators into the output vector
    while (!operatorStack.empty()) {
        if (operatorStack.top().value == "(" || operatorStack.top().value == ")") {
            throw SolverException("Mismatched parentheses.");
        }
        outputVector.push_back(operatorStack.top());
        operatorStack.pop();
    }

    return outputVector; // Return the postfix expression as a vector
}

#pragma endregion

#pragma region Postfix Evaluation

double evaluatePostfix(const std::vector<Token>& postfixQueue, const SymbolTable& symbolTable, const std::unordered_map<std::string, Function>& functions) {
    PROFILE_FUNCTION();
    std::stack<double> stack;

    for (const auto& token : postfixQueue) {
        if (token.type == NUMBER) {
            stack.push(std::stod(token.value));
        } else if (token.type == VARIABLE) {
            stack.push(symbolTable.lookupSymbol(token.value));
        } else if (token.type == OPERATOR) {
            if (stack.size() < 2) {
                throw SolverException("Not enough operands for operator '" + token.value + "'");
            }

            double right = stack.top(); stack.pop();
            double left = stack.top(); stack.pop();

            if (token.value == "+") stack.push(left + right);
            else if (token.value == "-") stack.push(left - right);
            else if (token.value == "*") stack.push(left * right);
            else if (token.value == "/") {
                if (right == 0) throw SolverException("Division by zero error.");
                stack.push(left / right);
            } else if (token.value == "^") stack.push(std::pow(left, right));
            else throw SolverException("Unknown operator: '" + token.value + "'");
        } else if (token.type == FUNCTION) {
            auto it = functions.find(token.value);
            if (it == functions.end()) {
                throw SolverException("Unknown function: '" + token.value + "'");
            }

            const Function& function = it->second;
            size_t argCount = function.argCount;

            if (stack.size() < argCount) {
                throw SolverException("Not enough arguments for function '" + token.value + "'");
            }

            std::vector<double> args(argCount);
            for (size_t i = 0; i < argCount; ++i) {
                args[argCount - i - 1] = stack.top();
                stack.pop();
            }

            if (function.isPredefined) {
                stack.push(function.callback(args));
            } else {
                throw SolverException("User defined functions should be flattened before eval stage");
            }
        } else {
            throw SolverException("Unsupported token type during evaluation.");
        }
    }

    if (stack.size() != 1) {
        throw SolverException("The postfix expression could not be evaluated.");
    }

    return stack.top();
}


std::vector<Token> flattenPostfix(const std::vector<Token>& postfixQueue, const std::unordered_map<std::string, Function>& functions) {
    PROFILE_FUNCTION();

    std::vector<Token> flattenedPostfix;
    std::stack<std::vector<Token>> argumentStack; // Stack to hold arguments for nested functions

    for (size_t i = 0; i < postfixQueue.size(); ++i) {
        const Token& token = postfixQueue[i];

        if (token.type == FUNCTION) {
            // Retrieve the function definition
            auto it = functions.find(token.value);
            if (it == functions.end()) {
                throw SolverException("Unknown function: '" + token.value + "'");
            }

            const Function& function = it->second;

            // Collect arguments
            if (argumentStack.size() < function.argCount) {
                throw SolverException("Insufficient arguments for function: '" + token.value + "'");
            }

            std::vector<std::vector<Token>> args(function.argCount);
            for (size_t j = 0; j < function.argCount; ++j) {
                args[function.argCount - j - 1] = argumentStack.top();
                argumentStack.pop();
            }

            if (function.isPredefined) {
                // For predefined functions, append arguments and the function token directly
                for (const auto& arg : args) {
                    flattenedPostfix.insert(flattenedPostfix.end(), arg.begin(), arg.end());
                }
                flattenedPostfix.push_back(token);
                continue; // Skip further processing for predefined functions
            }

            // Inline user-defined function
            std::vector<Token> inlinedPostfix;
            for (const auto& fToken : function.inlinedPostfix) {
                if (fToken.type == VARIABLE) {
                    auto argIt = std::find(function.argumentNames.begin(), function.argumentNames.end(), fToken.value);
                    if (argIt != function.argumentNames.end()) {
                        size_t index = std::distance(function.argumentNames.begin(), argIt);
                        inlinedPostfix.insert(inlinedPostfix.end(), args[index].begin(), args[index].end());
                    } else {
                        inlinedPostfix.push_back(fToken);
                    }
                } else {
                    inlinedPostfix.push_back(fToken);
                }
            }

            // Push the inlined result for user-defined functions back onto the stack
            argumentStack.push(inlinedPostfix);

        } else if (token.type == NUMBER || token.type == VARIABLE) {
            // Handle numbers and variables
            argumentStack.push({token});
        } else if (token.type == OPERATOR) {
            if (argumentStack.size() < 2) {
                throw SolverException("Not enough operands for operator '" + token.value + "'");
            }

            // Pop two operands
            auto right = argumentStack.top();
            argumentStack.pop();
            auto left = argumentStack.top();
            argumentStack.pop();

            // Combine into a single postfix sequence
            std::vector<Token> combinedPostfix = left;
            combinedPostfix.insert(combinedPostfix.end(), right.begin(), right.end());
            combinedPostfix.push_back(token);

            // Push the result back onto the stack
            argumentStack.push(combinedPostfix);
        } else {
            throw SolverException("Unsupported token type during flattening.");
        }
    }

    // Collect the final flattened expression
    while (!argumentStack.empty()) {
        auto result = argumentStack.top();
        argumentStack.pop();
        flattenedPostfix.insert(flattenedPostfix.end(), result.begin(), result.end());
    }

    return flattenedPostfix;
}


#pragma endregion

}  // namespace ExpressionTree
