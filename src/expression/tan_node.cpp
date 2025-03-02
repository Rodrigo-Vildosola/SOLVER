#include "expression/tan_node.h"
#include "expression/number_node.h"
#include "expression/cos_node.h"
#include "helpers/node_factory.h"

#include <cmath>

namespace Expression {

TanNode::TanNode(Node* operand)
    : UnaryOpNode(operand) {}

TanNode::~TanNode() {}

double TanNode::evaluate(const Env &env) const {
    double opVal = operand->evaluate(env);
    double result = std::tan(opVal);
    return result;
}

std::string TanNode::toString() const {
    return "tan(" + operand->toString() + ")";
}

Node* TanNode::simplify(NodeFactory &factory) const {
    Node* simplifiedOperand = operand->simplify(factory);

    // --- Constant Folding ---
    // If the operand is a constant (i.e. a NumberNode), compute tan immediately.
    if (NumberNode* num = dynamic_cast<NumberNode*>(simplifiedOperand)) {
        double value = num->getValue();  // Assumes NumberNode provides a getValue() method.
        Node* folded = factory.num(std::tan(value));
        return folded;
    }

    // --- Advanced Simplifications ---
    // Here you could add further pattern matching or rewrite rules.
    // For example, you might consider rewriting tan(x) as sin(x)/cos(x)
    // if that would allow further simplification down the line.
    // For now, we simply rebuild the TanNode.
    Node* result = factory.tan(simplifiedOperand);
    return result;
}

Node* TanNode::derivative(const std::string& variable, NodeFactory &factory) const {
    // Derivative of tan(x): d/dx tan(x) = sec^2(x)*dx/dx.
    // Since sec(x) = 1/cos(x), we write sec^2(x) as 1/(cos(x)*cos(x))
    Node* opDeriv = operand->derivative(variable, factory);
    Node* cosClone = factory.cos(operand->clone(factory));
    Node* cosSquared = factory.mul(cosClone, factory.cos(operand->clone(factory)));
    Node* secSquared = factory.div(factory.num(1), cosSquared);
    Node* derivativeResult = factory.mul(secSquared, opDeriv);
    return derivativeResult;
}

Node* TanNode::substitute(const std::string& variable, Node* value, NodeFactory &factory) const {
    Node* newOperand = operand->substitute(variable, value, factory);
    Node* result = factory.tan(newOperand);
    return result;
}

Node* TanNode::clone(NodeFactory &factory) const {
    return factory.tan(operand->clone(factory));
}

bool TanNode::equals(const Node* other) const {
    if (const TanNode* tanNode = dynamic_cast<const TanNode*>(other)) {
        return operand->equals(tanNode->operand);
    }
    return false;
}

bool TanNode::extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const {
    // tan(x) is non-linear except when the operand is a constant.
    double a = 0, b = 0;
    if (operand->extractLinearCoeffs(var, a, b)) {
        if (a == 0) {  // Constant operand.
            coeff = 0;
            constant = std::tan(b);
            return true;
        }
    }
    return false;
}

} // namespace Expression
