#pragma once

#include "pch.h"
#include "Token.h"
#include "ExprNode.h"
#include "SymbolTable.h"
#include "Function.h"

/**
 * @class Solver
 * @brief A class for evaluating mathematical expressions, managing variables, constants, and user-defined functions.
 * 
 * The Solver class provides methods for declaring constants, variables, user-defined functions, and evaluating
 * mathematical expressions. It supports both predefined and custom functions.
 */
class Solver {
public:
    /**
     * @brief Constructor for the Solver class.
     * 
     * Initializes the Solver instance and registers built-in functions.
     */
    Solver();
    
    /**
     * @brief Declare a constant in the solver's symbol table.
     * 
     * @param name The name of the constant.
     * @param value The value of the constant.
     */
    void declareConstant(const std::string& name, double value);
    
    /**
     * @brief Declare a variable in the solver's symbol table.
     * 
     * @param name The name of the variable.
     * @param value The initial value of the variable.
     */
    void declareVariable(const std::string& name, double value);
    
    /**
     * @brief Evaluate a mathematical expression.
     * 
     * @param expression The mathematical expression to evaluate.
     * @param debug If true, enables debug output during evaluation.
     * @return The result of evaluating the expression.
     */
    double evaluate(const std::string& expression, bool debug = false);
    
    /**
     * @brief Evaluate a mathematical expression over a range of values for a given variable.
     * 
     * @param variable The name of the variable to iterate over.
     * @param values A vector of values for the variable.
     * @param expression The mathematical expression to evaluate.
     * @param debug If true, enables debug output during evaluation.
     * @return A vector of results for each value in the range.
     */
    std::vector<double> evaluateForRange(const std::string& variable, const std::vector<double>& values, const std::string& expression, bool debug = false);

    /**
     * @brief Register a predefined function with a callback.
     * 
     * @param name The name of the function.
     * @param callback A function callback to execute when the function is called.
     * @param argCount The number of arguments the function expects.
     */
    void registerPredefinedFunction(const std::string& name, const FunctionCallback& callback, size_t argCount);
    
    /**
     * @brief Declare a user-defined function.
     * 
     * @param name The name of the function.
     * @param args A vector of argument names for the function.
     * @param expression The mathematical expression defining the function.
     */
    void declareFunction(const std::string& name, const std::vector<std::string>& args, const std::string& expression);

private:
    /**
     * @brief Register built-in mathematical functions like sin, cos, etc.
     */
    void registerBuiltInFunctions();

    /**
     * @brief Evaluate an expression tree node.
     * 
     * @param node A unique pointer to an expression tree node.
     * @return The result of evaluating the node.
     */
    double evaluateNode(const std::unique_ptr<ExprNode>& node);
    
    /**
     * @brief Tokenize a mathematical equation into tokens.
     * 
     * @param equation The equation to tokenize.
     * @return A vector of tokens extracted from the equation.
     */
    std::vector<Token> tokenize(const std::string& equation);

    /**
     * @brief Evaluate a function with the provided arguments.
     * 
     * @param func The name of the function.
     * @param args A vector of arguments to pass to the function.
     * @return The result of the function evaluation.
     */
    double evaluateFunction(const std::string& func, const std::vector<double>& args);

    /**
     * @brief Validate the syntax of the given expression.
     * 
     * @param expression The expression to validate.
     */
    void isValidSyntax(const std::string& expression);
    
    /**
     * @brief Validate that a function's dependencies (arguments) are valid.
     * 
     * @param expression The function expression.
     * @param args The arguments that the function depends on.
     */
    void validateFunctionDependencies(const std::string& expression, const std::vector<std::string>& args);
    
    /**
     * @brief Print the expression tree for debugging purposes.
     * 
     * @param node The current node of the tree.
     * @param depth The depth of the node in the tree.
     * @param prefix The string prefix for the node (used for tree formatting).
     * @param isLeft Whether the current node is a left child.
     */
    void printTree(const ExprNode* node, int depth = 0, const std::string& prefix = "", bool isLeft = true) const;

    // Predefined functions and user-defined functions
    std::unordered_map<std::string, Function> functions;

    // SymbolTable for managing variables and constants
    SymbolTable symbolTable;
};
