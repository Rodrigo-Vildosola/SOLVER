#include "expression/neg_node.h"
#include "expression/number_node.h"
#include "helpers/node_factory.h"

namespace Expression {

NegNode::NegNode(Node* operand)
    : UnaryOpNode(operand) {}

NegNode::~NegNode() {}

double NegNode::evaluate(const Env &env) const {
    // Evaluate the operand and return its negation.
    return -operand->evaluate(env);
}

std::string NegNode::toString() const {
    // Represent negation as "-(...)".
    return "(-" + operand->toString() + ")";
}

Node* NegNode::simplify(NodeFactory &factory) const {
    // First, simplify the operand.
    Node* simplifiedOperand = operand->simplify(factory);
    
    // If the operand is a constant, perform constant folding.
    if (auto num = dynamic_cast<NumberNode*>(simplifiedOperand)) {
        double foldedValue = -num->getValue();
        return factory.num(foldedValue);
    }
    
    // Eliminate double negation: -(-x) => x.
    if (auto innerNeg = dynamic_cast<NegNode*>(simplifiedOperand)) {
        // Simply return the inner operand's simplified form.
        return innerNeg->operand->simplify(factory);
    }
    
    // Otherwise, return a new NegNode constructed via the factory.
    return factory.neg(simplifiedOperand);
}

Node* NegNode::derivative(const std::string& variable, NodeFactory &factory) const {
    // d/dx (-f(x)) = -f'(x)
    Node* dOperand = operand->derivative(variable, factory);
    Node* result = factory.neg(dOperand);
    return result;
}

Node* NegNode::substitute(const std::string& variable, Node* value, NodeFactory &factory) const {
    // Substitute in the operand and return its negation.
    Node* substitutedOperand = operand->substitute(variable, value, factory);
    Node* result = factory.neg(substitutedOperand);
    return result;
}

Node* NegNode::clone(NodeFactory &factory) const {
    // Clone the operand and create a new NegNode.
    return factory.neg(operand->clone(factory));
}

bool NegNode::extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const {
    // If the operand can be represented as a linear function a*x + b,
    // then -f(x) = (-a)*x + (-b)
    double a = 0, b = 0;
    if (operand->extractLinearCoeffs(var, a, b)) {
        coeff = -a;
        constant = -b;
        return true;
    }
    return false;
}

bool NegNode::equals(const Node* other) const {
    if (const NegNode* neg = dynamic_cast<const NegNode*>(other)) {
        return operand->equals(neg->operand);
    }
    return false;
}

} // namespace Expression
