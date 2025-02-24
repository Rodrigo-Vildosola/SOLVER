#pragma once

#include "pch.h"
#include "token.h"

inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case NUMBER: return "NUMBER";
        case VARIABLE: return "VARIABLE";
        case OPERATOR: return "OPERATOR";
        case FUNCTION: return "FUNCTION";
        case PAREN: return "PAREN";
        case SEPARATOR: return "SEPARATOR";
        case UNARY_OPERATOR: return "UNARY_OPERATOR";
        default: return "UNKNOWN";
    }
}

inline TokenType stringToTokenType(const std::string& s) {
    if (s == "NUMBER") return NUMBER;
    if (s == "VARIABLE") return VARIABLE;
    if (s == "OPERATOR") return OPERATOR;
    if (s == "FUNCTION") return FUNCTION;
    if (s == "PAREN") return PAREN;
    if (s == "SEPARATOR") return SEPARATOR;
    if (s == "UNARY_OPERATOR") return UNARY_OPERATOR;
    throw std::runtime_error("Invalid token type string: " + s);
}
