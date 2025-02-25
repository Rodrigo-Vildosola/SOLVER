// Node.h
#pragma once

#include "token.h"
#include "function.h"
#include "symbol_table.h"

class Node {
public:
    virtual ~Node() {}
    
    // Evaluate the node given an environment (mapping variable names to values)
    virtual NUMBER_TYPE evaluate(const Env &env) const = 0;
    
    // Produce a human-readable representation of the node
    virtual std::string toString() const = 0;
    
    // Return the derivative of this node with respect to variable 'var'
    virtual Node* derivative(const std::string &var) const = 0;
    
    // Return a simplified version of this node
    virtual Node* simplify() const = 0;
};

class NumberNode : public Node {
    NUMBER_TYPE value;
public:
    explicit NumberNode(NUMBER_TYPE v) : value(v) {}

    NUMBER_TYPE evaluate(const Env &env) const override {
        return value;
    }

    std::string toString() const override {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

        // The derivative of a constant is 0.
    Node* derivative(const std::string &var) const override {
        return new NumberNode(0);
    }

    // A constant is already simplified.
    Node* simplify() const override {
        return new NumberNode(value);
    }
};

class VariableNode : public Node {
    std::string name;
public:
    explicit VariableNode(const std::string &n) : name(n) {}

    NUMBER_TYPE evaluate(const Env &env) const override {
        auto it = env.find(name);
        if (it == env.end())
            throw std::runtime_error("Variable not found: " + name);
        return it->second;
    }

    std::string toString() const override {
        return name;
    }

    Node* derivative(const std::string &var) const override {
        return new NumberNode((var == name) ? 1 : 0);
    }

    // A variable is already as simple as it can be.
    Node* simplify() const override {
        return new VariableNode(name);
    }
};

//--------------------------------------------------
// AdditionNode
//--------------------------------------------------
class AdditionNode : public Node {
    Node* left;
    Node* right;
public:
    AdditionNode(Node* l, Node* r)
        : left(l), right(r) {}

    ~AdditionNode() override {
        delete left;
        delete right;
    }

    NUMBER_TYPE evaluate(const Env &env) const override {
        return left->evaluate(env) + right->evaluate(env);
    }

    std::string toString() const override {
        return "(" + left->toString() + " + " + right->toString() + ")";
    }

    Node* derivative(const std::string &var) const override {
        return new AdditionNode(left->derivative(var), right->derivative(var));
    }

    Node* simplify() const override {
        Node* sleft = left->simplify();
        Node* sright = right->simplify();

        // If both sides are numbers, compute the result.
        NumberNode* nleft = dynamic_cast<NumberNode*>(sleft);
        NumberNode* nright = dynamic_cast<NumberNode*>(sright);
        if(nleft && nright) {
            NUMBER_TYPE sum = nleft->evaluate(Env{}) + nright->evaluate(Env{});
            delete sleft; delete sright;
            return new NumberNode(sum);
        }
        // If one side is zero, return the other.
        if(nleft && nleft->evaluate(Env{}) == 0) {
            delete sleft;
            return sright;
        }
        if(nright && nright->evaluate(Env{}) == 0) {
            delete sright;
            return sleft;
        }
        return new AdditionNode(sleft, sright);
    }
};

//--------------------------------------------------
// SubtractionNode
//--------------------------------------------------
class SubtractionNode : public Node {
    Node* left;
    Node* right;
public:
    SubtractionNode(Node* l, Node* r)
        : left(l), right(r) {}

    ~SubtractionNode() override {
        delete left;
        delete right;
    }

    NUMBER_TYPE evaluate(const Env &env) const override {
        return left->evaluate(env) - right->evaluate(env);
    }

    std::string toString() const override {
        return "(" + left->toString() + " - " + right->toString() + ")";
    }

    Node* derivative(const std::string &var) const override {
        return new SubtractionNode(left->derivative(var), right->derivative(var));
    }

    Node* simplify() const override {
        Node* sleft = left->simplify();
        Node* sright = right->simplify();

        NumberNode* nleft = dynamic_cast<NumberNode*>(sleft);
        NumberNode* nright = dynamic_cast<NumberNode*>(sright);
        if(nleft && nright) {
            NUMBER_TYPE diff = nleft->evaluate(Env{}) - nright->evaluate(Env{});
            delete sleft; delete sright;
            return new NumberNode(diff);
        }
        // If the right side is 0, return the left side.
        if(nright && nright->evaluate(Env{}) == 0) {
            delete sright;
            return sleft;
        }
        return new SubtractionNode(sleft, sright);
    }
};

//--------------------------------------------------
// MultiplicationNode
//--------------------------------------------------
class MultiplicationNode : public Node {
    Node* left;
    Node* right;
public:
    MultiplicationNode(Node* l, Node* r)
        : left(l), right(r) {}

    ~MultiplicationNode() override {
        delete left;
        delete right;
    }

    NUMBER_TYPE evaluate(const Env &env) const override {
        return left->evaluate(env) * right->evaluate(env);
    }

    std::string toString() const override {
        return "(" + left->toString() + " * " + right->toString() + ")";
    }

    Node* derivative(const std::string &var) const override {
        // Product rule: u' * v + u * v'
        return new AdditionNode(
            new MultiplicationNode(left->derivative(var), right->simplify()),
            new MultiplicationNode(left->simplify(), right->derivative(var))
        );
    }

    Node* simplify() const override {
        Node* sleft = left->simplify();
        Node* sright = right->simplify();
        NumberNode* nleft = dynamic_cast<NumberNode*>(sleft);
        NumberNode* nright = dynamic_cast<NumberNode*>(sright);
        if(nleft && nright) {
            NUMBER_TYPE prod = nleft->evaluate(Env{}) * nright->evaluate(Env{});
            delete sleft; delete sright;
            return new NumberNode(prod);
        }
        // If one side is 0, return 0.
        if(nleft && nleft->evaluate(Env{}) == 0) {
            delete sleft; delete sright;
            return new NumberNode(0);
        }
        if(nright && nright->evaluate(Env{}) == 0) {
            delete sleft; delete sright;
            return new NumberNode(0);
        }
        // If one side is 1, return the other.
        if(nleft && nleft->evaluate(Env{}) == 1) {
            delete sleft;
            return sright;
        }
        if(nright && nright->evaluate(Env{}) == 1) {
            delete sright;
            return sleft;
        }
        return new MultiplicationNode(sleft, sright);
    }
};

//--------------------------------------------------
// PowerNode
//--------------------------------------------------
class PowerNode : public Node {
    Node* base;
    Node* exponent;
public:
    PowerNode(Node* b, Node* e)
        : base(b), exponent(e) {}

    ~PowerNode() override {
        delete base;
        delete exponent;
    }

    NUMBER_TYPE evaluate(const Env &env) const override {
        return std::pow(base->evaluate(env), exponent->evaluate(env));
    }

    std::string toString() const override {
        return "(" + base->toString() + " ^ " + exponent->toString() + ")";
    }

    Node* derivative(const std::string &var) const override {
        // For simplicity, assume the exponent is a constant.
        NumberNode* nExp = dynamic_cast<NumberNode*>(exponent);
        if (!nExp)
            throw std::runtime_error("PowerNode derivative: exponent must be constant in this implementation.");
        NUMBER_TYPE expVal = nExp->evaluate(Env{});
        return new MultiplicationNode(
            new MultiplicationNode(
                new NumberNode(expVal),
                new PowerNode(base->simplify(), new NumberNode(expVal - 1))
            ),
            base->derivative(var)
        );
    }

    Node* simplify() const override {
        Node* sbase = base->simplify();
        Node* sexponent = exponent->simplify();
        NumberNode* nb = dynamic_cast<NumberNode*>(sbase);
        NumberNode* ne = dynamic_cast<NumberNode*>(sexponent);
        if(nb && ne) {
            NUMBER_TYPE result = std::pow(nb->evaluate(Env{}), ne->evaluate(Env{}));
            delete sbase; delete sexponent;
            return new NumberNode(result);
        }
        // x^0 = 1, x^1 = x
        if(ne && ne->evaluate(Env{}) == 0) {
            delete sbase; delete sexponent;
            return new NumberNode(1);
        }
        if(ne && ne->evaluate(Env{}) == 1) {
            delete sexponent;
            return sbase;
        }
        return new PowerNode(sbase, sexponent);
    }
};


//--------------------------------------------------
// DivisionNode
//--------------------------------------------------
class DivisionNode : public Node {
    Node* left;
    Node* right;
public:
    DivisionNode(Node* l, Node* r)
        : left(l), right(r) {}

    ~DivisionNode() override {
        delete left;
        delete right;
    }

    NUMBER_TYPE evaluate(const Env &env) const override {
        NUMBER_TYPE rVal = right->evaluate(env);
        if (rVal == 0)
            throw std::runtime_error("Division by zero");
        return left->evaluate(env) / rVal;
    }

    std::string toString() const override {
        return "(" + left->toString() + " / " + right->toString() + ")";
    }

    Node* derivative(const std::string &var) const override {
        // Quotient rule: (u'v - uv') / v^2
        return new DivisionNode(
            new SubtractionNode(
                new MultiplicationNode(left->derivative(var), right->simplify()),
                new MultiplicationNode(left->simplify(), right->derivative(var))
            ),
            new PowerNode(right->simplify(), new NumberNode(2))
        );
    }

    Node* simplify() const override {
        Node* sleft = left->simplify();
        Node* sright = right->simplify();
        NumberNode* nleft = dynamic_cast<NumberNode*>(sleft);
        NumberNode* nright = dynamic_cast<NumberNode*>(sright);
        if(nleft && nright) {
            NUMBER_TYPE quot = nleft->evaluate(Env{}) / nright->evaluate(Env{});
            delete sleft; delete sright;
            return new NumberNode(quot);
        }
        // If numerator is 0, return 0.
        if(nleft && nleft->evaluate(Env{}) == 0) {
            delete sleft; delete sright;
            return new NumberNode(0);
        }
        // If denominator is 1, return numerator.
        if(nright && nright->evaluate(Env{}) == 1) {
            delete sright;
            return sleft;
        }
        return new DivisionNode(sleft, sright);
    }
};

//--------------------------------------------------
// FunctionNode
//--------------------------------------------------
class FunctionNode : public Node {
    std::string name;
    std::vector<Node*> args;
    const Function* functionRef;
public:
    FunctionNode(const std::string &name, std::vector<Node*> arguments, const Function* func)
        : name(name), args(std::move(arguments)), functionRef(func) {}

    ~FunctionNode() override {
        for (Node* arg : args)
            delete arg;
    }

    NUMBER_TYPE evaluate(const Env &env) const override {
        std::vector<NUMBER_TYPE> evaluatedArgs;
        for (Node* arg : args)
            evaluatedArgs.push_back(arg->evaluate(env));
        return functionRef->callback(evaluatedArgs);
    }

    std::string toString() const override {
        std::string s = name + "(";
        for (size_t i = 0; i < args.size(); ++i) {
            s += args[i]->toString();
            if (i != args.size() - 1)
                s += ", ";
        }
        s += ")";
        return s;
    }

    // For now, differentiation of function calls is not implemented.
    Node* derivative(const std::string &var) const override {
        throw std::runtime_error("FunctionNode derivative not implemented.");
    }

    // Simplify by simplifying each argument.
    Node* simplify() const override {
        std::vector<Node*> newArgs;
        for (Node* arg : args) {
            newArgs.push_back(arg->simplify());
        }
        return new FunctionNode(name, newArgs, functionRef);
    }
};
