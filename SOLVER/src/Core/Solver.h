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
    double evaluate(const std::string& expression);
    
    // New method to solve linear equations
    double solveLinearEquation(const std::string& equation);

private:

    std::vector<Token> tokenize(const std::string& equation);
    std::unique_ptr<ExprNode> parseExpression(std::vector<Token> tokens);

    double evaluateNode(const std::unique_ptr<ExprNode>& node);
    double evaluateFunction(const std::string& func, const std::vector<double>& args);
    void validateFunctionExpression(const std::string& expression, const std::vector<std::string>& args);

    std::unordered_map<std::string, double> globalSymbols;
    std::unordered_map<std::string, double> localSymbols;
    std::unordered_map<std::string, Function> functions;

    std::queue<Token> shuntingYard(const std::vector<Token>& tokens);

    // New helper method to extract linear terms
    void extractLinearTerms(const std::string& expression, double& coefficient, double& constant);

    const std::unordered_set<std::string> standardFunctions = {"sin", "cos", "tan", "exp", "log", "sqrt"};
};
