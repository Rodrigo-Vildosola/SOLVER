#pragma once

#include "rewriting/rewrite_rule.h"

namespace Expression {

class ExpProductRule : public RewriteRule {
public:
    virtual bool matches(const Node* node) const override {
        // Check if the node is a MultiplicationNode.
        if (auto mul = dynamic_cast<const MultiplicationNode*>(node)) {
            // Check if both children are ExponentiationNodes.
            auto exp1 = dynamic_cast<const ExponentiationNode*>(mul->left);
            auto exp2 = dynamic_cast<const ExponentiationNode*>(mul->right);
            if (exp1 && exp2) {
                // Check if the bases are equal.
                return exp1->left->equals(exp2->left);
            }
        }
        return false;
    }
    
    virtual Node* apply(const Node* node, NodeFactory &factory) const override {
        // We assume matches() is true.
        auto mul = static_cast<const MultiplicationNode*>(node);
        auto exp1 = static_cast<const ExponentiationNode*>(mul->left);
        auto exp2 = static_cast<const ExponentiationNode*>(mul->right);
        
        // Clone the common base.
        Node* commonBase = exp1->left->clone(factory);
        // Sum the exponents.
        Node* newExponent = factory.add(exp1->right->clone(factory), exp2->right->clone(factory));
        return factory.exp(commonBase, newExponent);
    }
};

} // namespace Expression
