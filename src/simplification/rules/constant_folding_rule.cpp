#include "simplification/rules/constant_folding_rule.h"


bool ConstantFoldingRule::apply(const std::vector<Token>& input, std::vector<Token>& output) {
    // Expect a binary expression: NUMBER, NUMBER, OPERATOR
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
        // Convert the result to a string
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(15) << result;
        Token newToken(NUMBER, oss.str());
        newToken.numericValue = result;
        output = { newToken };
        return true;
    }
    return false;
}
