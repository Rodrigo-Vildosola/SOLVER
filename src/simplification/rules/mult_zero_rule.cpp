#include "simplification/rules/mult_zero_rule.h"
#include <cmath>

bool MultZeroRule::apply(const std::vector<Token>& input, std::vector<Token>& output) {
    // For multiplication: expr1, expr2, "*" 
    if (input.size() == 3 && input[2].type == OPERATOR && input[2].value == "*") {
        // If either operand is zero, result is 0.
        if ((input[0].type == NUMBER && std::fabs(input[0].numericValue) < 1e-14) ||
            (input[1].type == NUMBER && std::fabs(input[1].numericValue) < 1e-14)) {
            Token zeroToken(NUMBER, "0");
            zeroToken.numericValue = 0.0;
            output = { zeroToken };
            return true;
        }
    }
    return false;
}
