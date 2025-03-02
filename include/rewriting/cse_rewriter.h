#pragma once

#include "expression/node.h"
#include "helpers/node_factory.h"

namespace Expression {

class CSERewriter {
private:
    std::unordered_map<std::string, Node*> subexpressionCache;

    std::string hashKey(Node* node) {
        return node->toString();  // Generate a unique key based on the expression string
    }

public:
    Node* rewrite(Node* node, NodeFactory &factory) {
        if (!node) return nullptr;

        std::string key = hashKey(node);
        auto it = subexpressionCache.find(key);
        
        // If expression already exists, return the cached one.
        if (it != subexpressionCache.end()) {
            return it->second;
        }

        // Process different node types
        Node* rewrittenNode = nullptr;

        if (auto binOp = dynamic_cast<BinaryOpNode*>(node)) {
            Node* leftRewritten = rewrite(binOp->left, factory);
            Node* rightRewritten = rewrite(binOp->right, factory);

            if (auto add = dynamic_cast<AdditionNode*>(node)) {
                rewrittenNode = factory.add(leftRewritten, rightRewritten);
            } else if (auto mul = dynamic_cast<MultiplicationNode*>(node)) {
                rewrittenNode = factory.mul(leftRewritten, rightRewritten);
            } else if (auto sub = dynamic_cast<SubtractionNode*>(node)) {
                rewrittenNode = factory.sub(leftRewritten, rightRewritten);
            } else if (auto div = dynamic_cast<DivisionNode*>(node)) {
                rewrittenNode = factory.div(leftRewritten, rightRewritten);
            } else if (auto exp = dynamic_cast<ExponentiationNode*>(node)) {
                rewrittenNode = factory.exp(leftRewritten, rightRewritten);
            }
        } 
        else if (auto unaryOp = dynamic_cast<UnaryOpNode*>(node)) {
            Node* operandRewritten = rewrite(unaryOp->operand, factory);

            if (auto ln = dynamic_cast<LnNode*>(node)) {
                rewrittenNode = factory.ln(operandRewritten);
            } else if (auto sin = dynamic_cast<SinNode*>(node)) {
                rewrittenNode = factory.sin(operandRewritten);
            } else if (auto cos = dynamic_cast<CosNode*>(node)) {
                rewrittenNode = factory.cos(operandRewritten);
            } else if (auto tan = dynamic_cast<TanNode*>(node)) {
                rewrittenNode = factory.tan(operandRewritten);
            }
        } 
        else {
            rewrittenNode = node->clone(factory);  // Default clone for number, variable, etc.
        }

        // Cache and return
        subexpressionCache[key] = rewrittenNode;
        return rewrittenNode;
    }
};

} // namespace Expression
