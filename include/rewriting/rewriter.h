#pragma once

#include "rewriting/factorization_rule.h"
#include "rewriting/exp_product_rule.h"
#include "rewriting/exp_division_rule.h"
#include "rewriting/ln_difference_rule.h"
#include "rewriting/ln_addition_rule.h"
#include "rewriting/log_difference_rule.h"
#include "rewriting/log_addition_rule.h"


#include "rewriting/cse_rewriter.h"
#include "rewriting/rewrite_rule.h"
#include "helpers/node_factory.h"

namespace Expression {

class Rewriter {
private:
    std::vector<std::unique_ptr<RewriteRule>> rules;
    CSERewriter cse;

public:
    Rewriter() {
        rules.push_back(std::make_unique<ExpProductRule>());
        rules.push_back(std::make_unique<ExpDivisionRule>());
        rules.push_back(std::make_unique<LnDifferenceRule>());
        rules.push_back(std::make_unique<LnAdditionRule>());
        rules.push_back(std::make_unique<LogDifferenceRule>());
        rules.push_back(std::make_unique<LogAdditionRule>());
        rules.push_back(std::make_unique<FactorizationRule>());
        // Add more rules as needed.
    }

    // Recursively apply rewriting rules to a node until no more changes occur.
    Node* rewrite(Node* node, NodeFactory &factory) {
        Node* current = node->clone(factory);
        bool changed = true;

        while (changed) {
            changed = false;
            for (const auto &rule : rules) {
                if (rule->matches(current)) {
                    Node* newNode = rule->apply(current, factory);
                    if (!newNode->toString().empty() && newNode->toString() != current->toString()) {
                        current = newNode;
                        changed = true;
                        break;
                    }
                }
            }
        }

        return current->simplify(factory);
    }

    void add_rule(std::unique_ptr<RewriteRule> rule) {
        rules.push_back(std::move(rule));
    }
};

} // namespace Expression
