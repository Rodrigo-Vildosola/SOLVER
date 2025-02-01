#include "simplification/simplification_engine.h"

void SimplificationEngine::add_rule(std::unique_ptr<SimplificationRule> rule) {
    rules.push_back(std::move(rule));
}

std::vector<Token> SimplificationEngine::simplify(const std::vector<Token>& input) {
    std::vector<Token> current = input;
    bool changed = true;
    const int MAX_ITERATIONS = 50; // safeguard against infinite loops

    for (int i = 0; i < MAX_ITERATIONS && changed; ++i) {
        changed = false;
        // Try each rule in order. If one applies, restart from the first rule.
        for (const auto& rule : rules) {
            std::vector<Token> output;
            if (rule->apply(current, output)) {
                current = std::move(output);
                changed = true;
                break;
            }
        }
    }
    return current;
}
