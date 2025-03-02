#pragma once

#include "rewriting/rewrite_rule.h"

namespace Expression {

class NegativeExpRule : public RewriteRule {
public:
    virtual bool matches(const Node* node) const override {
        if (auto expNode = dynamic_cast<const ExponentiationNode*>(node)) {
            if (auto exponentNum = dynamic_cast<const NumberNode*>(expNode->right)) {
                return exponentNum->getValue() < 0;
            }
        }
        return false;
    }
    
    virtual Node* apply(const Node* node, NodeFactory &factory) const override {
        auto expNode = static_cast<const ExponentiationNode*>(node);
        double negExp = dynamic_cast<const NumberNode*>(expNode->right)->getValue();
        // Compute new exponent: -negExp (which is positive)
        Node* newExponent = factory.num(-negExp);
        Node* positiveExp = factory.exp(expNode->left->clone(factory), newExponent);
        // a^-b -> 1 / a^b
        return factory.div(factory.num(1), positiveExp);
    }
};

} // namespace Expression
