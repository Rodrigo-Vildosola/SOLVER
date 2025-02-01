#pragma once

#include "pch.h"

/**
 * @enum TokenType
 * @brief Represents the type of a token in a mathematical expression.
 * 
 * The TokenType enum defines the types of tokens that can be present in a mathematical expression.
 */
enum TokenType { 
    NUMBER,          ///< A numeric constant (e.g., 42, 3.14)
    VARIABLE,        ///< A variable (e.g., x, y)
    OPERATOR,        ///< An operator (e.g., +, -, *, /, ^)
    FUNCTION,        ///< A function (e.g., sin, cos, f)
    PAREN,           ///< A parenthesis (either '(' or ')')
    SEPARATOR,       ///< A separator (typically a comma in function arguments)
    UNARY_OPERATOR   ///< A unary operator (e.g., negation)
};


enum class OperatorType {
    ADD,    // +
    SUB,    // -
    MUL,    // *
    DIV,    // /
    POW,    // ^
    UNKNOWN // Fallback
};

/**
 * @struct Token
 * @brief Represents a token in a mathematical expression.
 * 
 * When the token is a number, the numeric value is precomputed and stored in
 * the numericValue field. This saves us from having to call std::stold() during evaluation.
 */
struct Token {
    TokenType type;         ///< The type of the token
    std::string value;      ///< The textual representation (for non-operator tokens)
    NUMBER_TYPE numericValue;  ///< Precomputed numeric value (only valid if type == NUMBER)
    OperatorType op;        ///< Operator type (only valid if type == OPERATOR)

    // Constructor for all tokens:
    Token(TokenType t, const std::string &val)
        : type(t), value(val), numericValue(0), op(OperatorType::UNKNOWN)
    {
        if (t == NUMBER) {
            numericValue = std::stold(val);
        }
    }

    Token() : type(NUMBER), value(), numericValue(0), op(OperatorType::UNKNOWN) {}
};
