// Validator.cpp
#include "Validator.h"
#include <regex>
#include <unordered_set>

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

}
