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

/**
 * @struct Token
 * @brief Represents a token in a mathematical expression.
 * 
 * A Token consists of a type (from the TokenType enum) and a string value.
 * It is used to break down mathematical expressions into manageable components.
 */
struct Token {
    TokenType type;       ///< The type of the token (e.g., NUMBER, OPERATOR)
    std::string value;    ///< The string value of the token (e.g., "42", "+", "x")
};
