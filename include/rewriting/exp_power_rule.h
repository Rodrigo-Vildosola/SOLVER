#pragma once

#include "rewriting/rewrite_rule.h"

namespace Expression {

class ExpPowerRule : public RewriteRule {
public:
    virtual bool matches(const Node* node) const override {
        // Check if node is an exponentiation where the base is itself an exponentiation.
        if (auto expOuter = dynamic_cast<const ExponentiationNode*>(node)) {
            return dynamic_cast<const ExponentiationNode*>(expOuter->left) != nullptr;
        }
        return false;
    }
    
    virtual Node* apply(const Node* node, NodeFactory &factory) const override {
        auto expOuter = static_cast<const ExponentiationNode*>(node);
        auto innerExp = static_cast<const ExponentiationNode*>(expOuter->left);
        // Compute new exponent: inner_exponent * outer_exponent.
        Node* newExponent = factory.mul(innerExp->right->clone(factory), expOuter->right->clone(factory));
        return factory.exp(innerExp->left->clone(factory), newExponent);
    }
};

} // namespace Expression
