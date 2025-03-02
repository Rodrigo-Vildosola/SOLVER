#include "expression/log_node.h"
#include "expression/ln_node.h"
#include "expression/division_node.h"
#include "expression/multiplication_node.h"
#include "expression/number_node.h"
#include "helpers/node_factory.h"


namespace Expression {

LogNode::LogNode(Node* base, Node* operand)
    : BinaryOpNode(base, operand) {}

LogNode::~LogNode() {}

double LogNode::evaluate(const Env &env) const {
    double baseVal = left->evaluate(env);
    double operandVal = right->evaluate(env);

    if (baseVal <= 0 || baseVal == 1 || operandVal <= 0) {
        throw std::runtime_error("Math error: log with invalid base or operand.");
    }
    double result = std::log(operandVal) / std::log(baseVal);
    return result;
}

std::string LogNode::toString() const {
    return "log(" + left->toString() + ", " + right->toString() + ")";
}

// **Simplify**
Node* LogNode::simplify(NodeFactory &factory) const {
    Node* baseS = left->simplify(factory);
    Node* operandS = right->simplify(factory);

    // log_b(b) = 1
    if (baseS->equals(operandS)) {
        return factory.num(1);
    }

    return factory.log(baseS, operandS);
}

// **Derivative**
Node* LogNode::derivative(const std::string& variable, NodeFactory &factory) const {
    // d/dx log_b(x) = 1 / (x ln(b))
    Node* clonedOperand = right->clone(factory);
    Node* lnBase = factory.ln(left->clone(factory));
    Node* denominator = factory.mul(clonedOperand, lnBase);

    Node* one = factory.num(1);
    return factory.div(one, denominator);
}

// **Substitute**
Node* LogNode::substitute(const std::string& variable, Node* value, NodeFactory &factory) const {
    Node* newBase = left->substitute(variable, value, factory);
    Node* newOperand = right->substitute(variable, value, factory);
    return factory.log(newBase, newOperand);
}

// **Clone**
Node* LogNode::clone(NodeFactory &factory) const {
    return factory.log(left->clone(factory), right->clone(factory));
}

bool LogNode::equals(const Node* other) const {
    if (const LogNode* log = dynamic_cast<const LogNode*>(other)) {
        // Addition is commutative.
        return (left->equals(log->left) && right->equals(log->right)) ||
               (left->equals(log->right) && right->equals(log->left));
    }
    return false;
}

bool LogNode::extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const {
    double a_base = 0, b_base = 0;
    double a_operand = 0, b_operand = 0;
    if (left->extractLinearCoeffs(var, a_base, b_base) &&
        right->extractLinearCoeffs(var, a_operand, b_operand)) {
        // Both must be constant for log to be constant.
        if (a_base == 0 && a_operand == 0) {
            coeff = 0;
            constant = std::log(b_operand) / std::log(b_base);
            return true;
        }
    }
    return false;
}

} // namespace Expression
