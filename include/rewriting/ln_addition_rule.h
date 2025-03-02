#pragma once

#include "rewriting/rewrite_rule.h"

namespace Expression {

class LnAdditionRule : public RewriteRule {
public:
    virtual bool matches(const Node* node) const override {
        if (auto add = dynamic_cast<const AdditionNode*>(node)) {
            // Check that both children are LnNodes.
            return dynamic_cast<const LnNode*>(add->left) &&
                   dynamic_cast<const LnNode*>(add->right);
        }
        return false;
    }

    virtual Node* apply(const Node* node, NodeFactory &factory) const override {
        auto add = static_cast<const AdditionNode*>(node);
        auto ln_a = static_cast<const LnNode*>(add->left);
        auto ln_b = static_cast<const LnNode*>(add->right);
        // ln(a) + ln(b) -> ln(a * b)
        // Assume LnNode has a method: Node* get_operand() const;
        Node* product = factory.mul(ln_a->operand->clone(factory), 
                                    ln_b->operand->clone(factory));
        return factory.ln(product);
    }
};

} // namespace Expression
