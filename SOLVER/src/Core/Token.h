#pragma once

#include <string>

enum TokenType { NUMBER, VARIABLE, OPERATOR, FUNCTION, PAREN, SEPARATOR };

struct Token {
    TokenType type;
    std::string value;
};
