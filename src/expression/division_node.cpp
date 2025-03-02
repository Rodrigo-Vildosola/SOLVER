#include "expression/division_node.h"
#include "expression/multiplication_node.h"
#include "expression/subtraction_node.h"
#include "expression/exponentiation_node.h"
#include "expression/number_node.h"
#include "helpers/node_factory.h"

#include <stdexcept>
#include <cmath>

namespace Expression {

DivisionNode::DivisionNode(Node* left, Node* right)
    : BinaryOpNode(left, right) {}

DivisionNode::~DivisionNode() {}

double DivisionNode::evaluate(const Env &env) const {
    double leftVal = left->evaluate(env);
    double rightVal = right->evaluate(env);

    if (rightVal == 0) {
        throw std::runtime_error("Division by zero error in " + toString());
    }

    double result = leftVal / rightVal;
    return result;
}

std::string DivisionNode::toString() const {
    return "(" + left->toString() + " / " + right->toString() + ")";
}

// **Symbolic Simplification** (Full Arena)
Node* DivisionNode::simplify(NodeFactory &factory) const {
    Node* leftSimplified = left->simplify(factory);
    Node* rightSimplified = right->simplify(factory);

    // If both sides are numbers, perform constant folding.
    if (auto leftNum = dynamic_cast<NumberNode*>(leftSimplified)) {
        if (auto rightNum = dynamic_cast<NumberNode*>(rightSimplified)) {
            if (rightNum->getValue() == 0) {
                throw std::runtime_error("Attempted division by zero in simplification.");
            }
            return factory.num(leftNum->getValue() / rightNum->getValue());
        }
    }

    // Identity rule: x / 1 = x
    if (auto rightNum = dynamic_cast<NumberNode*>(rightSimplified)) {
        if (rightNum->getValue() == 1) {
            return leftSimplified; 
        }
    }

    // Zero rule: 0 / x = 0
    if (auto leftNum = dynamic_cast<NumberNode*>(leftSimplified)) {
        if (leftNum->getValue() == 0) {
            return factory.num(0);
        }
    }

    return factory.div(leftSimplified, rightSimplified);
}

// **Symbolic Differentiation (Quotient Rule)**
Node* DivisionNode::derivative(const std::string& variable, NodeFactory &factory) const {
    Node* f_prime = left->derivative(variable, factory);
    Node* g_prime = right->derivative(variable, factory);

    // numerator = (f'(x) * g(x)) - (f(x) * g'(x))
    Node* numerator = factory.sub(
        factory.mul(f_prime, right->clone(factory)),
        factory.mul(left->clone(factory), g_prime)
    );

    // denominator = (g(x) * g(x))
    Node* denominator = factory.mul(right->clone(factory), right->clone(factory));

    return factory.div(numerator, denominator);
}

// **Symbolic Substitution**
Node* DivisionNode::substitute(const std::string& variable, Node* value, NodeFactory &factory) const {
    Node* newLeft = left->substitute(variable, value, factory);
    Node* newRight = right->substitute(variable, value, factory);
    return factory.div(newLeft, newRight);
}

// **Clone**
Node* DivisionNode::clone(NodeFactory &factory) const {
    return factory.div(left->clone(factory), right->clone(factory));
}

bool DivisionNode::equals(const Node* other) const {
    if (const DivisionNode* div = dynamic_cast<const DivisionNode*>(other)) {
        return left->equals(div->left) && right->equals(div->right);
    }
    return false;
}

bool DivisionNode::extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const {
    double a_num = 0, b_num = 0;
    double a_den = 0, b_den = 0;
    if (left->extractLinearCoeffs(var, a_num, b_num) &&
        right->extractLinearCoeffs(var, a_den, b_den)) {
        // We require denominator to be constant.
        if (a_den == 0 && b_den != 0) {
            coeff = a_num / b_den;
            constant = b_num / b_den;
            return true;
        }
    }
    return false;
}

} // namespace Expression
