#pragma once

#include "binary_op_node.h"

namespace Expression {

class NodeFactory;

// Represents logarithm: log_base(x).
class LogNode : public BinaryOpNode {
public:
    LogNode(Node* base, Node* operand);
    virtual ~LogNode();

    virtual double evaluate(const Env &env) const override;
    virtual std::string toString() const override;

    // Full Arena
    virtual Node* simplify(NodeFactory &factory) const override;
    virtual Node* derivative(const std::string& variable, NodeFactory &factory) const override;
    virtual Node* substitute(const std::string& variable, Node* value, NodeFactory &factory) const override;
    virtual Node* clone(NodeFactory &factory) const override;

    virtual bool extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const override;
    virtual bool equals(const Node* other) const override;

};

} // namespace Expression
