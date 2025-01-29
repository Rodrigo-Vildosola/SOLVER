// Validator.cpp
#include "validator.h"
#include "exception.h"

namespace Validator {

    bool isValidName(const std::string& name) {
        // Define the regex pattern for valid names
        static const std::regex pattern("^[A-Za-z_][A-Za-z0-9_]*$");
        
        // Check if the name matches the pattern
        if (!std::regex_match(name, pattern)) {
            return false;
        }

        // Define a set of reserved keywords
        static const std::unordered_set<std::string> reserved_keywords = {
            "if", "else", "while", "for", "return", "class", "public",
            "private", "protected", "int", "double", "float", "char",
            "bool", "void", "static", "const", "struct", "enum", "switch",
            "case", "break", "continue", "default", "do", "goto", "long",
            "short", "signed", "unsigned", "typedef", "sizeof", "volatile",
            "extern", "register", "union", "namespace", "using", "template",
            "typename"
        };

        // Check if the name is a reserved keyword
        if (reserved_keywords.find(name) != reserved_keywords.end()) {
            return false;
        }

        return true;
    }

    // Syntactic validation: Check for basic syntax correctness and provide detailed error messages
    void isValidSyntax(const std::string& expression) {
        int balance = 0;
        bool lastWasOperator = true; // To handle cases like "-3"

        for (size_t i = 0; i < expression.length(); ++i) {
            char c = expression[i];

            if (std::isspace(c)) {
                continue;
            }

            if (c == '(') {
                balance++;
                lastWasOperator = true;
            }
            else if (c == ')') {
                balance--;
                if (balance < 0) {
                    throw SolverException("Syntax Error: Unmatched closing parenthesis ')' at position " + std::to_string(i));
                }
                lastWasOperator = false;
            }
            else if (c == ',') {
                if (lastWasOperator) {
                    throw SolverException("Syntax Error: Unexpected comma ',' at position " + std::to_string(i));
                }
                lastWasOperator = true; // Comma acts similarly to an operator in terms of preceding elements
            }
            else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
                if (lastWasOperator) {
                    throw SolverException("Syntax Error: Unexpected operator '" + std::string(1, c) + "' at position " + std::to_string(i));
                }
                lastWasOperator = true;
            }
            else if (std::isdigit(c) || std::isalpha(c) || c == '.') {
                lastWasOperator = false;
            }
            else {
                // Unknown character
                throw SolverException("Syntax Error: Unknown character '" + std::string(1, c) + "' at position " + std::to_string(i));
            }
        }

        if (balance != 0) {
            std::stringstream ss;
            ss << "Syntax Error: Mismatched parentheses. ";
            ss << (balance > 0 ? "Missing closing parenthesis ')'." 
                                : "Extra closing parenthesis ')' found.");
            throw SolverException(ss.str());
        }

        if (lastWasOperator) {
            throw SolverException("Syntax Error: Expression cannot end with an operator.");
        }
    }


}
