#include "simplification/rules/function_folding_rule.h"
#include <sstream>
#include <iomanip>
#include <cmath>

FunctionFoldingRule::FunctionFoldingRule(const std::unordered_map<std::string, Function>& functions_map)
    : functions(functions_map) {}

bool FunctionFoldingRule::apply(const std::vector<Token>& input, std::vector<Token>& output) {
    // Expect input: arg1, arg2, ..., argN, FUNCTION
    if (input.size() < 2) {
        return false;
    }
    if (input.back().type != FUNCTION) {
        return false;
    }
    // Check that all preceding tokens are NUMBER
    for (size_t i = 0; i < input.size() - 1; ++i) {
        if (input[i].type != NUMBER) {
            return false;
        }
    }
    auto it = functions.find(input.back().value);
    if (it == functions.end()) {
        throw SolverException("Unknown function in folding: " + input.back().value);
    }
    const Function& func = it->second;
    size_t argCount = func.argCount;
    if (input.size() != argCount + 1) {
        return false;
    }
    std::vector<NUMBER_TYPE> numericArgs;
    numericArgs.reserve(argCount);
    for (size_t i = 0; i < argCount; ++i) {
        numericArgs.push_back(input[i].numericValue);
    }
    NUMBER_TYPE result = func.callback(numericArgs);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(15) << result;
    Token newToken(NUMBER, oss.str());
    newToken.numericValue = result;
    output = { newToken };
    return true;
}
