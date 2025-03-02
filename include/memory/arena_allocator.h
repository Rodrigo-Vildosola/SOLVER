#pragma once

#include "iallocator.h"
#include "arena.h"
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

class ArenaAllocator : public IAllocator {
private:
    Arena &arena;
public:
    explicit ArenaAllocator(Arena &a) : arena(a) {}

    virtual Node* createNumber(double value) override {
        return arena.make<NumberNode>(value);
    }
    virtual Node* createVariable(const std::string &name) override {
        return arena.make<VariableNode>(name);
    }
    virtual Node* createAddition(Node* left, Node* right) override {
        return arena.make<AdditionNode>(left, right);
    }
    virtual Node* createSubtraction(Node* left, Node* right) override {
        return arena.make<SubtractionNode>(left, right);
    }
    virtual Node* createMultiplication(Node* left, Node* right) override {
        return arena.make<MultiplicationNode>(left, right);
    }
    virtual Node* createDivision(Node* left, Node* right) override {
        return arena.make<DivisionNode>(left, right);
    }
    virtual Node* createExponentiation(Node* base, Node* exponent) override {
        return arena.make<ExponentiationNode>(base, exponent);
    }
    virtual Node* createSin(Node* operand) override {
        return arena.make<SinNode>(operand);
    }
    virtual Node* createCos(Node* operand) override {
        return arena.make<CosNode>(operand);
    }
    virtual Node* createTan(Node* operand) override {
        return arena.make<TanNode>(operand);
    }
    virtual Node* createLn(Node* operand) override {
        return arena.make<LnNode>(operand);
    }
    virtual Node* createLog(Node* base, Node* operand) override {
        return arena.make<LogNode>(base, operand);
    }
    virtual Node* createEquality(Node* left, Node* right) override {
        return arena.make<EqualityNode>(left, right);
    }
    virtual Node* createFunction(const std::string &name, int expectedArgCount,
                                 const std::vector<Node*>& args,
                                 FunctionCallback callback) override {
        return arena.make<FunctionNode>(name, expectedArgCount, args, callback);
    }
};

} // namespace Expression
