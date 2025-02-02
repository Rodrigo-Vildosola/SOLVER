#include "simplification/rules/constant_folding_rule.h"


bool ConstantFoldingRule::apply(const std::vector<Token>& input, std::vector<Token>& output) {
    // Check that the sub-expression is exactly of the form:
    // NUMBER, NUMBER, OPERATOR
    if (input.size() == 3 &&
        input[0].type == NUMBER &&
        input[1].type == NUMBER &&
        input[2].type == OPERATOR) {

        NUMBER_TYPE lhs = input[0].numericValue;
        NUMBER_TYPE rhs = input[1].numericValue;
        NUMBER_TYPE result = 0;
        std::string op = input[2].value;

        if (op == "+") {
            result = lhs + rhs;
        } else if (op == "-") {
            result = lhs - rhs;
        } else if (op == "*") {
            result = lhs * rhs;
        } else if (op == "/") {
            if (std::fabs(rhs) < 1e-14)
                throw SolverException("Division by zero in constant folding.");
            result = lhs / rhs;
        } else if (op == "^") {
            result = std::pow(lhs, rhs);
        } else {
            return false;
        }

        Token newToken;
        newToken.type  = NUMBER;
        newToken.value = numberToString(result);
        newToken.numericValue = result;
        output = { newToken };
        return true;
    }
    return false;
}
