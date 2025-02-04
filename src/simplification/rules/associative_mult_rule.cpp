#include "simplification/rules/associative_mult_rule.h"

bool AssociativeMultRule::apply(const std::vector<Token>& input, std::vector<Token>& output) {
    if (input.empty() || input.back().value != "*") return false;
    
    std::vector<Token> factors;
    NUMBER_TYPE product = 1;
    bool hasVariable = false;
    
    // Flatten associative multiplication
    for (size_t i = 0; i < input.size() - 1; ++i) {
        const Token& token = input[i];
        if (token.type == NUMBER) {
            product *= token.numericValue;
        } else {
            hasVariable = true;
            factors.push_back(token);
        }
    }
    
    // Build output expression
    output.clear();
    if (product != 1 || !hasVariable) {
        Token productToken;
        productToken.type = NUMBER;
        productToken.value = numberToString(product);
        productToken.numericValue = product;
        output.push_back(productToken);
    }
    output.insert(output.end(), factors.begin(), factors.end());
    if (hasVariable) {
        output.push_back(Token{OPERATOR, "*"});
    }
    return true;
}
