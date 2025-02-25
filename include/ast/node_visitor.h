#pragma once
#include <string>

class NumberNode;
class VariableNode;
class AdditionNode;
class SubtractionNode;
class MultiplicationNode;
class DivisionNode;
class PowerNode;
class FunctionNode;

class NodeVisitor {
public:
    virtual ~NodeVisitor() {}

    virtual void visit(NumberNode* node) = 0;
    virtual void visit(VariableNode* node) = 0;
    virtual void visit(AdditionNode* node) = 0;
    virtual void visit(SubtractionNode* node) = 0;
    virtual void visit(MultiplicationNode* node) = 0;
    virtual void visit(DivisionNode* node) = 0;
    virtual void visit(PowerNode* node) = 0;
    virtual void visit(FunctionNode* node) = 0;
};
