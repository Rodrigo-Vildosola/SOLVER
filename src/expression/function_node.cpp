#include "expression/function_node.h"
#include "helpers/node_factory.h"


namespace Expression {

FunctionNode::FunctionNode(const std::string& name, int expectedArgCount, const std::vector<Node*>& arguments, FunctionCallback callback)
    : name(name), expectedArgCount(expectedArgCount), arguments(arguments), callback(callback)
{
    if (arguments.size() != (size_t)expectedArgCount) {
        throw std::runtime_error("Function " + name + " expects " + std::to_string(expectedArgCount) +
                                 " arguments, but got " + std::to_string(arguments.size()));
    }
}

FunctionNode::~FunctionNode() {
    if (AllocationConfig::getPolicy() == AllocatorPolicy::Default) {
        for (auto &arg : arguments) {
            delete arg;
        }
    }
}

double FunctionNode::evaluate(const Env &env) const {
    if (arguments.size() != (size_t)expectedArgCount) {
        throw std::runtime_error("Function " + name + " expects " + std::to_string(expectedArgCount) +
                                 " arguments, but got " + std::to_string(arguments.size()));
    }
    std::vector<double> argVals;
    for (auto arg : arguments) {
        argVals.push_back(arg->evaluate(env));
    }
    double result = callback(argVals);
    return result;
}

std::string FunctionNode::toString() const {
    std::ostringstream oss;
    oss << name << "(";
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << arguments[i]->toString();
    }
    oss << ")";
    return oss.str();
}

// **Simplify** 
Node* FunctionNode::simplify(NodeFactory &factory) const {
    std::vector<Node*> newArgs;
    newArgs.reserve(arguments.size());
    for (auto arg : arguments) {
        newArgs.push_back(arg->simplify(factory));
    }
    // create a new FunctionNode with simplified args
    // or you might do factory.func(...)
    Node* simplified = factory.func(name, expectedArgCount, newArgs, callback);
    return simplified;
}

// **Derivative** - not yet implemented
Node* FunctionNode::derivative(const std::string& variable, NodeFactory &factory) const {
    return clone(factory);
}

// **Substitution**
Node* FunctionNode::substitute(const std::string& variable, Node* value, NodeFactory &factory) const {
    std::vector<Node*> newArgs;
    newArgs.reserve(arguments.size());
    for (auto arg : arguments) {
        newArgs.push_back(arg->substitute(variable, value, factory));
    }
    return factory.func(name, expectedArgCount, newArgs, callback);
}

// **Clone**
Node* FunctionNode::clone(NodeFactory &factory) const {
    // clone each argument
    std::vector<Node*> clonedArgs;
    clonedArgs.reserve(arguments.size());
    for (auto arg : arguments) {
        clonedArgs.push_back(arg->clone(factory));
    }
    return factory.func(name, expectedArgCount, clonedArgs, callback);
}

bool FunctionNode::equals(const Node* other) const {
    if (const FunctionNode* func = dynamic_cast<const FunctionNode*>(other)) {
        if (name != func->name || expectedArgCount != func->expectedArgCount || arguments.size() != func->arguments.size())
            return false;
        for (size_t i = 0; i < arguments.size(); ++i) {
            if (!arguments[i]->equals(func->arguments[i])) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool FunctionNode::extractLinearCoeffs(const std::string &var, double &coeff, double &constant) const {
    bool allConst = true;
    std::vector<double> argVals;
    for (auto arg : arguments) {
        double a = 0, b = 0;
        if (!arg->extractLinearCoeffs(var, a, b)) {
            allConst = false;
            break;
        }
        if (a != 0) {
            allConst = false;
            break;
        }
        argVals.push_back(b);
    }
    if (allConst) {
        // If all arguments are constant, the function itself is constant.
        coeff = 0;
        constant = callback(argVals);
        return true;
    }
    return false;
}

} // namespace Expression
