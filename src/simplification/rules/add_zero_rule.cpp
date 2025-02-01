#include "simplification/rules/add_zero_rule.h"
#include <cmath>

bool AddZeroRule::apply(const std::vector<Token>& input, std::vector<Token>& output) {
    // Expect a binary addition expression: <expr1> <expr2> "+"
    if (input.size() == 3 && input[2].type == OPERATOR && input[2].value == "+") {
        // If first operand is 0: 0 + x => x
        if (input[0].type == NUMBER && std::fabs(input[0].numericValue) < 1e-14) {
            output = { input[1] };
            return true;
        }
        // If second operand is 0: x + 0 => x
        if (input[1].type == NUMBER && std::fabs(input[1].numericValue) < 1e-14) {
            output = { input[0] };
            return true;
        }
    }
    return false;
}
