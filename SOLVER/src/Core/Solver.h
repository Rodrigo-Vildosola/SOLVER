#pragma once

#include "Token.h"
#include "ExprNode.h"
#include "ExprTree.h"
#include "SymbolTable.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

struct Function {
    std::vector<std::string> args;
    std::string expression;
};

using FunctionCallback = std::function<double(const std::vector<double>&)>;

class Solver {
public:
    Solver();
    
    // Methods to declare constants and variables
    void declareConstant(const std::string& name, double value);
    void declareVariable(const std::string& name, double value);
    void declareFunction(const std::string& name, const std::vector<std::string>& args, const std::string& expression);
    
    double evaluate(const std::string& expression, bool debug = false);
    std::vector<double> evaluateForRange(const std::string& variable, const std::vector<double>& values, const std::string& expression, bool debug = false);

    void registerPredefinedFunction(const std::string& name, const FunctionCallback& callback);

private:
    std::vector<Token> tokenize(const std::string& equation);
    double evaluateNode(const std::unique_ptr<ExprNode>& node);

    // Predefined functions and user-defined functions
    std::unordered_map<std::string, Function> functions;
    std::map<std::string, FunctionCallback> predefinedFunctions;

    // SymbolTable for managing variables and constants
    SymbolTable symbolTable;

    // Expression tree for parsing and simplifying expressions
    ExpressionTree expressionTree;

    void registerBuiltInFunctions();
    double evaluateFunction(const std::string& func, const std::vector<double>& args);
    void validateFunctionExpression(const std::string& expression, const std::vector<std::string>& args);
    
    void printTree(const ExprNode* node, int depth = 0, const std::string& prefix = "", bool isLeft = true) const;
};
