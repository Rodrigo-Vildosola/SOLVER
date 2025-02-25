// LoggingVisitor.h
#pragma once

#include "node_visitor.h"
#include "node.h"
#include <vector>
#include <string>

class LoggingVisitor : public NodeVisitor {
public:
    std::vector<std::string> steps;

    void visit(NumberNode* node) override {
        // For example, log no transformation on constants.
        steps.push_back("Visited Number: " + node->toString());
    }
    void visit(VariableNode* node) override {
        steps.push_back("Visited Variable: " + node->toString());
    }
    void visit(AdditionNode* node) override {
        steps.push_back("Visited Addition: " + node->toString());
    }
    void visit(SubtractionNode* node) override {
        steps.push_back("Visited Subtraction: " + node->toString());
    }
    void visit(MultiplicationNode* node) override {
        steps.push_back("Visited Multiplication: " + node->toString());
    }
    void visit(DivisionNode* node) override {
        steps.push_back("Visited Division: " + node->toString());
    }
    void visit(PowerNode* node) override {
        steps.push_back("Visited Power: " + node->toString());
    }
    void visit(FunctionNode* node) override {
        steps.push_back("Visited Function: " + node->toString());
    }
};
