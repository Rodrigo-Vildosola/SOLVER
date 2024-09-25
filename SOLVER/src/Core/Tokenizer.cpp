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
            name += c;
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

                if (closingParensToInsert > 0) {
                    tokens.emplace_back(Token{PAREN, ")"});
                    closingParensToInsert--;
                }
            }

            // Emit accumulated name (function or variable)
            if (!name.empty()) {
                if (isFunction) {
                    tokens.emplace_back(Token{FUNCTION, name});
                    isFunction = false;
                }
                else {
                    tokens.emplace_back(Token{VARIABLE, name});
                }
                name.clear();

                if (closingParensToInsert > 0) {
                    tokens.emplace_back(Token{PAREN, ")"});
                    closingParensToInsert--;
                }
            }

            // Handle operators, parentheses, and separators
            if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
                if (c == '-') {
                    if (tokens.empty() || 
                        tokens.back().type == OPERATOR || 
                        (tokens.back().type == PAREN && tokens.back().value == "(") ||
                        tokens.back().type == SEPARATOR) {
                        // It's a unary minus
                        tokens.emplace_back(Token{FUNCTION, "neg"});
                        
                        // If next character is not '(', we wrap the following expression in parentheses
                        if (i + 1 < equation.size() && equation[i + 1] != '(') {
                            tokens.emplace_back(Token{PAREN, "("});
                            closingParensToInsert++;
                        }
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

    // Emit remaining number or variable
    if (!number.empty()) {
        tokens.emplace_back(Token{NUMBER, number});
        if (closingParensToInsert > 0) {
            tokens.emplace_back(Token{PAREN, ")"});
            closingParensToInsert--;
        }
    }

    if (!name.empty()) {
        tokens.emplace_back(isFunction ? Token{FUNCTION, name} : Token{VARIABLE, name});
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
