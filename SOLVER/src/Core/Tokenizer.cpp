#include "Solver.h"
#include "Exception.h"
#include "Validator.h"
#include <regex>
#include <iostream>
#include <iomanip>

#include <regex>
#include <iterator>  // For std::next

std::vector<Token> Solver::tokenize(const std::string& equation) {
    std::vector<Token> tokens;

    // Define regular expressions for token types
    std::regex numberRegex(R"(\d+(\.\d+)?)");           // Matches integers and decimals
    std::regex variableRegex(R"([a-zA-Z_][a-zA-Z_0-9]*)"); // Matches variables and function names
    std::regex operatorRegex(R"([+\-*/^])");             // Matches basic operators
    std::regex parenRegex(R"([\(\)])");                 // Matches parentheses
    std::regex separatorRegex(R"(,)");                  // Matches function argument separator
    std::regex tokenRegex(R"(\s*([+\-*/^]|\d+(\.\d+)?|[a-zA-Z_][a-zA-Z_0-9]*|[(),])\s*)");

    // Use regex iterator to extract tokens
    auto begin = std::sregex_iterator(equation.begin(), equation.end(), tokenRegex);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        std::string match = (*it)[1].str();

        // Check if it's a number
        if (std::regex_match(match, numberRegex)) {
            if (!tokens.empty() && tokens.back().type == FUNCTION && tokens.back().value == "neg") {
                // Handle exponentiation case: do not merge negation with the number if followed by "^"
                auto next_it = std::next(it);
                if (next_it != end && (*next_it)[1].str() == "^") {
                    tokens.emplace_back(Token{NUMBER, match});  // Just add the number separately
                } else {
                    tokens.back() = Token{NUMBER, "-" + match};  // Unary minus is merged with the number
                }
            } else {
                tokens.emplace_back(Token{NUMBER, match});
            }
        }

        // Check if it's a variable or function name
        else if (std::regex_match(match, variableRegex)) {
            // Lookahead to see if it's a function (followed by '(')
            auto next_it = std::next(it); // Get the next iterator position
            if (next_it != end && (*next_it)[1].str() == "(") {
                tokens.emplace_back(Token{FUNCTION, match});
            } else {
                tokens.emplace_back(Token{VARIABLE, match});
            }
        }

        // Check if it's an operator
        else if (std::regex_match(match, operatorRegex)) {
            if (match == "-" && (tokens.empty() || 
                tokens.back().type == OPERATOR || 
                tokens.back().type == PAREN || 
                tokens.back().type == SEPARATOR || 
                tokens.back().value == "neg")) {
                // It's a unary minus (negation)
                tokens.emplace_back(Token{FUNCTION, "neg"});
                // Insert a parenthesis to wrap the negation when followed by exponentiation
                auto next_it = std::next(it);
                if (next_it != end && (*next_it)[1].str() == "^") {
                    tokens.emplace_back(Token{PAREN, "("});  // Wrap the negation before exponentiation
                }
            } else {
                tokens.emplace_back(Token{OPERATOR, match});
            }
        }

        // Check if it's a parenthesis
        else if (std::regex_match(match, parenRegex)) {
            tokens.emplace_back(Token{PAREN, match});
        }

        // Check if it's a separator (comma)
        else if (std::regex_match(match, separatorRegex)) {
            tokens.emplace_back(Token{SEPARATOR, match});
        }

        else {
            throw SolverException("Error: Unknown token '" + match + "'");
        }

    }

    return tokens;
}
