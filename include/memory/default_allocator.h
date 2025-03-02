#pragma once

#include "iallocator.h"
#include "expression/number_node.h"
#include "expression/addition_node.h"
#include "expression/subtraction_node.h"
#include "expression/multiplication_node.h"
#include "expression/division_node.h"
#include "expression/exponentiation_node.h"
#include "expression/sin_node.h"
#include "expression/cos_node.h"
#include "expression/ln_node.h"
#include "expression/log_node.h"
#include "expression/variable_node.h"
#include "expression/equality_node.h"
#include "expression/function_node.h"
#include "expression/tan_node.h"

namespace Expression {

class DefaultAllocator : public IAllocator {
public:
    virtual ~DefaultAllocator() = default;

    virtual Node* createNumber(double value) override {
        return new NumberNode(value);
    }
    virtual Node* createVariable(const std::string &name) override {
        return new VariableNode(name);
    }
    virtual Node* createAddition(Node* left, Node* right) override {
        return new AdditionNode(left, right);
    }
    virtual Node* createSubtraction(Node* left, Node* right) override {
        return new SubtractionNode(left, right);
    }
    virtual Node* createMultiplication(Node* left, Node* right) override {
        return new MultiplicationNode(left, right);
    }
    virtual Node* createDivision(Node* left, Node* right) override {
        return new DivisionNode(left, right);
    }
    virtual Node* createExponentiation(Node* base, Node* exponent) override {
        return new ExponentiationNode(base, exponent);
    }
    virtual Node* createSin(Node* operand) override {
        return new SinNode(operand);
    }
    virtual Node* createCos(Node* operand) override {
        return new CosNode(operand);
    }
    virtual Node* createTan(Node* operand) override {
        return new TanNode(operand);
    }
    virtual Node* createLn(Node* operand) override {
        return new LnNode(operand);
    }
    virtual Node* createLog(Node* base, Node* operand) override {
        return new LogNode(base, operand);
    }
    virtual Node* createEquality(Node* left, Node* right) override {
        return new EqualityNode(left, right);
    }
    virtual Node* createFunction(const std::string &name, int expectedArgCount,
                                 const std::vector<Node*>& args,
                                 FunctionCallback callback) override {
        return new FunctionNode(name, expectedArgCount, args, callback);
    }
};

} // namespace Expression
