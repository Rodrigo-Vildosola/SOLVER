#pragma once

#include "rewriting/rewrite_rule.h"

namespace Expression {

class LnPowerRule : public RewriteRule {
public:
    virtual bool matches(const Node* node) const override {
        // Match if the node is LnNode and its operand is an ExponentiationNode.
        if (auto ln = dynamic_cast<const LnNode*>(node)) {
            return dynamic_cast<const ExponentiationNode*>(ln->operand) != nullptr;
        }
        return false;
    }
    
    virtual Node* apply(const Node* node, NodeFactory &factory) const override {
        auto ln = static_cast<const LnNode*>(node);
        auto expNode = static_cast<const ExponentiationNode*>(ln->operand);
        // ln(a^b) -> b * ln(a)
        Node* lnA = factory.ln(expNode->left->clone(factory));
        return factory.mul(expNode->right->clone(factory), lnA);
    }
};

} // namespace Expression
