#pragma once

#include "pch.h"

enum TokenType { NUMBER, VARIABLE, OPERATOR, FUNCTION, PAREN, SEPARATOR, UNARY_OPERATOR };

struct Token {
    TokenType type;
    std::string value;
};
