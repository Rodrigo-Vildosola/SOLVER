#include "expression/equality_node.h"
#include "expression/addition_node.h"
#include "expression/subtraction_node.h"
#include "expression/multiplication_node.h"
#include "expression/variable_node.h"
#include "expression/number_node.h"
#include "helpers/node_factory.h"


namespace Expression {

// A naive helper that tries to extract coefficients from a simplified AST.
// It assumes the expression is linear in the variable `var`.
// If successful, it sets a and b such that the expression is a*x + b.
bool extractLinearCoeffs(Node* expr, const std::string &var, double &a, double &b) {
    // If the expression is a constant.
    if(auto num = dynamic_cast<NumberNode*>(expr)) {
        a = 0;
        b = num->getValue();
        return true;
    }
    // If the expression is the variable.
    if(auto varNode = dynamic_cast<VariableNode*>(expr)) {
        if(varNode->toString() == var) {
            a = 1;
            b = 0;
            return true;
        }
        return false; // some other variable
    }
    // If the expression is a multiplication.
    if(auto mult = dynamic_cast<MultiplicationNode*>(expr)) {
        // Try to find a constant multiplied by the variable.
        double c = 0;
        bool foundConstant = false;
        Node* other = nullptr;
        if(auto leftNum = dynamic_cast<NumberNode*>(mult->left)) {
            c = leftNum->getValue();
            foundConstant = true;
            other = mult->right;
        } else if(auto rightNum = dynamic_cast<NumberNode*>(mult->right)) {
            c = rightNum->getValue();
            foundConstant = true;
            other = mult->left;
        }
        if(foundConstant && other) {
            if(auto otherVar = dynamic_cast<VariableNode*>(other)) {
                if(otherVar->toString() == var) {
                    a = c;
                    b = 0;
                    return true;
                }
            }
        }
    }
    // If the expression is an addition.
    if(auto add = dynamic_cast<AdditionNode*>(expr)) {
        double a1 = 0, b1 = 0, a2 = 0, b2 = 0;
        bool ok1 = extractLinearCoeffs(add->left, var, a1, b1);
        bool ok2 = extractLinearCoeffs(add->right, var, a2, b2);
        a = a1 + a2;
        b = b1 + b2;
        return ok1 || ok2;
    }
    // If the expression is a subtraction.
    if(auto sub = dynamic_cast<SubtractionNode*>(expr)) {
        double a1 = 0, b1 = 0, a2 = 0, b2 = 0;
        bool ok1 = extractLinearCoeffs(sub->left, var, a1, b1);
        bool ok2 = extractLinearCoeffs(sub->right, var, a2, b2);
        a = a1 - a2;
        b = b1 - b2;
        return ok1 || ok2;
    }
    return false;
}


EqualityNode::EqualityNode(Node* left, Node* right)
    : left(left), right(right) {}

EqualityNode::~EqualityNode() {
    if (AllocationConfig::getPolicy() == AllocatorPolicy::Default) {
        delete left;
        delete right;
    }
}

double EqualityNode::evaluate(const Env &env) const {
    double leftVal = left->evaluate(env);
    double rightVal = right->evaluate(env);
    bool equal = (std::fabs(leftVal - rightVal) < 1e-9);
    return equal ? 1.0 : 0.0;
}

std::string EqualityNode::toString() const {
    return "(" + left->toString() + " == " + right->toString() + ")";
}

// **Simplify** 
Node* EqualityNode::simplify(NodeFactory &factory) const {
    Node* leftS = left->simplify(factory);
    Node* rightS = right->simplify(factory);
    if (leftS->equals(rightS)) {
        return factory.num(1);
    }
    return factory.eq(leftS, rightS);
}

// **Derivative** d/dx of an equation is just derivative of each side
Node* EqualityNode::derivative(const std::string& variable, NodeFactory &factory) const {
    Node* leftD = left->derivative(variable, factory);
    Node* rightD = right->derivative(variable, factory);
    return factory.eq(leftD, rightD);
}

// **Substitute variable with a value/expression**
Node* EqualityNode::substitute(const std::string& variable, Node* value, NodeFactory &factory) const {
    Node* leftSub = left->substitute(variable, value, factory);
    Node* rightSub = right->substitute(variable, value, factory);
    return factory.eq(leftSub, rightSub);
}

// **Clone**
Node* EqualityNode::clone(NodeFactory &factory) const {
    return factory.eq(left->clone(factory), right->clone(factory));
}

// **solveFor** 
Node* EqualityNode::solveFor(const std::string& variable, NodeFactory &factory) const {
    // Create f(x) = left - right
    Node* diff = factory.sub(left->clone(factory), right->clone(factory));
    Node* simplifiedDiff = diff->simplify(factory);  // Simplify before extracting coefficients.

    double a = 0;
    double b = 0;

    // Try to extract coefficients a and b where the equation is: a * x + b = 0
    if (!simplifiedDiff->extractLinearCoeffs(variable, a, b) || a == 0) {
        return simplifiedDiff->clone(factory);  // Return unchanged if not solvable.
    }

    double solution = -b / a;
    Node* solNode = factory.num(solution);
    return solNode;
}

} // namespace Expression
