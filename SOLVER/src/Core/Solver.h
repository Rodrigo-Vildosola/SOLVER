#pragma once

#include "Token.h"
#include "ExprNode.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
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
    std::unique_ptr<ExprNode> parseExpression(std::vector<Token> tokens);
    double evaluateNode(const std::unique_ptr<ExprNode>& node);

    // Separate maps for constants and variables
    std::unordered_map<std::string, double> constants;
    std::unordered_map<std::string, double> variables;

    std::unordered_map<std::string, Function> functions;
    std::map<std::string, FunctionCallback> predefinedFunctions;

    void registerBuiltInFunctions();
    std::queue<Token> shuntingYard(const std::vector<Token>& tokens);
    std::unique_ptr<ExprNode> simplify(std::unique_ptr<ExprNode> node);
    double evaluateFunction(const std::string& func, const std::vector<double>& args);
    void validateFunctionExpression(const std::string& expression, const std::vector<std::string>& args);

    bool isZero(const std::unique_ptr<ExprNode>& node);
    bool isOne(const std::unique_ptr<ExprNode>& node);
    bool areEqual(const std::unique_ptr<ExprNode>& left, const std::unique_ptr<ExprNode>& right);
    std::unique_ptr<ExprNode> makeMulNode(double coefficient, std::unique_ptr<ExprNode> node);
    std::unique_ptr<ExprNode> makeConstNode(double value);

    void printTokens(const std::vector<Token>& tokens) const;
    void printTree(const ExprNode* node, int depth = 0, const std::string& prefix = "", bool isLeft = true) const;
};
