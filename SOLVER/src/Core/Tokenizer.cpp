// Solver.cpp
#include "Solver.h"
#include "Exception.h"
#include "Validator.h"
#include <cctype>
#include <iostream>
#include <iomanip>

std::vector<Token> Solver::tokenize(const std::string& equation) {
    std::vector<Token> tokens;
    std::string number;
    std::string name;
    bool isFunction = false;

    // Counter to track the number of closing parentheses to insert
    int closingParensToInsert = 0;

    for (size_t i = 0; i < equation.length(); ++i) {
        char c = equation[i];
        
        if (std::isspace(c)) {
            continue;
        }

        if (std::isdigit(c) || c == '.') {
            number += c;
        }
        else if (std::isalpha(c)) {
            // Start of a variable or function name
            name += c;
            // Peek to see if the next character is '(' indicating a function call
            size_t j = i + 1;
            while (j < equation.length() && std::isspace(equation[j])) {
                j++;
            }
            if (j < equation.length() && equation[j] == '(') {
                isFunction = true;
            }
            else {
                isFunction = false;
            }
        }
        else {
            // Emit accumulated number
            if (!number.empty()) {
                tokens.emplace_back(Token{NUMBER, number});
                number.clear();

                // If there's a pending closing parenthesis, insert it after the operand
                if (closingParensToInsert > 0) {
                    tokens.emplace_back(Token{PAREN, ")"});
                    closingParensToInsert--;
                }
            }

            // Emit accumulated name
            if (!name.empty()) {
                if (isFunction) {
                    tokens.emplace_back(Token{FUNCTION, name});
                    isFunction = false;
                }
                else {
                    tokens.emplace_back(Token{VARIABLE, name});
                }
                name.clear();

                // If there's a pending closing parenthesis, insert it after the operand
                if (closingParensToInsert > 0) {
                    tokens.emplace_back(Token{PAREN, ")"});
                    closingParensToInsert--;
                }
            }

            if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
                // Determine if '-' is unary or binary
                if (c == '-') {
                    if (tokens.empty() || 
                        tokens.back().type == OPERATOR || 
                        (tokens.back().type == PAREN && tokens.back().value == "(") ||
                        tokens.back().type == SEPARATOR) {
                        // It's a unary minus
                        tokens.emplace_back(Token{FUNCTION, "neg"});
                        tokens.emplace_back(Token{PAREN, "("});
                        closingParensToInsert++;
                        continue;
                    }
                }
                tokens.emplace_back(Token{OPERATOR, std::string(1, c)});
            }
            else if (c == '(' || c == ')') {
                tokens.emplace_back(Token{PAREN, std::string(1, c)});
            }
            else if (c == ',') {
                tokens.emplace_back(Token{SEPARATOR, ","});
            }
            else {
                throw SolverException("Error: Unknown character '" + std::string(1, c) + "'");
            }
        }
    }

    // Emit any remaining number
    if (!number.empty()) {
        tokens.emplace_back(Token{NUMBER, number});
        number.clear();

        // Insert closing parenthesis if needed
        if (closingParensToInsert > 0) {
            tokens.emplace_back(Token{PAREN, ")"});
            closingParensToInsert--;
        }
    }

    // Emit any remaining name
    if (!name.empty()) {
        if (isFunction) {
            tokens.emplace_back(Token{FUNCTION, name});
            isFunction = false;
        }
        else {
            tokens.emplace_back(Token{VARIABLE, name});
        }

        // Insert closing parenthesis if needed
        if (closingParensToInsert > 0) {
            tokens.emplace_back(Token{PAREN, ")"});
            closingParensToInsert--;
        }
    }

    // Insert any remaining closing parentheses
    while (closingParensToInsert > 0) {
        tokens.emplace_back(Token{PAREN, ")"});
        closingParensToInsert--;
    }

    return tokens;
}
