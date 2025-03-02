#pragma once

#include "rewriting/rewrite_rule.h"

namespace Expression {

class LogDifferenceRule : public RewriteRule {
public:
    virtual bool matches(const Node* node) const override {
        if (auto sub = dynamic_cast<const SubtractionNode*>(node)) {
            return dynamic_cast<const LogNode*>(sub->left) &&
                   dynamic_cast<const LogNode*>(sub->right);
        }
        return false;
    }
    
    virtual Node* apply(const Node* node, NodeFactory &factory) const override {
        auto sub = static_cast<const SubtractionNode*>(node);
        auto log_a = static_cast<const LogNode*>(sub->left);
        auto log_b = static_cast<const LogNode*>(sub->right);
        if (!log_a->left->equals(log_b->left)) {
            return nullptr; // Inconsistent bases.
        }
        // log_base(a) - log_base(b) -> log_base(a / b)
        Node* quotient = factory.div(log_a->right->clone(factory),
                                     log_b->right->clone(factory));
        return factory.log(log_a->left->clone(factory), quotient);
    }
};

} // namespace Expression
