#pragma once

#include "binary_op_node.h"
#include "number_node.h"

namespace Expression {

class NodeFactory;

// Represents division: left / right.
class DivisionNode : public BinaryOpNode {
public:
    DivisionNode(Node* left, Node* right);
    virtual ~DivisionNode();

    virtual double evaluate(const Env &env) const override;
    virtual std::string toString() const override;

    // Full Arena approach: pass NodeFactory to these methods
    virtual Node* simplify(NodeFactory &factory) const override;
    virtual Node* derivative(const std::string& variable, NodeFactory &factory) const override;
    virtual Node* substitute(const std::string& variable, Node* value, NodeFactory &factory) const override;
    virtual Node* clone(NodeFactory &factory) const override;

    virtual bool extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const override;
    virtual bool equals(const Node* other) const override;

};

} // namespace Expression
