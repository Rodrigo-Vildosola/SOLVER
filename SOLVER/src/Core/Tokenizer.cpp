#include "Solver.h"
#include "Exception.h"
#include <cctype>
#include <iostream>

std::vector<Token> Solver::tokenize(const std::string& equation) {
    std::vector<Token> tokens;
    std::string number;
    std::string name;
    bool isFunction = false;

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
            if (!number.empty()) {
                tokens.emplace_back(Token{NUMBER, number});
                number.clear();
            }
            if (!name.empty()) {
                if (isFunction) {
                    tokens.emplace_back(Token{FUNCTION, name});
                    isFunction = false;
                }
                else {
                    tokens.emplace_back(Token{VARIABLE, name});
                }
                name.clear();
            }

            if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
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

    if (!number.empty()) {
        tokens.emplace_back(Token{NUMBER, number});
    }
    if (!name.empty()) {
        if (isFunction) {
            tokens.emplace_back(Token{FUNCTION, name});
        }
        else {
            tokens.emplace_back(Token{VARIABLE, name});
        }
    }

    return tokens;
}

