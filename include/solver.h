#pragma once

#include "pch.h"
#include "token.h"
#include "expr_node.h"
#include "symbol_table.h"
#include "function.h"
#include "LRU_cache.h"


/**
 * @class Solver
 * @brief A class for evaluating mathematical expressions, managing variables, constants, and user-defined functions.
 * 
 * The Solver class provides methods for declaring constants, variables, user-defined functions, and evaluating
 * mathematical expressions. It supports both predefined and custom functions. The class also supports memoization
 * for caching function and expression results, with an option to disable caching if necessary.
 */
class Solver {
public:
    /**
     * @brief Constructor for the Solver class.
     * 
     * Initializes the Solver instance and registers built-in functions.
     */
    Solver(size_t exprCacheSize = 100, size_t funcCacheSize = 100);

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
     * This method parses and simplifies the expression (if not cached), evaluates the expression, and returns the result.
     * 
     * @param expression The mathematical expression to evaluate.
     * @param debug If true, enables debug output during evaluation.
     * @return The result of evaluating the expression.
     */
    double evaluate(const std::string& expression, bool debug = false);

    /**
     * @brief Evaluate a mathematical expression over a range of values for a given variable.
     * 
     * The method parses and simplifies the expression once, then evaluates the expression tree for each value in the range.
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
     * Registers a function in the solver's function table using a predefined callback for execution.
     * 
     * @param name The name of the function.
     * @param callback A function callback to execute when the function is called.
     * @param argCount The number of arguments the function expects.
     */
    void registerPredefinedFunction(const std::string& name, const FunctionCallback& callback, size_t argCount);

    /**
     * @brief Declare a user-defined function.
     * 
     * Declares a function in the solver's function table, allowing the user to define functions with expressions and variables.
     * 
     * @param name The name of the function.
     * @param args A vector of argument names for the function.
     * @param expression The mathematical expression defining the function.
     */
    void declareFunction(const std::string& name, const std::vector<std::string>& args, const std::string& expression);

    /**
     * @brief Clears the cache when variables or constants change.
     * 
     * This clears both the function and expression caches to ensure correct recalculation of results.
     */
    void clearCache();

    /**
     * @brief Set cache usage flag.
     * 
     * @param useCache If true, enables caching of results. If false, disables caching.
     */
    void setUseCache(bool useCache);

    /**
     * @brief Lists all declared constants.
     * 
     * @return A map of constant names and their values.
     */
    std::unordered_map<std::string, double> listConstants() const;

    /**
     * @brief Lists all declared variables.
     * 
     * @return A map of variable names and their values.
     */
    std::unordered_map<std::string, double> listVariables() const;

    /**
     * @brief Lists all declared functions.
     * 
     * @return A map of function names, along with argument lists and a flag indicating whether the function is predefined.
     */
    std::unordered_map<std::string, std::pair<std::vector<std::string>, bool>> listFunctions() const;

    /**
     * @brief Sets the current expression and parses it into an expression tree.
     * 
     * This method stores the expression and its parsed tree, so future evaluations of the same expression can use the cached tree.
     * 
     * @param expression The mathematical expression to set as the current expression.
     * @param debug If true, enables debug output during parsing and simplification.
     */
    void setCurrentExpression(const std::string &expression, bool debug);

    /**
     * @brief Get the current expression being evaluated.
     * 
     * @return The current expression being evaluated.
     */
    std::string getCurrentExpression() const { return currentExpression; }

private:
    /**
     * @brief Register built-in mathematical functions like sin, cos, etc.
     * 
     * This method is called during initialization to register standard mathematical functions.
     */
    void registerBuiltInFunctions();

    /**
     * @brief Evaluate an expression tree node.
     * 
     * This method recursively evaluates an expression tree node and returns the result.
     * 
     * @param node A unique pointer to an expression tree node.
     * @return The result of evaluating the node.
     */
    double evaluateNode(ExprNode* node);

    /**
     * @brief Parses and simplifies a mathematical expression.
     * 
     * @param expression The mathematical expression to parse.
     * @param debug If true, enables debug output during parsing and simplification.
     * @return A unique pointer to the simplified expression tree (ExprNode).
     */
    ExprNode* parse(const std::string &expression, bool debug = false);

    /**
     * @brief Evaluate a function with the provided arguments.
     * 
     * This method handles the evaluation of user-defined and predefined functions with the provided arguments.
     * 
     * @param func The name of the function.
     * @param args A vector of arguments to pass to the function.
     * @return The result of the function evaluation.
     */
    double evaluateFunction(const std::string& func, const std::vector<double>& args);

    /**
     * @brief Generates a unique cache key for a given expression or function call.
     * 
     * @param base The base string for the cache key (typically the expression or function name).
     * @param args The arguments used in the expression or function.
     * @return A unique string to use as the cache key.
     */
    std::size_t generateCacheKey(const std::string& base, const std::vector<double>& args);

    /**
     * @brief Invalidate cached results.
     * 
     * This function is called when the solver's state changes, such as when variables or constants are declared.
     */
    void invalidateCaches();

    /**
     * @brief Validate that a function's dependencies (arguments) are valid.
     * 
     * @param expression The function expression.
     * @param args The arguments that the function depends on.
     */
    void validateFunctionDependencies(const std::string& expression, const std::vector<std::string>& args);


    // Predefined functions and user-defined functions
    std::unordered_map<std::string, Function> functions;

    LRUCache<std::size_t, double> expressionCache;  ///< LRU Cache for expression evaluation results
    LRUCache<std::size_t, double> functionCache;    ///< LRU Cache for function evaluation results

    bool cacheEnabled = true;  ///< Flag to enable or disable cache

    // SymbolTable for managing variables and constants
    SymbolTable symbolTable;

    std::string currentExpression; ///< The current expression being evaluated

    ExprNode* currentExprTree;

};
