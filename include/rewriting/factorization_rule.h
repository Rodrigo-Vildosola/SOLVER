#pragma once

#include "rewriting/rewrite_rule.h"

namespace Expression {

class FactorizationRule : public RewriteRule {
public:
    virtual bool matches(const Node* node) const override {
        if (auto add = dynamic_cast<const AdditionNode*>(node)) {
            auto mul1 = dynamic_cast<const MultiplicationNode*>(add->left);
            auto mul2 = dynamic_cast<const MultiplicationNode*>(add->right);
            if (mul1 && mul2) {
                // For simplicity, factor if the left children are equal.
                return mul1->left->equals(mul2->left);
            }
        }
        return false;
    }
    virtual Node* apply(const Node* node, NodeFactory &factory) const override {
        auto add = static_cast<const AdditionNode*>(node);
        auto mul1 = static_cast<const MultiplicationNode*>(add->left);
        auto mul2 = static_cast<const MultiplicationNode*>(add->right);
        // Common factor is the left child.
        Node* common = mul1->left->clone(factory);
        // Sum the right children.
        Node* sum = factory.add(mul1->right->clone(factory), mul2->right->clone(factory));
        return factory.mul(common, sum);
    }
};

} // namespace Expression
