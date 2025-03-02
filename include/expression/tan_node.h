#pragma once

#include "unary_op_node.h"

namespace Expression {

class NodeFactory;

// Represents tan(x).
class TanNode : public UnaryOpNode {
public:
    explicit TanNode(Node* operand);
    virtual ~TanNode();

    virtual double evaluate(const Env &env) const override;
    virtual std::string toString() const override;

    // Simplification method with constant folding and room for more advanced rules.
    virtual Node* simplify(NodeFactory &factory) const override;
    virtual Node* derivative(const std::string &variable, NodeFactory &factory) const override;
    virtual Node* substitute(const std::string &variable, Node* value, NodeFactory &factory) const override;
    virtual Node* clone(NodeFactory &factory) const override;

    virtual bool extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const override;
    virtual bool equals(const Node* other) const override;
};

} // namespace Expression
