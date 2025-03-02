#pragma once

#include "node.h"

namespace Expression {

class NodeFactory;

// EqualityNode represents an equation (e.g., A == B).
class EqualityNode : public Node {
public:
    EqualityNode(Node* left, Node* right);
    virtual ~EqualityNode();

    virtual double evaluate(const Env &env) const override;
    virtual std::string toString() const override;

    // Full Arena
    virtual Node* simplify(NodeFactory &factory) const override;
    virtual Node* derivative(const std::string& variable, NodeFactory &factory) const override;
    virtual Node* substitute(const std::string& variable, Node* value, NodeFactory &factory) const override;
    virtual Node* clone(NodeFactory &factory) const override;

    // Equation Solving
    virtual Node* solveFor(const std::string& variable, NodeFactory &factory) const;

    // virtual bool extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const override;

    Node* left;
    Node* right;
};

} // namespace Expression
