#pragma once

#include "rewriting/rewrite_rule.h"

namespace Expression {

class LogAdditionRule : public RewriteRule {
public:
    virtual bool matches(const Node* node) const override {
        if (auto add = dynamic_cast<const AdditionNode*>(node)) {
            return dynamic_cast<const LogNode*>(add->left) &&
                   dynamic_cast<const LogNode*>(add->right);
        }
        return false;
    }
    
    virtual Node* apply(const Node* node, NodeFactory &factory) const override {
        auto add = static_cast<const AdditionNode*>(node);
        auto log_a = static_cast<const LogNode*>(add->left);
        auto log_b = static_cast<const LogNode*>(add->right);
        // Check that both LogNodes have the same base.
        if (!log_a->left->equals(log_b->left)) {
            return nullptr; // Rule doesn't apply if bases differ.
        }
        // log_base(a) + log_base(b) -> log_base(a * b)
        Node* product = factory.mul(log_a->right->clone(factory), 
                                    log_b->right->clone(factory));
        return factory.log(log_a->left->clone(factory), product);
    }
};

} // namespace Expression
