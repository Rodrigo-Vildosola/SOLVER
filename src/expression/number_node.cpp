#include "expression/number_node.h"
#include "helpers/node_factory.h"  // NodeFactory


namespace Expression {

NumberNode::NumberNode(double value) : value(value) {}

NumberNode::~NumberNode() {}

double NumberNode::evaluate(const Env &env) const {
    return value;
}

std::string NumberNode::toString() const {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// **Simplify**
Node* NumberNode::simplify(NodeFactory &factory) const {
    // A number is already simplified, so we can just return `this` or clone it.
    // For consistency, let’s just return `this`.
    // If you prefer returning a new node, do: return factory.num(value);
    return const_cast<NumberNode*>(this); 
}

// **Derivative**
Node* NumberNode::derivative(const std::string& variable, NodeFactory &factory) const {
    // Derivative of a constant is 0
    return factory.num(0);
}

// **Substitute**
Node* NumberNode::substitute(const std::string& variable, Node* value, NodeFactory &factory) const {
    // A number is unaffected by substitution
    return const_cast<NumberNode*>(this); 
}

// **Clone**
Node* NumberNode::clone(NodeFactory &factory) const {
    return factory.num(value);
}

bool NumberNode::equals(const Node* other) const {
    if (const NumberNode* num = dynamic_cast<const NumberNode*>(other)) {
        return std::fabs(value - num->value) < 1e-9;
    }
    return false;
}

double NumberNode::getValue() const {
    return value;
}

bool NumberNode::extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const {
    coeff = 0;
    constant = value;
    return true;
}

} // namespace Expression
