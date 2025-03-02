#pragma once

#include "memory/iallocator.h"
#include "memory/arena_allocator.h"
#include "memory/default_allocator.h"
#include "memory/allocation_config.h"

namespace Expression {

class NodeFactory {
private:
    IAllocator* allocator;

public:
    // Constructor without caching logic
    NodeFactory(IAllocator* alloc, AllocatorPolicy policy)
        : allocator(alloc) {
        AllocationConfig::setPolicy(policy);
    }

    // Number & Variable Nodes (always create new instances)
    Node* num(double value) {
        return allocator->createNumber(value);
    }

    Node* var(const std::string &name) {
        return allocator->createVariable(name);
    }

    // Binary Operations (always create new instances)
    Node* add(Node* left, Node* right) {
        return allocator->createAddition(left, right);
    }

    Node* sub(Node* left, Node* right) {
        return allocator->createSubtraction(left, right);
    }

    Node* mul(Node* left, Node* right) {
        return allocator->createMultiplication(left, right);
    }

    Node* div(Node* left, Node* right) {
        return allocator->createDivision(left, right);
    }

    Node* exp(Node* base, Node* exponent) {
        return allocator->createExponentiation(base, exponent);
    }

    // Unary Operations
    Node* sin(Node* operand) {
        return allocator->createSin(operand);
    }

    Node* cos(Node* operand) {
        return allocator->createCos(operand);
    }

    Node* tan(Node* operand) {
        return allocator->createTan(operand);
    }

    Node* ln(Node* operand) {
        return allocator->createLn(operand);
    }

    Node* log(Node* base, Node* operand) {
        return allocator->createLog(base, operand);
    }

    // Equality & Function Nodes
    Node* eq(Node* left, Node* right) {
        return allocator->createEquality(left, right);
    }

    Node* func(const std::string &name, int expectedArgCount, const std::vector<Node*>& args, FunctionCallback callback) {
        return allocator->createFunction(name, expectedArgCount, args, callback);
    }
};

} // namespace Expression
