#include "simplification/rules/associative_add_rule.h"

bool AssociativeAddRule::apply(const std::vector<Token>& input, std::vector<Token>& output) {
    if (input.empty() || input.back().value != "+") return false;
    
    std::vector<Token> terms;
    NUMBER_TYPE sum = 0;
    bool hasVariable = false;
    
    // Flatten associative addition
    for (size_t i = 0; i < input.size() - 1; ++i) {
        const Token& token = input[i];
        if (token.type == NUMBER) {
            sum += token.numericValue;
        } else {
            hasVariable = true;
            terms.push_back(token);
        }
    }
    
    // Build output expression
    output.clear();
    if (sum != 0) {
        Token sumToken;
        sumToken.type = NUMBER;
        sumToken.value = numberToString(sum);
        sumToken.numericValue = sum;
        output.push_back(sumToken);
    }
    output.insert(output.end(), terms.begin(), terms.end());
    if (hasVariable) {
        output.push_back(Token{OPERATOR, "+"});
    }
    return true;
}
