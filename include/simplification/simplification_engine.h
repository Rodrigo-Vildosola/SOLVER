#pragma once
#include "function.h"
#include "rules/simplification_rule.h"

class SimplificationEngine {
public:
    /// Add a new simplification rule.
    void add_rule(std::unique_ptr<SimplificationRule> rule);

    /**
     * @brief Simplify the full postfix token sequence.
     *
     * This function repeatedly applies all simplification rules on every
     * sub-expression (by “rebuilding” the expression via a stack) until no rule
     * makes any further change or a maximum number of passes is reached.
     *
     * @param input The original postfix token vector.
     * @param functions A table of functions (needed for function rules).
     * @return The fully simplified postfix token vector.
     */
    std::vector<Token> simplify(const std::vector<Token>& input, const std::unordered_map<std::string, Function>& functions);

private:
    /// A single pass that traverses the full expression and applies rules where possible.
    std::vector<Token> simplifyPass(const std::vector<Token>& input, const std::unordered_map<std::string, Function>& functions, bool &changed);

    std::vector<std::unique_ptr<SimplificationRule>> rules;
};
