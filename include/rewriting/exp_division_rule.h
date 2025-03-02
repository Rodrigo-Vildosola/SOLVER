#pragma once

#include "rewriting/rewrite_rule.h"


namespace Expression {

class ExpDivisionRule : public RewriteRule {
public:
    virtual bool matches(const Node* node) const override {
        if (auto div = dynamic_cast<const DivisionNode*>(node)) {
            auto exp1 = dynamic_cast<const ExponentiationNode*>(div->left);
            auto exp2 = dynamic_cast<const ExponentiationNode*>(div->right);
            if (exp1 && exp2) {
                // Check if both bases are equal.
                return exp1->left->equals(exp2->left);
            }
        }
        return false;
    }
    
    virtual Node* apply(const Node* node, NodeFactory &factory) const override {
        auto div = static_cast<const DivisionNode*>(node);
        auto exp1 = static_cast<const ExponentiationNode*>(div->left);
        auto exp2 = static_cast<const ExponentiationNode*>(div->right);
        
        Node* commonBase = exp1->left->clone(factory);
        // Compute new exponent: exponent1 - exponent2
        Node* newExponent = factory.sub(exp1->right->clone(factory), exp2->right->clone(factory));
        return factory.exp(commonBase, newExponent);
    }
};

} // namespace Expression
