#pragma once
#include "simplification_rule.h"

class SubZeroRule : public SimplificationRule {
public:
    bool apply(const std::vector<Token>& input, std::vector<Token>& output) override;
};
