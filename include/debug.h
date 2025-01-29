#pragma once

#include "pch.h"
#include "token.h"
#include "function.h"
#include "exception.h"

// Color definitions
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"


inline std::string postfixToInfix(const std::vector<Token>& tokens, const std::unordered_map<std::string, Function>& functions) {
    std::stack<std::string> stack;

    for (const auto& token : tokens) {
        if (token.type == TokenType::NUMBER || token.type == TokenType::VARIABLE) {
            // Push numbers or variables directly onto the stack
            stack.push(token.value);
        } else if (token.type == TokenType::OPERATOR) {
            // Pop the top two elements for binary operators
            if (stack.size() < 2) {
                throw SolverException("Invalid postfix expression: insufficient operands for operator '" + token.value + "'");
            }
            std::string right = stack.top();
            stack.pop();
            std::string left = stack.top();
            stack.pop();

            // Combine into an infix expression and push back
            std::string infix = "(" + left + " " + token.value + " " + right + ")";
            stack.push(infix);
        } else if (token.type == TokenType::FUNCTION) {
            // Retrieve the function definition
            auto it = functions.find(token.value);
            if (it == functions.end()) {
                throw SolverException("Unknown function: '" + token.value + "'");
            }
            const Function& function = it->second;

            // Ensure sufficient arguments are available
            if (stack.size() < function.argCount) {
                throw SolverException("Invalid postfix expression: insufficient arguments for function '" + token.value + "'");
            }

            // Pop arguments from the stack in reverse order
            std::vector<std::string> args(function.argCount);
            for (size_t i = 0; i < function.argCount; ++i) {
                args[function.argCount - i - 1] = stack.top();
                stack.pop();
            }

            // Build the function call representation
            std::string functionCall = token.value + "(";
            for (size_t i = 0; i < args.size(); ++i) {
                functionCall += args[i];
                if (i < args.size() - 1) {
                    functionCall += ", ";
                }
            }
            functionCall += ")";
            stack.push(functionCall);
        }
    }

    // After processing all tokens, there should be exactly one element left
    if (stack.size() != 1) {
        throw SolverException("Invalid postfix expression: leftover operands after evaluation");
    }

    return stack.top();
}

inline void printInfix(const std::vector<Token>& tokens, const std::unordered_map<std::string, Function>& functions) {
    try {
        std::string infix = postfixToInfix(tokens, functions);
        std::cout << infix << std::endl;
    } catch (const SolverException& e) {
        std::cerr << "Error converting postfix to infix: " << e.what() << std::endl;
    }
}

inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::VARIABLE: return "VARIABLE";
        case TokenType::OPERATOR: return "OPERATOR";
        case TokenType::FUNCTION: return "FUNCTION";
        case TokenType::PAREN: return "PAREN";
        case TokenType::SEPARATOR: return "SEPARATOR";
        case TokenType::UNARY_OPERATOR: return "UNARY_OPERATOR";
        default: return "UNKNOWN";
    }
}

inline void printTokens(const std::vector<Token>& tokens) {
    std::cout << "[";
    for (size_t i = 0; i < tokens.size(); ++i) {
        const Token& token = tokens[i];
        std::cout << "{Type: " << tokenTypeToString(token.type) << ", Value: \"" << token.value << "\"}";
        if (i != tokens.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

inline void printPostfix(const std::vector<Token>& tokens) {
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << tokens[i].value;
        if (i != tokens.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}


/**
 * @brief Print the tokens with their types for debugging purposes.
 * 
 * @param tokens A vector of Token objects representing the expression.
 */
void printTokens(const std::vector<Token>& tokens);


/**
 * @brief Print a boxed header with colored output for debug mode.
 * 
 * @param title The title of the header to be displayed.
 */
void printBoxedHeader(const std::string& title, size_t boxWidth = 0);

/**
 * @brief Print a boxed content block for displaying debug information.
 * 
 * This method creates a dynamic box based on the length of the content.
 * 
 * @param content The content to be displayed inside the box.
 */
void printBoxedContent(const std::string& content, size_t boxWidth = 0);
