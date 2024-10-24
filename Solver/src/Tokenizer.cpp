#include "Solver.h"
#include "Exception.h"
#include "Validator.h"
#include "Tokenizer.h"


std::vector<Token> Tokenizer::tokenize(const std::string& equation) {
    std::vector<Token> tokens;
    tokens.reserve(equation.size());

    auto begin = tokenizeUsingRegex(equation);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        std::string match = (*it)[1].str();
        processMatch(match, tokens, it, end);
    }

    return tokens;
}

std::sregex_iterator Tokenizer::tokenizeUsingRegex(const std::string& equation) {
    // Regex definitions
    static const std::regex tokenRegex(R"(\s*([+\-*/^]|\d+(\.\d+)?|[a-zA-Z_][a-zA-Z_0-9]*|[(),])\s*)");
    
    return std::sregex_iterator(equation.begin(), equation.end(), tokenRegex);
}

void Tokenizer::processMatch(const std::string& match, std::vector<Token>& tokens, std::sregex_iterator& it, const std::sregex_iterator& end) {
    static const std::regex numberRegex(R"(\d+(\.\d+)?)");
    static const std::regex variableRegex(R"([a-zA-Z_][a-zA-Z_0-9]*)");
    static const std::regex operatorRegex(R"([+\-*/^])");
    static const std::regex parenRegex(R"([\(\)])");
    static const std::regex separatorRegex(R"(,)");

    if (std::regex_match(match, numberRegex)) {
        handleNumberToken(match, tokens, it, end);
    }
    else if (std::regex_match(match, variableRegex)) {
        handleVariableOrFunctionToken(match, tokens, it, end);
    }
    else if (std::regex_match(match, operatorRegex)) {
        handleOperatorToken(match, tokens, it, end);
    }
    else if (std::regex_match(match, parenRegex)) {
        tokens.emplace_back(Token{PAREN, match});
    }
    else if (std::regex_match(match, separatorRegex)) {
        tokens.emplace_back(Token{SEPARATOR, match});
    }
    else {
        throw SolverException("Error: Unknown token '" + match + "'");
    }
}

void Tokenizer::handleNumberToken(const std::string& match, std::vector<Token>& tokens, std::sregex_iterator& it, const std::sregex_iterator& end) {
    // Handle negation case for numbers
    if (!tokens.empty() && tokens.back().type == FUNCTION && tokens.back().value == "neg") {
        auto next_it = std::next(it);
        if (next_it != end && (*next_it)[1].str() == "^") {
            tokens.emplace_back(Token{NUMBER, match}); 
        } else {
            tokens.back() = Token{NUMBER, "-" + match};  // Merge negation into the number token
        }
    } else {
        tokens.emplace_back(Token{NUMBER, match});
    }
}

void Tokenizer::handleVariableOrFunctionToken(const std::string& match, std::vector<Token>& tokens, std::sregex_iterator& it, const std::sregex_iterator& end) {
    auto next_it = std::next(it);
    if (next_it != end && (*next_it)[1].str() == "(") {
        tokens.emplace_back(Token{FUNCTION, match});
    } else {
        tokens.emplace_back(Token{VARIABLE, match});
    }
}

void Tokenizer::handleOperatorToken(const std::string& match, std::vector<Token>& tokens, std::sregex_iterator& it, const std::sregex_iterator& end) {
    if (match == "-" && (tokens.empty() || isUnaryContext(tokens.back()))) {
        // Handle unary minus (negation)
        tokens.emplace_back(Token{FUNCTION, "neg"});
        auto next_it = std::next(it);
        if (next_it != end && (*next_it)[1].str() == "^") {
            tokens.emplace_back(Token{PAREN, "("});  // Wrap the negation before exponentiation
        }
    } else {
        tokens.emplace_back(Token{OPERATOR, match});
    }
}

bool Tokenizer::isUnaryContext(const Token& lastToken) {
    return lastToken.type == OPERATOR || 
           lastToken.type == PAREN || 
           lastToken.type == SEPARATOR || 
           lastToken.value == "neg";
}
