#pragma once
#include "rules/simplification_rule.h"
#include <memory>
#include <vector>

class SimplificationEngine {
public:
    void add_rule(std::unique_ptr<SimplificationRule> rule);
    /// @brief Simplify the given token sequence by applying rules repeatedly.
    /// @param input The original token sequence.
    /// @return The simplified token sequence.
    std::vector<Token> simplify(const std::vector<Token>& input);
private:
    std::vector<std::unique_ptr<SimplificationRule>> rules;
};
