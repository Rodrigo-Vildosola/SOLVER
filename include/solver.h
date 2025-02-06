#pragma once

#include "pch.h"
#include "token.h"
#include "symbol_table.h"
#include "function.h"
#include "LRU_cache.h"
#include "simplification.h"

/**
 * @class Solver
 * @brief A class for evaluating mathematical expressions, managing variables, constants, and user-defined functions.
 * 
 * The Solver class provides methods for declaring constants, variables, user-defined functions, and evaluating
 * mathematical expressions. It supports both predefined (built-in) and custom functions. It can also use memoization
 * to cache expression results, with an option to enable or disable caching on demand.
 */
class Solver {
public:
    /**
     * @brief Constructs a Solver instance and registers built-in functions.
     * 
     * By default, initializes an LRU cache for expression results of size \p exprCacheSize.
     * Also starts the profiling session (if enabled) and calls registerBuiltInFunctions() to load
     * functions such as sin, cos, etc.
     * 
     * @param exprCacheSize The maximum number of entries the expression cache can hold (default 100).
     */
    Solver(size_t exprCacheSize = 100);

    /**
     * @brief Destructor for the Solver class.
     * 
     * Ends the profiling session (if enabled). Cleans up any associated resources.
     */
    ~Solver() {
        PROFILE_END_SESSION();
        if (currentAST) {
            delete currentAST;
            currentAST = nullptr;
        }
    }

    /**
     * @brief Prints expressions (postfix or inlined) for all registered functions to stdout.
     * 
     * This can be useful for debugging or understanding how user-defined functions have been
     * internally flattened to postfix representation.
     */
    void printFunctionExpressions();

    /**
     * @brief Declares a constant in the symbol table.
     * 
     * This inserts a new constant \p name with value \p value into the symbol table, replacing
     * any existing entry with the same name. Once declared, the constant cannot be altered by
     * normal assignment. Any existing caches are invalidated to reflect the new symbol.
     * 
     * @param name The name of the constant (e.g., "pi").
     * @param value The numeric value of the constant (e.g., 3.14159).
     */
    void declareConstant(const std::string& name, NUMBER_TYPE value);

    /**
     * @brief Declares (or re-declares) a variable in the symbol table.
     * 
     * This inserts a new variable \p name with initial value \p value into the symbol table.
     * If the variable already exists, its value is updated. Declaring a variable also invalidates
     * any cached expression results to ensure correctness if that variable is used in expressions.
     * 
     * @param name The name of the variable (e.g., "x").
     * @param value The numeric value to assign to the variable.
     */
    void declareVariable(const std::string& name, NUMBER_TYPE value);

    /**
     * @brief Evaluates a mathematical expression and returns its numeric result.
     * 
     * - Internally, this calls setCurrentExpression() which parses the expression (or uses a cached parse if unchanged).
     * - Then it checks the cache (if enabled) to see if the expression result is already stored.
     * - If not in cache, it evaluates the expression in postfix form and stores the result if caching is on.
     * 
     * @param expression A string representing the mathematical expression to evaluate (e.g. "3 + 4 * 2").
     * @param debug If true, prints debugging information such as the final postfix representation.
     * @return The computed value of the expression.
     * @throws SolverException If there is a parsing error, missing function, or other runtime error.
     */
    NUMBER_TYPE evaluate(const std::string& expression, bool debug = false);

    /**
     * @brief Evaluates a mathematical expression for each value in a range of inputs for one variable.
     * 
     * This method is similar to evaluate(), but it iterates over the provided vector \p values for the
     * specified variable \p variable. For each value, the variable is set in the symbol table, and the
     * same parsed/postfix expression is evaluated. This allows efficient bulk evaluation.
     * 
     * @param variable The name of the variable to iterate (e.g. "x").
     * @param values A vector of values to assign to that variable sequentially.
     * @param expression The mathematical expression to evaluate (e.g. "x^2 + 1").
     * @param debug If true, prints debug info for the parsing phase (once) and indicates evaluations in a loop.
     * @return A vector of computed results, with one result per value in \p values.
     * @throws SolverException If \p variable is invalid or if an error occurs during evaluation.
     */
    std::vector<NUMBER_TYPE> evaluateForRange(const std::string& variable,
                                         const std::vector<NUMBER_TYPE>& values,
                                         const std::string& expression,
                                         bool debug = false);


    /**
     * @brief Evaluates a single expression across multiple variables, each with a range of values.
     *
     * This method computes the cartesian product of all provided value ranges. For each combination,
     * it sets the corresponding variable values in the symbol table, evaluates the (already-parsed)
     * postfix expression, and appends the result to the output vector.
     *
     * The result vector is returned in row-major order with respect to the input vectors in
     * \p valuesSets, i.e., the last variable varies fastest.
     *
     * @param variables A list of variable names, e.g., ["x", "y"].
     * @param valuesSets A corresponding list of ranges for each variable, e.g., [[0, 1, 2], [10, 20]].
     * @param expression The expression to evaluate once for every combination (cartesian product).
     * @param debug If true, prints debug info for parsing; does NOT print each evaluation.
     * @return A flat vector of results (size = product of the lengths of each range in \p valuesSets).
     * @throws SolverException If a variable name is invalid, or expression parsing fails, etc.
     */
    std::vector<NUMBER_TYPE> evaluateForRanges(const std::vector<std::string>& variables,
                                          const std::vector<std::vector<NUMBER_TYPE>>& valuesSets,
                                          const std::string& expression,
                                          bool debug);


    /**
     * @brief Computes the derivative of an expression with respect to a variable.
     *
     * @param expression The function to differentiate.
     * @param variable The variable with respect to which differentiation is performed.
     * @param atValue The point at which the derivative is evaluated.
     * @param method The numerical differentiation method (e.g., "central", "forward", "backward").
     * @param epsilon The small delta for numerical differentiation.
     * @return The computed derivative value.
     */
    NUMBER_TYPE evaluateDerivative(const std::string& expression, 
                                   const std::string& variable, 
                                   NUMBER_TYPE atValue, 
                                   const std::string& method = "central", 
                                   NUMBER_TYPE epsilon = 1e-5);


    /**
     * @brief Computes the definite integral of an expression over a range using numerical methods.
     *
     * @param expression The function to integrate.
     * @param variable The variable of integration.
     * @param lower The lower bound of integration.
     * @param upper The upper bound of integration.
     * @param steps Number of steps to divide the interval (higher = more precision).
     * @return The computed integral value.
     */
    NUMBER_TYPE evaluateIntegral(const std::string& expression, 
                                const std::string& variable, 
                                NUMBER_TYPE lower, 
                                NUMBER_TYPE upper, 
                                size_t steps = 1000);

    /**
     * @brief Registers a predefined function with a C++ callback.
     * 
     * This allows you to bind any C++ function or lambda that matches the FunctionCallback signature
     * as a recognized function in the solver. When the function is invoked in an expression, the
     * callback is called with the appropriate number of arguments.
     * 
     * @param name The function name (e.g. "sin").
     * @param callback A C++ std::function or lambda matching the FunctionCallback signature (returns double, takes a vector of doubles).
     * @param argCount The number of arguments the function requires (e.g. 1 for sin).
     * @throws SolverException If a function with the same name already exists.
     */
    void registerPredefinedFunction(const std::string& name, const FunctionCallback& callback, size_t argCount);

    /**
     * @brief Declares a user-defined function in terms of an expression and parameter list.
     * 
     * Internally, this parses the expression to a flattened postfix form and stores it, along
     * with the argument names. When invoked in other expressions, the function is inlined
     * (substituted for its body) or evaluated as needed.
     * 
     * @param name The function name (e.g. "f").
     * @param args A list of parameter names (e.g. ["x", "y"]).
     * @param expression The expression defining the function body (e.g. "x^2 + y^2").
     * @throws SolverException If the function name is invalid, the syntax is incorrect,
     *         or a function by the same name already exists.
     */
    void declareFunction(const std::string& name, const std::vector<std::string>& args, const std::string& expression);

    /**
     * @brief Clears the solver's expression cache and function cache (if any).
     * 
     * This is a direct way to force the solver to discard all cached results. After calling,
     * the next evaluations will re-parse and re-compute the expression outcomes from scratch.
     */
    void clearCache();

    /**
     * @brief Toggles whether the solver uses its LRU cache.
     * 
     * When disabled (\p useCache = false), all subsequent evaluations will parse and compute
     * results fresh each time. Enabling again restarts caching but does not retroactively
     * recover old cached values.
     * 
     * @param useCache Pass true to enable expression caching, false to disable it.
     */
    void setUseCache(bool useCache);

    /**
     * @brief Lists all declared constants.
     * 
     * Provides a snapshot of the symbol table's constant entries, mapping constant names
     * to their numeric values.
     * 
     * @return An unordered_map from constant name to double value.
     */
    Env listConstants() const { return symbolTable.getConstants(); }

    /**
     * @brief Lists all declared variables.
     * 
     * Provides a snapshot of the symbol table's variable entries, mapping variable names
     * to their current numeric values.
     * 
     * @return An unordered_map from variable name to double value.
     */
    Env listVariables() const { return symbolTable.getVariables(); }

    /**
     * @brief Sets the expression to be evaluated and parses it into a postfix representation.
     * 
     * If the given expression is different from the current one (or if there's no valid
     * current postfix), it tokenizes and converts the expression to postfix, storing both.
     * If the same expression was already set and parsed, it reuses the stored postfix
     * unless \p debug is true (in which case debug info may be printed).
     * 
     * @param expression The new expression to parse and set as current.
     * @param debug If true, prints out the final postfix and related debug info.
     */
    void setCurrentExpression(const std::string &expression, bool debug);

    /**
     * @brief Sets the current expression for AST-based evaluation (builds or re-builds an AST).
     *
     * This is similar to setCurrentExpression() but constructs an AST pipeline
     * instead of storing postfix tokens. If the expression is identical to the
     * previously stored one (and the AST is valid), we skip re-building unless debug is true.
     */
    void setCurrentExpressionAST(const std::string &expression, bool debug = false);

    /**
     * @brief Evaluate an expression using the AST pipeline.
     * 
     * This either reuses an existing AST if `expression` is the same as last time,
     * or builds (and optionally simplifies) a new AST. Then calls AST::evaluateAST() 
     * or a similar function to get the numeric result.
     * 
     * @param expression The mathematical expression string.
     * @param debug Whether to print debugging info.
     * @return The numeric evaluation result.
     * @throws SolverException on parse errors, unknown symbols, etc.
     */
    NUMBER_TYPE evaluateAST(const std::string &expression, bool debug = false);

    /**
     * @brief Retrieves the most recently set expression string.
     * 
     * This is simply the raw expression as last passed to setCurrentExpression().
     * 
     * @return The current expression string.
     */
    std::string getCurrentExpression() const { return currentExpression; }

    /**
     * @brief Efficiently generates animation data for visualizing a function.
     *
     * This optimized version evaluates the function in bulk rather than iterating over individual points.
     *
     * @param expression The function to evaluate.
     * @param variable The variable to sweep across.
     * @param start The starting value of the variable.
     * @param end The ending value of the variable.
     * @param steps The number of steps (frames) to generate.
     * @return A tuple (x_values, y_values), where x_values are input values and y_values are computed results.
     */
    std::tuple<NumberList, NumberList> generateAnimationData(
        const std::string &expression,
        const std::string &variable,
        NUMBER_TYPE start,
        NUMBER_TYPE end,
        size_t steps);


    /**
     * @brief Efficiently generates contour data for 3D surface plots.
     *
     * This function uses batch evaluation to compute values over a grid of (x, y) coordinates.
     *
     * @param expression The function to evaluate.
     * @param variable1 The first variable (x-axis).
     * @param variable2 The second variable (y-axis).
     * @param start1 The start value for variable1.
     * @param end1 The end value for variable1.
     * @param steps1 The number of steps for variable1.
     * @param start2 The start value for variable2.
     * @param end2 The end value for variable2.
     * @param steps2 The number of steps for variable2.
     * @return A tuple (x_values, y_values, z_values), where z_values is a 2D matrix of results.
     */
    std::tuple<NumberList, NumberList, NumberMatrix> generateContourData(
        const std::string &expression,
        const std::string &variable1,
        const std::string &variable2,
        NUMBER_TYPE start1,
        NUMBER_TYPE end1,
        size_t steps1,
        NUMBER_TYPE start2,
        NUMBER_TYPE end2,
        size_t steps2);



private:
    /**
     * @brief Registers built-in mathematical functions such as sin, cos, exp, etc.
     * 
     * This method is called in the constructor. It adds standard functions into
     * the solver's function table with predefined callbacks.
     */
    void registerBuiltInFunctions();

    /**
     * @brief Compiles an expression string into an EvalFunc.
     *
     * This function tokenizes the input, converts it to postfix notation,
     * flattens and inlines user-defined functions, replaces constant symbols,
     * simplifies the expression, and finally compiles it into an EvalFunc.
     *
     * @param expression The mathematical expression to compile.
     * @param debug If true, prints debug information.
     * @return The compiled EvalFunc.
     */
    EvalFunc compileExpression(const std::string &expression, bool debug = false);

    /**
     * @brief Parses a mathematical expression from string to postfix.
     * 
     * The process includes tokenizing, converting tokens to postfix notation,
     * and flattening (inlining any user-defined functions). If \p debug is set,
     * it may print intermediate steps.
     * 
     * @param expression The input mathematical expression (in infix).
     * @param debug If true, prints debug information about the tokenization/postfix steps.
     * @return A vector of Tokens representing the flattened postfix form.
     * @throws SolverException If a syntax error or unknown function is encountered.
     */
    ASTNode* parseAST(const std::string &expression, bool debug = false);

    /**
     * @brief Generates an integer cache key based on an expression string and argument values.
     * 
     * This function is used internally to create a unique hash for caching the result
     * of a specific expression and argument set, preventing collisions between different calls.
     * 
     * @param base Typically the expression string or function signature.
     * @param args A list of argument values used when evaluating that expression.
     * @return A std::size_t representing a unique hash key.
     */
    std::size_t generateCacheKey(const std::string& base, const std::vector<NUMBER_TYPE>& args);

    /**
     * @brief Invalidates solver caches if caching is enabled.
     * 
     * Primarily, this clears the expressionCache when any symbolic changes occur
     * (e.g., new variables or constants). Ensures results do not become stale.
     */
    void invalidateCaches();

    // -------------------------------------------------------------------------
    // Member Variables

    /// A map storing both built-in (predefined) and user-defined functions by name.
    std::unordered_map<std::string, Function> functions;

    /// An LRU cache for storing evaluated expression results by hashed keys.
    LRUCache<std::size_t, NUMBER_TYPE> expressionCache;

    /// Flag indicating whether expression caching is currently active.
    bool cacheEnabled = true;

    /// Symbol table for all declared variables and constants (manages their values).
    SymbolTable symbolTable;

    /// The most recent expression string passed to setCurrentExpression().
    std::string currentExpression;

    /// The most recent expression callback created by parse().
    EvalFunc currentCompiledExpression;

    /// The most recent expression string passed to setCurrentExpression().
    std::string currentExpressionAST;

    /// The parsed (and flattened) AST tokens corresponding to currentExpression.
    ASTNode* currentAST;
};
