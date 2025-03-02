#include "expression/addition_node.h"
#include "expression/number_node.h"
#include "helpers/node_factory.h"


namespace Expression {

AdditionNode::AdditionNode(Node* left, Node* right)
    : BinaryOpNode(left, right) {}

AdditionNode::~AdditionNode() {}

double AdditionNode::evaluate(const Env &env) const {
    double leftVal = left->evaluate(env);
    double rightVal = right->evaluate(env);
    double result = leftVal + rightVal;
    return result;
}

std::string AdditionNode::toString() const {
    return "(" + left->toString() + " + " + right->toString() + ")";
}

// **Symbolic Simplification (Full Arena)**
Node* AdditionNode::simplify(NodeFactory &factory) const {
    Node* leftSimplified = left->simplify(factory);
    Node* rightSimplified = right->simplify(factory);

    // If both sides are numbers, perform constant folding
    if (auto leftNum = dynamic_cast<NumberNode*>(leftSimplified)) {
        if (auto rightNum = dynamic_cast<NumberNode*>(rightSimplified)) {
            Node* result = factory.num(leftNum->getValue() + rightNum->getValue());
            return result;
        }
    }
    // Identity rule: x + 0 = x
    if (auto rightNum = dynamic_cast<NumberNode*>(rightSimplified)) {
        if (rightNum->getValue() == 0) {
            return leftSimplified;
        }
    }
    if (auto leftNum = dynamic_cast<NumberNode*>(leftSimplified)) {
        if (leftNum->getValue() == 0) {
            return rightSimplified;
        }
    }
    Node* result = factory.add(leftSimplified, rightSimplified);
    return result;
}

// **Symbolic Differentiation**
Node* AdditionNode::derivative(const std::string& variable, NodeFactory &factory) const {
    Node* derivedLeft = left->derivative(variable, factory);
    Node* derivedRight = right->derivative(variable, factory);
    Node* result = factory.add(derivedLeft, derivedRight);
    return result;
}

// **Symbolic Substitution**
Node* AdditionNode::substitute(const std::string& variable, Node* value, NodeFactory &factory) const {
    Node* newLeft = left->substitute(variable, value, factory);
    Node* newRight = right->substitute(variable, value, factory);
    Node* substituted = factory.add(newLeft, newRight);
    return substituted;
}

// **Clone**
Node* AdditionNode::clone(NodeFactory &factory) const {
    Node* clonedLeft = left->clone(factory);
    Node* clonedRight = right->clone(factory);
    return factory.add(clonedLeft, clonedRight);
}

bool AdditionNode::equals(const Node* other) const {
    if (const AdditionNode* add = dynamic_cast<const AdditionNode*>(other)) {
        // Addition is commutative.
        return (left->equals(add->left) && right->equals(add->right)) ||
               (left->equals(add->right) && right->equals(add->left));
    }
    return false;
}

bool AdditionNode::extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const {
    double a1 = 0, b1 = 0, a2 = 0, b2 = 0;
    bool ok1 = left->extractLinearCoeffs(var, a1, b1);
    bool ok2 = right->extractLinearCoeffs(var, a2, b2);
    if (ok1 || ok2) {
        coeff = a1 + a2;
        constant = b1 + b2;
        return true;
    }
    return false;
}

} // namespace Expression
