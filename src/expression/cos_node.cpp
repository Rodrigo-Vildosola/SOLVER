#include "expression/cos_node.h"
#include "expression/number_node.h"
#include "expression/multiplication_node.h"
#include "expression/sin_node.h"
#include "helpers/node_factory.h"


namespace Expression {

CosNode::CosNode(Node* operand)
    : UnaryOpNode(operand) {}

CosNode::~CosNode() {}

double CosNode::evaluate(const Env &env) const {
    double opVal = operand->evaluate(env);
    double result = std::cos(opVal);
    return result;
}

std::string CosNode::toString() const {
    return "cos(" + operand->toString() + ")";
}

// **Simplification (Full Arena)**
Node* CosNode::simplify(NodeFactory &factory) const {
    Node* simplifiedOperand = operand->simplify(factory);

    // Constant folding: if the operand is a constant, evaluate cos immediately.
    if (auto num = dynamic_cast<NumberNode*>(simplifiedOperand)) {
        double value = num->getValue();
        Node* folded = factory.num(std::cos(value));
        return folded;
    }

    Node* result = factory.cos(simplifiedOperand);
    return result;
}

// **Differentiation** d/dx cos(x) = -sin(x) * dx
Node* CosNode::derivative(const std::string& variable, NodeFactory &factory) const {
    // -sin(operand) * operand->derivative
    Node* negativeOne = factory.num(-1);
    Node* sinTerm = factory.sin(operand->clone(factory));
    Node* derivTerm = operand->derivative(variable, factory);
    // multiply them: (-1) * ( sin(...) * deriv(...) )
    Node* mulInner = factory.mul(sinTerm, derivTerm);
    Node* derivativeResult = factory.mul(negativeOne, mulInner);

    return derivativeResult;
}

// **Substitution**
Node* CosNode::substitute(const std::string& variable, Node* value, NodeFactory &factory) const {
    Node* substitutedOperand = operand->substitute(variable, value, factory);
    Node* result = factory.cos(substitutedOperand);
    return result;
}

// **Clone**
Node* CosNode::clone(NodeFactory &factory) const {
    return factory.cos(operand->clone(factory));
}

bool CosNode::equals(const Node* other) const {
    if (const CosNode* cosNode = dynamic_cast<const CosNode*>(other)) {
        return operand->equals(cosNode->operand);
    }
    return false;
}

bool CosNode::extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const {
    double a = 0, b = 0;
    if (operand->extractLinearCoeffs(var, a, b)) {
        if (a == 0) {  // Operand is constant.
            coeff = 0;
            constant = std::cos(b);
            return true;
        }
    }
    return false;
}


} // namespace Expression
