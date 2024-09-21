#pragma once

#include "Token.h"
#include "ExprNode.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <memory>

struct Function {
    std::vector<std::string> args;
    std::string expression;
};

class Solver {
public:
    Solver();
    
    void declareVariable(const std::string& name, double value, bool isGlobal = false);
    void declareFunction(const std::string& name, const std::vector<std::string>& args, const std::string& expression);
    std::vector<double> evaluateForRange(const std::string& variable, const std::vector<double>& values, const std::string& expression);
    double evaluate(const std::string& expression);
    
private:

    std::vector<Token> tokenize(const std::string& equation);
    std::unique_ptr<ExprNode> parseExpression(std::vector<Token> tokens);

    double evaluateNode(const std::unique_ptr<ExprNode>& node);

    std::unordered_map<std::string, double> globalSymbols;
    std::unordered_map<std::string, double> localSymbols;
    std::unordered_map<std::string, Function> functions;

    std::queue<Token> shuntingYard(const std::vector<Token>& tokens);
    std::unique_ptr<ExprNode> simplify(std::unique_ptr<ExprNode> node);

    double evaluateFunction(const std::string& func, const std::vector<double>& args);
    void validateFunctionExpression(const std::string& expression, const std::vector<std::string>& args);

    const std::unordered_set<std::string> standardFunctions = {"sin", "cos", "tan", "exp", "log", "sqrt"};

    bool isZero(const std::unique_ptr<ExprNode>& node);
    bool isOne(const std::unique_ptr<ExprNode>& node);
    bool areEqual(const std::unique_ptr<ExprNode>& left, const std::unique_ptr<ExprNode>& right);
    std::unique_ptr<ExprNode> makeMulNode(double coefficient, std::unique_ptr<ExprNode> node);
    std::unique_ptr<ExprNode> makeConstNode(double value);
};
