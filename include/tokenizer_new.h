// TokenizerNew.h
#pragma once

#include "pch.h"
#include "solver.h"      // For NUMBER_TYPE, etc.
#include "exception.h"   // For SolverException

// (Assuming your Token, TokenType, OperatorType, etc. are already defined.)
// For instance, your TokenType enum contains NUMBER, VARIABLE, FUNCTION,
// OPERATOR, UNARY_OPERATOR, PAREN, SEPARATOR, and EOF.

struct TokenRule {
    // A regex that should match the token at the current position.
    std::regex pattern;
    // A function to create a token from the match.
    // It receives:
    //   - the regex match,
    //   - the full input string,
    //   - the current position in the input,
    //   - a pointer to the last token (or nullptr if none),
    // and returns a Token.
    std::function<Token(const std::smatch&, const std::string&, size_t, const Token*)> createToken;
};

class TokenizerNew {
public:
    /// Tokenizes the input string into a list of tokens.
    static std::vector<Token> tokenize(const std::string &input) {
        std::vector<Token> tokens;
        size_t pos = 0;
        while (pos < input.size()) {
            // Skip whitespace
            while (pos < input.size() && std::isspace(input[pos])) {
                ++pos;
            }
            if (pos >= input.size()) break;

            bool matched = false;
            for (const auto &rule : getTokenRules()) {
                std::smatch match;
                std::string substring = input.substr(pos);
                // Use match_continuous so that the rule must match at the current pos.
                if (std::regex_search(substring, match, rule.pattern, std::regex_constants::match_continuous)) {
                    const Token* lastToken = tokens.empty() ? nullptr : &tokens.back();
                    Token token = rule.createToken(match, input, pos, lastToken);
                    tokens.push_back(token);
                    pos += match.length();
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                throw SolverException("Unknown token starting at: " + input.substr(pos));
            }
        }
        // Append an EOF token to signal end of input.
        // tokens.push_back(Token(EOF, ""));
        return tokens;
    }

private:
    /// Returns a (static) list of token rules.
    static const std::vector<TokenRule>& getTokenRules() {
        static const std::vector<TokenRule> rules = []() -> std::vector<TokenRule> {
            std::vector<TokenRule> rules;

            // Number rule: e.g., "3", "4.5"
            rules.push_back(TokenRule{
                std::regex(R"(\d+(\.\d+)?)"),
                [](const std::smatch &m, const std::string & /*input*/, size_t /*pos*/, const Token* /*lastToken*/) -> Token {
                    return Token(NUMBER, m.str());
                }
            });

            // Identifier rule: could be a variable or a function name.
            // (We decide if it’s a function later by peeking ahead.)
            rules.push_back(TokenRule{
                std::regex(R"([a-zA-Z_]\w*)"),
                [](const std::smatch &m, const std::string &input, size_t pos, const Token* /*lastToken*/) -> Token {
                    // Look ahead: if the next non-space char is '(' then it’s a function.
                    size_t nextPos = pos + m.length();
                    while (nextPos < input.size() && std::isspace(input[nextPos])) { ++nextPos; }
                    if (nextPos < input.size() && input[nextPos] == '(') {
                        return Token(FUNCTION, m.str());
                    }
                    return Token(VARIABLE, m.str());
                }
            });

            // Operator rule: supports +, -, *, /, ^, and also potential new ones like '!'
            rules.push_back(TokenRule{
                std::regex(R"([+\-*/^!])"),
                [](const std::smatch &m, const std::string & /*input*/, size_t /*pos*/, const Token* lastToken) -> Token {
                    std::string opStr = m.str();
                    // Decide if '-' is unary.
                    if (opStr == "-") {
                        bool isUnary = (!lastToken) ||
                            (lastToken->type == OPERATOR) ||
                            (lastToken->type == UNARY_OPERATOR) ||
                            (lastToken->type == SEPARATOR) ||
                            (lastToken->type == PAREN && lastToken->value == "(");
                        if (isUnary) {
                            // Create a dedicated UNARY_OPERATOR token.
                            Token token(UNARY_OPERATOR, opStr);
                            token.op = OperatorType::SUB; // Or you can define a dedicated unary minus type.
                            return token;
                        }
                    }
                    // For all other operators (or a binary '-')
                    Token token(OPERATOR, opStr);
                    if (opStr == "+")      token.op = OperatorType::ADD;
                    else if (opStr == "-") token.op = OperatorType::SUB;
                    else if (opStr == "*") token.op = OperatorType::MUL;
                    else if (opStr == "/") token.op = OperatorType::DIV;
                    else if (opStr == "^") token.op = OperatorType::POW;
                    else                   token.op = OperatorType::UNKNOWN;
                    return token;
                }
            });

            // Parenthesis rule: '(' and ')'
            rules.push_back(TokenRule{
                std::regex(R"([\(\)])"),
                [](const std::smatch &m, const std::string & /*input*/, size_t /*pos*/, const Token* /*lastToken*/) -> Token {
                    return Token(PAREN, m.str());
                }
            });

            // Separator rule: comma (for function arguments, etc.)
            rules.push_back(TokenRule{
                std::regex(R"(,)"),
                [](const std::smatch &m, const std::string & /*input*/, size_t /*pos*/, const Token* /*lastToken*/) -> Token {
                    return Token(SEPARATOR, m.str());
                }
            });

            return rules;
        }();
        return rules;
    }
};
