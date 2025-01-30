// Function.h
#pragma once

#include "pch.h"
#include "token.h"

using FunctionCallback = std::function<long double(const std::vector<long double>&)>;

struct Function {
    FunctionCallback callback;              // For predefined functions
    std::vector<Token> inlinedPostfix;      // Postfix expression for user-defined functions
    std::vector<std::string> argumentNames; // Names of the arguments
    size_t argCount;                        // Number of arguments
    bool isPredefined;                      // Flag for predefined functions

    // Default Constructor
    Function()
        : callback(nullptr), inlinedPostfix(), argumentNames(), argCount(0), isPredefined(true) {}

    // Constructor for predefined functions
    Function(FunctionCallback cb, size_t argCnt)
        : callback(std::move(cb)), inlinedPostfix(), argumentNames(), argCount(argCnt), isPredefined(true) {}

    // Constructor for user-defined functions
    Function(std::vector<Token> postfix, std::vector<std::string> args)
        : callback(nullptr), inlinedPostfix(std::move(postfix)), argumentNames(std::move(args)), argCount(argumentNames.size()), isPredefined(false) {}
};
