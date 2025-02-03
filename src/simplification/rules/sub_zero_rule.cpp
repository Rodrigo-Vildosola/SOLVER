#include "simplification/rules/sub_zero_rule.h"
#include <cmath>

bool SubZeroRule::apply(const std::vector<Token>& input, std::vector<Token>& output) {
    // For subtraction: expr1, expr2, "-" 
    if (input.size() == 3 && input[2].type == OPERATOR && input[2].value == "-") {
        // x - 0 => x
        if (input[1].type == NUMBER && std::fabs(input[1].numericValue) < 1e-14) {
            output = { input[0] };
            return true;
        }
    }
    return false;
}
