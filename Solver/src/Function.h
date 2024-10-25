// Function.h
#pragma once

#include "pch.h"
#include "ExprNode.h"

using FunctionCallback = std::function<double(const std::vector<double>&)>;

struct Function {
    FunctionCallback callback;             // For predefined functions
    std::vector<std::string> args;         // For user-defined functions
    std::string expression;                // For user-defined functions
    ExprNode* exprTree;                    // Pre-parsed expression tree
    size_t argCount;                       // Number of arguments
    bool isPredefined;                     // Flag to differentiate between predefined and user-defined functions

    // Default Constructor
    Function()
        : callback(), args(), expression(), exprTree(), argCount(0), isPredefined(true) {}

    // Constructor for predefined functions
    Function(FunctionCallback cb, size_t argCnt)
        : callback(cb), args(), expression(), argCount(argCnt), isPredefined(true) {}

    // Constructor for user-defined functions
    Function(std::vector<std::string> a, std::string expr)
        : callback(), args(std::move(a)), expression(std::move(expr)), argCount(args.size()), isPredefined(false) {}
};
