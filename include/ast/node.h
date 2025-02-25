// ASTNode.h
#pragma once

#include "token.h"
#include "function.h"
#include "symbol_table.h"

class ASTNode {
public:
    virtual ~ASTNode() {}
    
    // Evaluate the node given an environment (mapping variable names to values)
    virtual NUMBER_TYPE evaluate(const Env &env) const = 0;
    
    // Produce a human-readable representation of the node
    virtual std::string toString() const = 0;
    
    // For later: virtual methods for differentiation, simplification, etc.
    // virtual ASTNode* derivative(const std::string& var) const = 0;
    // virtual ASTNode* simplify() const = 0;
};

class NumberNode : public ASTNode {
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
};

class VariableNode : public ASTNode {
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
};

class AdditionNode : public ASTNode {
    ASTNode* left;
    ASTNode* right;
public:
    AdditionNode(ASTNode* l, ASTNode* r)
        : left(std::move(l)), right(std::move(r)) {}

    NUMBER_TYPE evaluate(const Env &env) const override {
        return left->evaluate(env) + right->evaluate(env);
    }

    std::string toString() const override {
        return "(" + left->toString() + " + " + right->toString() + ")";
    }
};

class SubtractionNode : public ASTNode {
    ASTNode* left;
    ASTNode* right;
public:
    SubtractionNode(ASTNode* l, ASTNode* r)
        : left(std::move(l)), right(std::move(r)) {}

    NUMBER_TYPE evaluate(const Env &env) const override {
        return left->evaluate(env) - right->evaluate(env);
    }

    std::string toString() const override {
        return "(" + left->toString() + " - " + right->toString() + ")";
    }
};

class MultiplicationNode : public ASTNode {
    ASTNode* left;
    ASTNode* right;
public:
    MultiplicationNode(ASTNode* l, ASTNode* r)
        : left(std::move(l)), right(std::move(r)) {}

    NUMBER_TYPE evaluate(const Env &env) const override {
        return left->evaluate(env) * right->evaluate(env);
    }

    std::string toString() const override {
        return "(" + left->toString() + " * " + right->toString() + ")";
    }
};

class DivisionNode : public ASTNode {
    ASTNode* left;
    ASTNode* right;
public:
    DivisionNode(ASTNode* l, ASTNode* r)
        : left(std::move(l)), right(std::move(r)) {}

    NUMBER_TYPE evaluate(const Env &env) const override {
        NUMBER_TYPE rVal = right->evaluate(env);
        if (rVal == 0)
            throw std::runtime_error("Division by zero");
        return left->evaluate(env) / rVal;
    }

    std::string toString() const override {
        return "(" + left->toString() + " / " + right->toString() + ")";
    }
};

class PowerNode : public ASTNode {
    ASTNode* base;
    ASTNode* exponent;
public:
    PowerNode(ASTNode* b, ASTNode* e)
        : base(std::move(b)), exponent(std::move(e)) {}

    NUMBER_TYPE evaluate(const Env &env) const override {
        return std::pow(base->evaluate(env), exponent->evaluate(env));
    }

    std::string toString() const override {
        return "(" + base->toString() + " ^ " + exponent->toString() + ")";
    }
};

class FunctionNode : public ASTNode {
    std::string name;
    std::vector<ASTNode*> args;
    const Function* functionRef;
public:
    FunctionNode(const std::string &name, std::vector<ASTNode*> arguments, const Function* func)
        : name(name), args(std::move(arguments)), functionRef(func) {}

    NUMBER_TYPE evaluate(const Env &env) const override {
        std::vector<NUMBER_TYPE> evaluatedArgs;
        for (const auto &arg : args)
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
};
