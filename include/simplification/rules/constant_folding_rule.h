#pragma once
#include "simplification_rule.h"

class ConstantFoldingRule : public SimplificationRule {
public:
    bool apply(const std::vector<Token>& input, std::vector<Token>& output) override;
};
