#include "expression/exponentiation_node.h"
#include "expression/number_node.h"
#include "expression/multiplication_node.h"
#include "expression/addition_node.h"
#include "expression/division_node.h"
#include "expression/ln_node.h"
#include "helpers/node_factory.h"


namespace Expression {

ExponentiationNode::ExponentiationNode(Node* base, Node* exponent)
    : BinaryOpNode(base, exponent) {}

ExponentiationNode::~ExponentiationNode() {
    // no child deletion
}

double ExponentiationNode::evaluate(const Env &env) const {
    double baseVal = left->evaluate(env);
    double exponentVal = right->evaluate(env);

    if (baseVal == 0 && exponentVal <= 0) {
        throw std::runtime_error("Math error: 0 raised to a non-positive exponent.");
    }
    double result = std::pow(baseVal, exponentVal);
    return result;
}

std::string ExponentiationNode::toString() const {
    return "(" + left->toString() + " ^ " + right->toString() + ")";
}

// **Symbolic Simplification**
Node* ExponentiationNode::simplify(NodeFactory &factory) const {
    Node* baseSimplified = left->simplify(factory);
    Node* exponentSimplified = right->simplify(factory);

    // Constant folding: if both base and exponent are constants, compute the power.
    if (auto baseNum = dynamic_cast<NumberNode*>(baseSimplified)) {
        if (auto exponentNum = dynamic_cast<NumberNode*>(exponentSimplified)) {
            double value = std::pow(baseNum->getValue(), exponentNum->getValue());
            Node* folded = factory.num(value);
            return folded;
        }
    }

    // Special cases:
    // x^0 = 1
    if (auto exponentNum = dynamic_cast<NumberNode*>(exponentSimplified)) {
        if (exponentNum->getValue() == 0) {
            Node* folded = factory.num(1);
            return folded;
        }
        // x^1 = x
        if (exponentNum->getValue() == 1) {
            return baseSimplified;
        }
    }

    // 0^x = 0 (except 0^0, which is undefined, but assumed handled elsewhere)
    if (auto baseNum = dynamic_cast<NumberNode*>(baseSimplified)) {
        if (baseNum->getValue() == 0) {
            Node* folded = factory.num(0);
            return folded;
        }
        // 1^x = 1
        if (baseNum->getValue() == 1) {
            Node* folded = factory.num(1);
            return folded;
        }
    }

    Node* result = factory.exp(baseSimplified, exponentSimplified);
    return result;
}

// **Symbolic Differentiation (General Power Rule)**
Node* ExponentiationNode::derivative(const std::string& variable, NodeFactory &factory) const {
    // If exponent is constant, apply power rule: d/dx (f(x)^n) = n * f(x)^(n-1) * f'(x)
    if (auto exponentNum = dynamic_cast<NumberNode*>(right)) {
        double n = exponentNum->getValue();
        Node* factor1 = factory.mul( factory.num(n),
            factory.exp(left->clone(factory), factory.num(n - 1))
        );
        Node* factor2 = left->derivative(variable, factory);
        return factory.mul(factor1, factor2);
    }

    // General case: d/dx (f^g) = f^g * ( g'(x)*ln(f) + g(x)*f'(x)/f )
    Node* term1 = factory.mul(right->derivative(variable, factory),
                              factory.ln(left->clone(factory)));
    Node* term2 = factory.mul(right->clone(factory),
                              factory.div(left->derivative(variable, factory),
                                          left->clone(factory)));
    Node* sum = factory.add(term1, term2);
    return factory.mul( factory.exp(left->clone(factory), right->clone(factory)), sum );
}

// **Symbolic Substitution**
Node* ExponentiationNode::substitute(const std::string& variable, Node* value, NodeFactory &factory) const {
    Node* newBase = left->substitute(variable, value, factory);
    Node* newExp = right->substitute(variable, value, factory);
    return factory.exp(newBase, newExp);
}

// **Clone**
Node* ExponentiationNode::clone(NodeFactory &factory) const {
    return factory.exp(left->clone(factory), right->clone(factory));
}

bool ExponentiationNode::equals(const Node* other) const {
    if (const ExponentiationNode* exp = dynamic_cast<const ExponentiationNode*>(other)) {
        return left->equals(exp->left) && right->equals(exp->right);
    }
    return false;
}

bool ExponentiationNode::extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const {
    double a_exp = 0, b_exp = 0;
    // Check if exponent is constant.
    if (right->extractLinearCoeffs(var, a_exp, b_exp) && a_exp == 0) {
        double expVal = b_exp;
        if (expVal == 0) {
            // f(x)^0 = 1, constant.
            coeff = 0;
            constant = 1;
            return true;
        }
        if (expVal == 1) {
            // f(x)^1 = f(x): extract linear coeff from left.
            return left->extractLinearCoeffs(var, coeff, constant);
        }
    }
    return false;
}

} // namespace Expression
