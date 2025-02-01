#pragma once
#include "simplification_rule.h"

class MultOneRule : public SimplificationRule {
public:
    bool apply(const std::vector<Token>& input, std::vector<Token>& output) override;
};
