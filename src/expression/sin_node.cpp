#include "expression/sin_node.h"
#include "expression/number_node.h"
#include "expression/multiplication_node.h"
#include "expression/cos_node.h"
#include "helpers/node_factory.h"


namespace Expression {

SinNode::SinNode(Node* operand)
    : UnaryOpNode(operand) {}

SinNode::~SinNode() {}

double SinNode::evaluate(const Env &env) const {
    double opVal = operand->evaluate(env);
    double result = std::sin(opVal);
    return result;
}

std::string SinNode::toString() const {
    return "sin(" + operand->toString() + ")";
}

// **Simplification**
Node* SinNode::simplify(NodeFactory &factory) const {
    Node* simplifiedOperand = operand->simplify(factory);

    // Constant folding: if the operand is a constant, evaluate sin immediately.
    if (auto num = dynamic_cast<NumberNode*>(simplifiedOperand)) {
        double value = num->getValue();
        Node* folded = factory.num(std::sin(value));
        return folded;
    }

    Node* result = factory.sin(simplifiedOperand);
    return result;
}

// **Differentiation (d/dx sin(x) = cos(x)*dx)**
Node* SinNode::derivative(const std::string& variable, NodeFactory &factory) const {
    // cos(operand)* operand->derivative(variable)
    Node* cosTerm = factory.cos(operand->clone(factory));
    Node* opDeriv = operand->derivative(variable, factory);
    Node* derivativeResult = factory.mul(cosTerm, opDeriv);

    return derivativeResult;
}

// **Substitution**
Node* SinNode::substitute(const std::string& variable, Node* value, NodeFactory &factory) const {
    Node* newOperand = operand->substitute(variable, value, factory);
    Node* result = factory.sin(newOperand);
    return result;
}

// **Clone**
Node* SinNode::clone(NodeFactory &factory) const {
    return factory.sin(operand->clone(factory));
}

bool SinNode::equals(const Node* other) const {
    if (const SinNode* sinNode = dynamic_cast<const SinNode*>(other)) {
        return operand->equals(sinNode->operand);
    }
    return false;
}

bool SinNode::extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const {
    double a = 0, b = 0;
    if (operand->extractLinearCoeffs(var, a, b)) {
        if (a == 0) {  // Operand is constant.
            coeff = 0;
            constant = std::sin(b);
            return true;
        }
    }
    return false;
}


} // namespace Expression
