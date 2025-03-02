#pragma once

#include "node.h"

namespace Expression {

class NodeFactory;

/**
 * @brief FunctionNode with an arbitrary number of arguments.
 */
class FunctionNode : public Node {
public:
    FunctionNode(const std::string& name, int expectedArgCount, const std::vector<Node*>& arguments, FunctionCallback callback);
    virtual ~FunctionNode();

    virtual double evaluate(const Env &env) const override;
    virtual std::string toString() const override;

    // Full Arena
    virtual Node* simplify(NodeFactory &factory) const override;
    virtual Node* derivative(const std::string& variable, NodeFactory &factory) const override;
    virtual Node* substitute(const std::string& variable, Node* value, NodeFactory &factory) const override;
    virtual Node* clone(NodeFactory &factory) const override;

    virtual bool extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const override;
    virtual bool equals(const Node* other) const override;

private:
    std::string name;
    int expectedArgCount;
    std::vector<Node*> arguments;
    FunctionCallback callback;
};

} // namespace Expression
