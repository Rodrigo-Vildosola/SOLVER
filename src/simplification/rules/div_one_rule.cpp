#include "simplification/rules/div_one_rule.h"
#include <cmath>

bool DivOneRule::apply(const std::vector<Token>& input, std::vector<Token>& output) {
    // For division: expr1, expr2, "/" 
    if (input.size() == 3 && input[2].type == OPERATOR && input[2].value == "/") {
        // x / 1 => x
        if (input[1].type == NUMBER && std::fabs(input[1].numericValue - 1.0) < 1e-14) {
            output = { input[0] };
            return true;
        }
    }
    return false;
}
