#pragma once

#include "rewriting/rewrite_rule.h"

namespace Expression {

class PythagoreanRule : public RewriteRule {
public:
    virtual bool matches(const Node* node) const override {
        if (auto add = dynamic_cast<const AdditionNode*>(node)) {
            // Look for expressions like (sin(x)^2) + (cos(x)^2)
            auto exp1 = dynamic_cast<const ExponentiationNode*>(add->left);
            auto exp2 = dynamic_cast<const ExponentiationNode*>(add->right);
            if (exp1 && exp2) {
                if (auto sinNode = dynamic_cast<const SinNode*>(exp1->left)) {
                    if (auto cosNode = dynamic_cast<const CosNode*>(exp2->left)) {
                        // Check if both operate on the same x.
                        return sinNode->operand->equals(cosNode->operand);
                    }
                }
            }
        }
        return false;
    }
    
    virtual Node* apply(const Node* node, NodeFactory &factory) const override {
        // Replace sin^2(x) + cos^2(x) with 1.
        return factory.num(1);
    }
};

} // namespace Expression
