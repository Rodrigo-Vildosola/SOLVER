#pragma once

#include "pch.h"
#include "token.h"
#include "exception.h"

class SimplificationRule {
public:
    virtual ~SimplificationRule() = default;
    /// @brief Attempt to simplify a given token sequence.
    /// @param input The tokens representing a subexpression.
    /// @param output The simplified tokens if the rule applies.
    /// @return true if the rule was applied; false otherwise.
    virtual bool apply(const std::vector<Token>& input, std::vector<Token>& output) = 0;
};
