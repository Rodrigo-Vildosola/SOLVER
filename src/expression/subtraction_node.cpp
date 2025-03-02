#include "expression/subtraction_node.h"
#include "expression/number_node.h"
#include "helpers/node_factory.h"


namespace Expression {

SubtractionNode::SubtractionNode(Node* left, Node* right)
    : BinaryOpNode(left, right) {}

SubtractionNode::~SubtractionNode() {}

double SubtractionNode::evaluate(const Env &env) const {
    double leftVal = left->evaluate(env);
    double rightVal = right->evaluate(env);
    double result = leftVal - rightVal;
    return result;
}

std::string SubtractionNode::toString() const {
    return "(" + left->toString() + " - " + right->toString() + ")";
}

// **Symbolic Simplification**
Node* SubtractionNode::simplify(NodeFactory &factory) const {
    Node* leftSimplified = left->simplify(factory);
    Node* rightSimplified = right->simplify(factory);

    // If both sides are numbers => constant folding
    if (auto leftNum = dynamic_cast<NumberNode*>(leftSimplified)) {
        if (auto rightNum = dynamic_cast<NumberNode*>(rightSimplified)) {
            return factory.num(leftNum->getValue() - rightNum->getValue());
        }
    }

    // Identity rule: x - 0 = x
    if (auto rightNum = dynamic_cast<NumberNode*>(rightSimplified)) {
        if (rightNum->getValue() == 0) {
            return leftSimplified;
        }
    }

    // x - x = 0
    if (left->toString() == right->toString()) {
        return factory.num(0);
    }

    return factory.sub(leftSimplified, rightSimplified);
}

// **Symbolic Differentiation**
Node* SubtractionNode::derivative(const std::string& variable, NodeFactory &factory) const {
    Node* dLeft = left->derivative(variable, factory);
    Node* dRight = right->derivative(variable, factory);
    return factory.sub(dLeft, dRight);
}

// **Substitution**
Node* SubtractionNode::substitute(const std::string& variable, Node* value, NodeFactory &factory) const {
    Node* newLeft = left->substitute(variable, value, factory);
    Node* newRight = right->substitute(variable, value, factory);
    return factory.sub(newLeft, newRight);
}

// **Clone**
Node* SubtractionNode::clone(NodeFactory &factory) const {
    return factory.sub(left->clone(factory), right->clone(factory));
}

bool SubtractionNode::equals(const Node* other) const {
    if (const SubtractionNode* sub = dynamic_cast<const SubtractionNode*>(other)) {
        return left->equals(sub->left) && right->equals(sub->right);
    }
    return false;
}

bool SubtractionNode::extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const {
    double a1 = 0, b1 = 0, a2 = 0, b2 = 0;
    bool ok1 = left->extractLinearCoeffs(var, a1, b1);
    bool ok2 = right->extractLinearCoeffs(var, a2, b2);
    if (ok1 || ok2) {
        coeff = a1 - a2;
        constant = b1 - b2;
        return true;
    }
    return false;
}

} // namespace Expression
