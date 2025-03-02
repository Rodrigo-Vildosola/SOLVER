#pragma once

#include "expression/node.h"

namespace Expression {

class IAllocator {
public:
    virtual ~IAllocator() = default;

    // Virtual functions for each node type.
    virtual Node* createNumber(double value) = 0;
    virtual Node* createVariable(const std::string &name) = 0;
    virtual Node* createAddition(Node* left, Node* right) = 0;
    virtual Node* createSubtraction(Node* left, Node* right) = 0;
    virtual Node* createMultiplication(Node* left, Node* right) = 0;
    virtual Node* createDivision(Node* left, Node* right) = 0;
    virtual Node* createExponentiation(Node* base, Node* exponent) = 0;
    virtual Node* createSin(Node* operand) = 0;
    virtual Node* createCos(Node* operand) = 0;
    virtual Node* createTan(Node* operand) = 0;
    virtual Node* createLn(Node* operand) = 0;
    virtual Node* createLog(Node* base, Node* operand) = 0;
    virtual Node* createEquality(Node* left, Node* right) = 0;
    virtual Node* createFunction(const std::string &name, int expectedArgCount,
                                 const std::vector<Node*>& args,
                                 FunctionCallback callback) = 0;
};

} // namespace Expression
