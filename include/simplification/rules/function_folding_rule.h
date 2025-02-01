#pragma once
#include "simplification_rule.h"
#include "function.h"
#include <unordered_map>

class FunctionFoldingRule : public SimplificationRule {
public:
    // Constructor takes the functions map.
    FunctionFoldingRule(const std::unordered_map<std::string, Function>& functions_map);
    bool apply(const std::vector<Token>& input, std::vector<Token>& output) override;
private:
    const std::unordered_map<std::string, Function>& functions;
};
