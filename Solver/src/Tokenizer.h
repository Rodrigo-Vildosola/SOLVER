#pragma once

#include "pch.h"
#include "Token.h"
#include "Exception.h"

/**
 * @class Tokenizer
 * @brief A static utility class for tokenizing mathematical expressions.
 * 
 * The Tokenizer class provides functions to convert a mathematical expression string
 * into a series of tokens (numbers, variables, operators, etc.) using regular expressions.
 */
class Tokenizer {
public:
    /**
     * @brief Tokenizes a mathematical expression into tokens.
     * 
     * @param equation The equation to tokenize.
     * @return A vector of tokens representing the equation.
     */
    static std::vector<Token> tokenize(const std::string& equation);

private:
    static std::sregex_iterator tokenizeUsingRegex(const std::string& equation);

    static void processMatch(const std::string& match, std::vector<Token>& tokens, std::sregex_iterator& it, const std::sregex_iterator& end);

    static void handleNumberToken(const std::string& match, std::vector<Token>& tokens, std::sregex_iterator& it, const std::sregex_iterator& end);

    static void handleVariableOrFunctionToken(const std::string& match, std::vector<Token>& tokens, std::sregex_iterator& it, const std::sregex_iterator& end);

    static void handleOperatorToken(const std::string& match, std::vector<Token>& tokens, std::sregex_iterator& it, const std::sregex_iterator& end);

    static bool isUnaryContext(const Token& lastToken);
};
