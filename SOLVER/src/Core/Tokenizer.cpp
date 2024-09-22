#include "Solver.h"
#include "Exception.h"
#include <cctype>
#include <iostream>

std::vector<Token> Solver::tokenize(const std::string& equation) {
    std::vector<Token> tokens;
    std::string number;

    for (size_t i = 0; i < equation.length(); ++i) {
        char c = equation[i];
        
        if (std::isspace(c)) {
            continue;
        }

        if (std::isdigit(c) || (c == '.' && !number.empty())) {
            number += c;
        } else {
            if (!number.empty()) {
                tokens.push_back({NUMBER, number});
                number.clear();
            }
            
            if (std::isalpha(c)) {
                std::string name;
                while (i < equation.length() && std::isalpha(equation[i])) {
                    name += equation[i++];
                }
                --i; // Adjust for extra increment
                if (predefinedFunctions.find(name) != predefinedFunctions.end() || functions.find(name) != functions.end()) {
                    tokens.push_back({FUNCTION, name});
                } else {
                    tokens.push_back({VARIABLE, name});
                }
            } else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
                tokens.push_back({OPERATOR, std::string(1, c)});
            } else if (c == '(' || c == ')') {
                tokens.push_back({PAREN, std::string(1, c)});
            } else if (c == ',') {
                tokens.push_back({SEPARATOR, ","});  // Ensure comma is properly tokenized
            } else {
                throw SolverException("Error: Unknown character '" + std::string(1, c) + "'");
            }
        }
    }

    if (!number.empty()) {
        tokens.push_back({NUMBER, number});
    }

    return tokens;
}

