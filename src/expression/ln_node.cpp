#include "expression/ln_node.h"
#include "expression/division_node.h"
#include "expression/number_node.h"
#include "helpers/node_factory.h"


namespace Expression {

LnNode::LnNode(Node* operand)
    : UnaryOpNode(operand) {}

LnNode::~LnNode() {}

double LnNode::evaluate(const Env &env) const {
    double opVal = operand->evaluate(env);
    if (opVal <= 0) {
        throw std::runtime_error("Math error: ln of non-positive number.");
    }
    double result = std::log(opVal);
    return result;
}

std::string LnNode::toString() const {
    return "ln(" + operand->toString() + ")";
}

// **Simplify**
Node* LnNode::simplify(NodeFactory &factory) const {
    Node* simplifiedOperand = operand->simplify(factory);

    // ln(1) = 0
    if (auto numNode = dynamic_cast<NumberNode*>(simplifiedOperand)) {
        if (numNode->getValue() == 1) {
            return factory.num(0);
        }
    }
    return factory.ln(simplifiedOperand);
}

// **Derivative**
Node* LnNode::derivative(const std::string& variable, NodeFactory &factory) const {
    // d/dx ln(x) = 1/x
    Node* one = factory.num(1);
    Node* clonedOperand = operand->clone(factory);
    return factory.div(one, clonedOperand);
}

// **Substitution**
Node* LnNode::substitute(const std::string& variable, Node* value, NodeFactory &factory) const {
    Node* newOperand = operand->substitute(variable, value, factory);
    return factory.ln(newOperand);
}

// **Clone**
Node* LnNode::clone(NodeFactory &factory) const {
    return factory.ln(operand->clone(factory));
}

bool LnNode::equals(const Node* other) const {
    if (const LnNode* ln = dynamic_cast<const LnNode*>(other)) {
        return operand->equals(ln->operand);
    }
    return false;
}

bool LnNode::extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const {
    double a = 0, b = 0;
    if (operand->extractLinearCoeffs(var, a, b)) {
        if (a == 0) {  // Operand is constant.
            coeff = 0;
            constant = std::log(b);
            return true;
        }
    }
    return false;
}


} // namespace Expression
