#pragma once

#include "rewriting/rewrite_rule.h"


namespace Expression {

class LnDifferenceRule : public RewriteRule {
public:
    virtual bool matches(const Node* node) const override {
        if (auto sub = dynamic_cast<const SubtractionNode*>(node)) {
            return dynamic_cast<const LnNode*>(sub->left) &&
                   dynamic_cast<const LnNode*>(sub->right);
        }
        return false;
    }

    virtual Node* apply(const Node* node, NodeFactory &factory) const override {
        auto sub = static_cast<const SubtractionNode*>(node);
        auto ln_a = static_cast<const LnNode*>(sub->left);
        auto ln_b = static_cast<const LnNode*>(sub->right);
        // ln(a) - ln(b) -> ln(a / b)
        Node* quotient = factory.div(ln_a->operand->clone(factory),
                                     ln_b->operand->clone(factory));
        return factory.ln(quotient);
    }
};

} // namespace Expression
