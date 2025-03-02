#include "expression/variable_node.h"
#include "expression/number_node.h"
#include "helpers/node_factory.h"


namespace Expression {

VariableNode::VariableNode(const std::string& name) 
    : name(name) {}

VariableNode::~VariableNode() {}

double VariableNode::evaluate(const Env &env) const {
    auto it = env.find(name);
    if (it != env.end()) {
        return it->second;
    }
    return 0.0;  // default to 0 if variable not found
}

std::string VariableNode::toString() const {
    return name;
}

// **Simplify** (Full Arena)
Node* VariableNode::simplify(NodeFactory &factory) const {
    // A variable is already "simplified"
    return const_cast<VariableNode*>(this); 
}

// **Derivative**
Node* VariableNode::derivative(const std::string& variable, NodeFactory &factory) const {
    // d/dx x = 1, d/dx y = 0
    double val = (name == variable) ? 1.0 : 0.0;
    return factory.num(val);
}

// **Substitution**
Node* VariableNode::substitute(const std::string& variable, Node* value, NodeFactory &factory) const {
    // If this variable is the one being substituted, return a clone of `value`
    if (name == variable) {
        return value->clone(factory);
    }
    // otherwise, no change
    return const_cast<VariableNode*>(this); 
}

Node* VariableNode::clone(NodeFactory &factory) const {
    return factory.var(name);
}

bool VariableNode::equals(const Node* other) const {
    if (const VariableNode* var = dynamic_cast<const VariableNode*>(other)) {
        return name == var->name;
    }
    return false;
}

bool VariableNode::extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const {
    if (name == var) {
        coeff = 1.0;
        constant = 0.0;
        return true;
    }
    return false;
}

} // namespace Expression
