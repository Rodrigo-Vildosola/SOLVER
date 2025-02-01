/*
  This file contains docstrings for use in the Python bindings.
  Do not edit! They were automatically extracted by pybind11_mkdoc.
 */

#define __EXPAND(x)                                      x
#define __COUNT(_1, _2, _3, _4, _5, _6, _7, COUNT, ...)  COUNT
#define __VA_SIZE(...)                                   __EXPAND(__COUNT(__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1, 0))
#define __CAT1(a, b)                                     a ## b
#define __CAT2(a, b)                                     __CAT1(a, b)
#define __DOC1(n1)                                       __doc_##n1
#define __DOC2(n1, n2)                                   __doc_##n1##_##n2
#define __DOC3(n1, n2, n3)                               __doc_##n1##_##n2##_##n3
#define __DOC4(n1, n2, n3, n4)                           __doc_##n1##_##n2##_##n3##_##n4
#define __DOC5(n1, n2, n3, n4, n5)                       __doc_##n1##_##n2##_##n3##_##n4##_##n5
#define __DOC6(n1, n2, n3, n4, n5, n6)                   __doc_##n1##_##n2##_##n3##_##n4##_##n5##_##n6
#define __DOC7(n1, n2, n3, n4, n5, n6, n7)               __doc_##n1##_##n2##_##n3##_##n4##_##n5##_##n6##_##n7
#define DOC(...)                                         __EXPAND(__EXPAND(__CAT2(__DOC, __VA_SIZE(__VA_ARGS__)))(__VA_ARGS__))

#if defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif


static const char *__doc_ASTNode =
R"doc(A node in the abstract syntax tree (AST) built from a flattened postfix
expression.

Each node holds: - A Token (NUMBER, VARIABLE, OPERATOR, or FUNCTION). - A list
of children (for operators, typically 2 children; for a function, 'argCount'
children).

Note: This uses raw pointers for children; you will need to manage memory
(delete) manually.)doc";

static const char *__doc_ASTNode_ASTNode =
R"doc(Constructor that initializes the node with a Token.

Parameter ``t``:
    The Token to store in this ASTNode.)doc";

static const char *__doc_ASTNode_children =
R"doc(Child pointers. For binary operators, typically size=2. For a function,
size=argCount.)doc";

static const char *__doc_ASTNode_token = R"doc(The token for this node (e.g., {OPERATOR, "+"} or {VARIABLE, "x"}).)doc";

static const char *__doc_AST_buildASTFromPostfix =
R"doc(Builds an AST from a flattened postfix expression.

Parameter ``postfix``:
    The flattened postfix tokens (no user-defined FUNCTION tokens remain).

Parameter ``functions``:
    Map of function name to Function struct (for predefined functions).

Returns:
    A pointer to the root ASTNode of the constructed tree. Caller is responsible
    for deleting it.

Throws:
    SolverException if there's a mismatch in the stack usage, unknown function,
    etc.)doc";

static const char *__doc_AST_evaluateAST =
R"doc(Recursively evaluates a simplified AST.

Parameter ``node``:
    Pointer to the root AST node (must not be null).

Parameter ``symbolTable``:
    The symbol table for looking up variables.

Parameter ``functions``:
    Map of predefined functions (for FUNCTION nodes).

Returns:
    The numeric result of evaluating the AST.

Throws:
    SolverException If an unknown operator or function is encountered, or if
    division by zero occurs, etc.)doc";

static const char *__doc_AST_printAST =
R"doc(Public-facing function to pretty-print the AST from its root.

Parameter ``root``:
    Pointer to the root ASTNode of the tree.)doc";

static const char *__doc_Function = R"doc()doc";

static const char *__doc_Function_Function = R"doc()doc";

static const char *__doc_Function_Function_2 = R"doc()doc";

static const char *__doc_Function_Function_3 = R"doc()doc";

static const char *__doc_Function_argCount = R"doc()doc";

static const char *__doc_Function_argumentNames = R"doc()doc";

static const char *__doc_Function_callback = R"doc()doc";

static const char *__doc_Function_inlinedPostfix = R"doc()doc";

static const char *__doc_Function_isPredefined = R"doc()doc";

static const char *__doc_LRUCache = R"doc()doc";

static const char *__doc_LRUCache_LRUCache = R"doc()doc";

static const char *__doc_LRUCache_cacheList = R"doc()doc";

static const char *__doc_LRUCache_cacheMap = R"doc()doc";

static const char *__doc_LRUCache_clear = R"doc()doc";

static const char *__doc_LRUCache_get = R"doc()doc";

static const char *__doc_LRUCache_maxSize = R"doc()doc";

static const char *__doc_LRUCache_put = R"doc()doc";

static const char *__doc_Postfix_evaluatePostfix = R"doc()doc";

static const char *__doc_Postfix_flattenPostfix = R"doc()doc";

static const char *__doc_Postfix_getFunctionArgCount =
R"doc(Retrieves the expected argument count for a given function.

Parameter ``functionName``:
    The name of the function.

Parameter ``functions``:
    Map of available functions with argument details.

Returns:
    Number of arguments the function expects.

Throws:
    SolverException if the function is not found.)doc";

static const char *__doc_Postfix_getPrecedence =
R"doc(Retrieves the precedence level of a given operator.

Parameter ``op``:
    The operator as a string.

Returns:
    Integer representing the precedence level.)doc";

static const char *__doc_Postfix_handleFunctionArgumentSeparator =
R"doc(Processes a function argument separator (comma) in the Shunting Yard algorithm.
Ensures operators are correctly handled within function arguments.

Parameter ``operatorStack``:
    Stack of operators.

Parameter ``outputQueue``:
    Queue for output tokens in postfix notation.

Parameter ``argumentCounts``:
    Stack tracking the argument counts for nested functions.

Throws:
    SolverException if parentheses or comma placement is invalid.)doc";

static const char *__doc_Postfix_handleParentheses =
R"doc(Handles closing parentheses in the Shunting Yard algorithm. Pops operators from
the stack until an opening parenthesis is found.

Parameter ``operatorStack``:
    Stack of operators.

Parameter ``outputQueue``:
    Queue for output tokens in postfix notation.

Throws:
    SolverException if parentheses are mismatched.)doc";

static const char *__doc_Postfix_isLeftAssociative =
R"doc(Determines if an operator is left-associative.

Parameter ``op``:
    The operator as a string.

Returns:
    True if the operator is left-associative, false otherwise.)doc";

static const char *__doc_Postfix_processOperatorStack =
R"doc(Manages the operator stack according to precedence and associativity rules.

Parameter ``token``:
    The current operator token.

Parameter ``operatorStack``:
    Stack of operators.

Parameter ``outputQueue``:
    Queue for output tokens in postfix notation.)doc";

static const char *__doc_Postfix_shuntingYard =
R"doc(Converts an infix expression represented by tokens to postfix notation using the
Shunting Yard algorithm.

Parameter ``tokens``:
    A vector of tokens representing the expression in infix notation.

Returns:
    A queue of tokens in postfix notation.)doc";

static const char *__doc_Profiler_InstrumentationSession = R"doc()doc";

static const char *__doc_Profiler_InstrumentationSession_name = R"doc()doc";

static const char *__doc_Profiler_InstrumentationTimer = R"doc()doc";

static const char *__doc_Profiler_InstrumentationTimer_InstrumentationTimer = R"doc()doc";

static const char *__doc_Profiler_InstrumentationTimer_name = R"doc()doc";

static const char *__doc_Profiler_InstrumentationTimer_start_timepoint = R"doc()doc";

static const char *__doc_Profiler_InstrumentationTimer_stop = R"doc()doc";

static const char *__doc_Profiler_InstrumentationTimer_stopped = R"doc()doc";

static const char *__doc_Profiler_Instrumentor = R"doc()doc";

static const char *__doc_Profiler_Instrumentor_Instrumentor = R"doc()doc";

static const char *__doc_Profiler_Instrumentor_Instrumentor_2 = R"doc()doc";

static const char *__doc_Profiler_Instrumentor_Instrumentor_3 = R"doc()doc";

static const char *__doc_Profiler_Instrumentor_begin_session = R"doc()doc";

static const char *__doc_Profiler_Instrumentor_current_session = R"doc()doc";

static const char *__doc_Profiler_Instrumentor_end_session = R"doc()doc";

static const char *__doc_Profiler_Instrumentor_get = R"doc()doc";

static const char *__doc_Profiler_Instrumentor_internal_end_session = R"doc()doc";

static const char *__doc_Profiler_Instrumentor_is_first_entry = R"doc()doc";

static const char *__doc_Profiler_Instrumentor_mutex = R"doc()doc";

static const char *__doc_Profiler_Instrumentor_output_stream = R"doc()doc";

static const char *__doc_Profiler_Instrumentor_write_footer = R"doc()doc";

static const char *__doc_Profiler_Instrumentor_write_header = R"doc()doc";

static const char *__doc_Profiler_Instrumentor_write_profile = R"doc()doc";

static const char *__doc_Profiler_ProfileResult = R"doc()doc";

static const char *__doc_Profiler_ProfileResult_elapsed_time = R"doc()doc";

static const char *__doc_Profiler_ProfileResult_name = R"doc()doc";

static const char *__doc_Profiler_ProfileResult_start = R"doc()doc";

static const char *__doc_Profiler_ProfileResult_thread_id = R"doc()doc";

static const char *__doc_Profiler_Utils_CleanedString = R"doc()doc";

static const char *__doc_Profiler_Utils_CleanedString_data = R"doc()doc";

static const char *__doc_Profiler_Utils_cleanup_output_string = R"doc()doc";

static const char *__doc_Simplification_fullySimplifyPostfix = R"doc()doc";

static const char *__doc_Simplification_replaceConstantSymbols =
R"doc(Replaces tokens that refer to constants with NUMBER tokens in a postfix
sequence.

If a token is of type VARIABLE but the symbol table indicates it is actually a
constant, we convert that token’s type to NUMBER and set its value to the
constant’s numeric string.

Parameter ``postfix``:
    The flattened postfix tokens (after flattenPostfix).

Parameter ``symbolTable``:
    The symbol table (holds variables and constants).

Returns:
    A new vector of tokens where any constant references have been inlined as
    numbers.)doc";

static const char *__doc_Simplification_simplifyAST =
R"doc(Recursively simplifies the AST in place, applying constant folding and basic
algebraic identities.

Parameter ``node``:
    The current AST node to simplify (may be modified in place).

Parameter ``functions``:
    A map of predefined functions (for function folding).

Returns:
    The (possibly replaced) pointer to the simplified AST node. If a node is
    replaced, the old pointer is deleted.)doc";

static const char *__doc_Simplification_singlePassSimplify =
R"doc(Performs a single pass of local folding/simplification on a fully flattened
postfix expression.

Parameter ``postfix``:
    The input postfix tokens (flattened).

Parameter ``functions``:
    The map of function names to Function definitions (for predefined funcs).

Parameter ``changed``:
    Set to true if any folding/simplification occurred during this pass, false
    otherwise.

Returns:
    A (possibly) simplified postfix sequence after one pass.)doc";

static const char *__doc_Simplification_trySimplifyBinary =
R"doc(Attempts to simplify a binary operation (leftExpr op rightExpr) using local
rules.

Parameter ``leftExpr``:
    Postfix tokens for the left operand (often a single token).

Parameter ``rightExpr``:
    Postfix tokens for the right operand (often a single token).

Parameter ``opToken``:
    The operator token (e.g. +, -, *, /, ^).

Returns:
    A new postfix token vector that represents the simplified expression.)doc";

static const char *__doc_Simplification_trySimplifyFunction = R"doc()doc";

static const char *__doc_Solver =
R"doc(A class for evaluating mathematical expressions, managing variables, constants,
and user-defined functions.

The Solver class provides methods for declaring constants, variables, user-
defined functions, and evaluating mathematical expressions. It supports both
predefined (built-in) and custom functions. It can also use memoization to cache
expression results, with an option to enable or disable caching on demand.)doc";

static const char *__doc_SolverException =
R"doc(Custom exception class for handling errors in the Solver class.

The SolverException class is derived from std::runtime_error and is thrown when
the Solver encounters an error during expression evaluation or function
declaration.)doc";

static const char *__doc_SolverException_SolverException =
R"doc(Constructor for the SolverException class.

Parameter ``message``:
    The error message to display when the exception is thrown.)doc";

static const char *__doc_Solver_Solver =
R"doc(Constructs a Solver instance and registers built-in functions.

By default, initializes an LRU cache for expression results of size
``exprCacheSize.`` Also starts the profiling session (if enabled) and calls
registerBuiltInFunctions() to load functions such as sin, cos, etc.

Parameter ``exprCacheSize``:
    The maximum number of entries the expression cache can hold (default 100).)doc";

static const char *__doc_Solver_cacheEnabled = R"doc(Flag indicating whether expression caching is currently active.)doc";

static const char *__doc_Solver_clearCache =
R"doc(Clears the solver's expression cache and function cache (if any).

This is a direct way to force the solver to discard all cached results. After
calling, the next evaluations will re-parse and re-compute the expression
outcomes from scratch.)doc";

static const char *__doc_Solver_currentAST = R"doc(The parsed (and flattened) AST tokens corresponding to currentExpression.)doc";

static const char *__doc_Solver_currentExpressionAST = R"doc(The most recent expression string passed to setCurrentExpression().)doc";

static const char *__doc_Solver_currentExpressionPostfix = R"doc(The most recent expression string passed to setCurrentExpression().)doc";

static const char *__doc_Solver_currentPostfix = R"doc(The parsed (and flattened) postfix tokens corresponding to currentExpression.)doc";

static const char *__doc_Solver_declareConstant =
R"doc(Declares a constant in the symbol table.

This inserts a new constant ``name`` with value ``value`` into the symbol table,
replacing any existing entry with the same name. Once declared, the constant
cannot be altered by normal assignment. Any existing caches are invalidated to
reflect the new symbol.

Parameter ``name``:
    The name of the constant (e.g., "pi").

Parameter ``value``:
    The numeric value of the constant (e.g., 3.14159).)doc";

static const char *__doc_Solver_declareFunction =
R"doc(Declares a user-defined function in terms of an expression and parameter list.

Internally, this parses the expression to a flattened postfix form and stores
it, along with the argument names. When invoked in other expressions, the
function is inlined (substituted for its body) or evaluated as needed.

Parameter ``name``:
    The function name (e.g. "f").

Parameter ``args``:
    A list of parameter names (e.g. ["x", "y"]).

Parameter ``expression``:
    The expression defining the function body (e.g. "x^2 + y^2").

Throws:
    SolverException If the function name is invalid, the syntax is incorrect, or
    a function by the same name already exists.)doc";

static const char *__doc_Solver_declareVariable =
R"doc(Declares (or re-declares) a variable in the symbol table.

This inserts a new variable ``name`` with initial value ``value`` into the
symbol table. If the variable already exists, its value is updated. Declaring a
variable also invalidates any cached expression results to ensure correctness if
that variable is used in expressions.

Parameter ``name``:
    The name of the variable (e.g., "x").

Parameter ``value``:
    The numeric value to assign to the variable.)doc";

static const char *__doc_Solver_evaluate =
R"doc(Evaluates a mathematical expression and returns its numeric result.

- Internally, this calls setCurrentExpression() which parses the expression (or
uses a cached parse if unchanged). - Then it checks the cache (if enabled) to
see if the expression result is already stored. - If not in cache, it evaluates
the expression in postfix form and stores the result if caching is on.

Parameter ``expression``:
    A string representing the mathematical expression to evaluate (e.g. "3 + 4 *
    2").

Parameter ``debug``:
    If true, prints debugging information such as the final postfix
    representation.

Returns:
    The computed value of the expression.

Throws:
    SolverException If there is a parsing error, missing function, or other
    runtime error.)doc";

static const char *__doc_Solver_evaluateAST =
R"doc(Evaluate an expression using the AST pipeline.

This either reuses an existing AST if `expression` is the same as last time, or
builds (and optionally simplifies) a new AST. Then calls AST::evaluateAST() or a
similar function to get the numeric result.

Parameter ``expression``:
    The mathematical expression string.

Parameter ``debug``:
    Whether to print debugging info.

Returns:
    The numeric evaluation result.

Throws:
    SolverException on parse errors, unknown symbols, etc.)doc";

static const char *__doc_Solver_evaluateForRange =
R"doc(Evaluates a mathematical expression for each value in a range of inputs for one
variable.

This method is similar to evaluate(), but it iterates over the provided vector
``values`` for the specified variable ``variable.`` For each value, the variable
is set in the symbol table, and the same parsed/postfix expression is evaluated.
This allows efficient bulk evaluation.

Parameter ``variable``:
    The name of the variable to iterate (e.g. "x").

Parameter ``values``:
    A vector of values to assign to that variable sequentially.

Parameter ``expression``:
    The mathematical expression to evaluate (e.g. "x^2 + 1").

Parameter ``debug``:
    If true, prints debug info for the parsing phase (once) and indicates
    evaluations in a loop.

Returns:
    A vector of computed results, with one result per value in ``values.``

Throws:
    SolverException If ``variable`` is invalid or if an error occurs during
    evaluation.)doc";

static const char *__doc_Solver_evaluateForRanges =
R"doc(Evaluates a single expression across multiple variables, each with a range of
values.

This method computes the cartesian product of all provided value ranges. For
each combination, it sets the corresponding variable values in the symbol table,
evaluates the (already-parsed) postfix expression, and appends the result to the
output vector.

The result vector is returned in row-major order with respect to the input
vectors in ``valuesSets,`` i.e., the last variable varies fastest.

Parameter ``variables``:
    A list of variable names, e.g., ["x", "y"].

Parameter ``valuesSets``:
    A corresponding list of ranges for each variable, e.g., [[0, 1, 2], [10,
    20]].

Parameter ``expression``:
    The expression to evaluate once for every combination (cartesian product).

Parameter ``debug``:
    If true, prints debug info for parsing; does NOT print each evaluation.

Returns:
    A flat vector of results (size = product of the lengths of each range in
    ``valuesSets).``

Throws:
    SolverException If a variable name is invalid, or expression parsing fails,
    etc.)doc";

static const char *__doc_Solver_expressionCache = R"doc(An LRU cache for storing evaluated expression results by hashed keys.)doc";

static const char *__doc_Solver_functions = R"doc(A map storing both built-in (predefined) and user-defined functions by name.)doc";

static const char *__doc_Solver_generateCacheKey =
R"doc(Generates an integer cache key based on an expression string and argument
values.

This function is used internally to create a unique hash for caching the result
of a specific expression and argument set, preventing collisions between
different calls.

Parameter ``base``:
    Typically the expression string or function signature.

Parameter ``args``:
    A list of argument values used when evaluating that expression.

Returns:
    A std::size_t representing a unique hash key.)doc";

static const char *__doc_Solver_getCurrentExpression =
R"doc(Retrieves the most recently set expression string.

This is simply the raw expression as last passed to setCurrentExpression().

Returns:
    The current expression string.)doc";

static const char *__doc_Solver_invalidateCaches =
R"doc(Invalidates solver caches if caching is enabled.

Primarily, this clears the expressionCache when any symbolic changes occur
(e.g., new variables or constants). Ensures results do not become stale.)doc";

static const char *__doc_Solver_listConstants =
R"doc(Lists all declared constants.

Provides a snapshot of the symbol table's constant entries, mapping constant
names to their numeric values.

Returns:
    An unordered_map from constant name to double value.)doc";

static const char *__doc_Solver_listVariables =
R"doc(Lists all declared variables.

Provides a snapshot of the symbol table's variable entries, mapping variable
names to their current numeric values.

Returns:
    An unordered_map from variable name to double value.)doc";

static const char *__doc_Solver_parse =
R"doc(Parses a mathematical expression from string to postfix.

The process includes tokenizing, converting tokens to postfix notation, and
flattening (inlining any user-defined functions). If ``debug`` is set, it may
print intermediate steps.

Parameter ``expression``:
    The input mathematical expression (in infix).

Parameter ``debug``:
    If true, prints debug information about the tokenization/postfix steps.

Returns:
    A vector of Tokens representing the flattened postfix form.

Throws:
    SolverException If a syntax error or unknown function is encountered.)doc";

static const char *__doc_Solver_parseAST =
R"doc(Parses a mathematical expression from string to postfix.

The process includes tokenizing, converting tokens to postfix notation, and
flattening (inlining any user-defined functions). If ``debug`` is set, it may
print intermediate steps.

Parameter ``expression``:
    The input mathematical expression (in infix).

Parameter ``debug``:
    If true, prints debug information about the tokenization/postfix steps.

Returns:
    A vector of Tokens representing the flattened postfix form.

Throws:
    SolverException If a syntax error or unknown function is encountered.)doc";

static const char *__doc_Solver_printFunctionExpressions =
R"doc(Prints expressions (postfix or inlined) for all registered functions to stdout.

This can be useful for debugging or understanding how user-defined functions
have been internally flattened to postfix representation.)doc";

static const char *__doc_Solver_registerBuiltInFunctions =
R"doc(Registers built-in mathematical functions such as sin, cos, exp, etc.

This method is called in the constructor. It adds standard functions into the
solver's function table with predefined callbacks.)doc";

static const char *__doc_Solver_registerPredefinedFunction =
R"doc(Registers a predefined function with a C++ callback.

This allows you to bind any C++ function or lambda that matches the
FunctionCallback signature as a recognized function in the solver. When the
function is invoked in an expression, the callback is called with the
appropriate number of arguments.

Parameter ``name``:
    The function name (e.g. "sin").

Parameter ``callback``:
    A C++ std::function or lambda matching the FunctionCallback signature
    (returns double, takes a vector of doubles).

Parameter ``argCount``:
    The number of arguments the function requires (e.g. 1 for sin).

Throws:
    SolverException If a function with the same name already exists.)doc";

static const char *__doc_Solver_setCurrentExpression =
R"doc(Sets the expression to be evaluated and parses it into a postfix representation.

If the given expression is different from the current one (or if there's no
valid current postfix), it tokenizes and converts the expression to postfix,
storing both. If the same expression was already set and parsed, it reuses the
stored postfix unless ``debug`` is true (in which case debug info may be
printed).

Parameter ``expression``:
    The new expression to parse and set as current.

Parameter ``debug``:
    If true, prints out the final postfix and related debug info.)doc";

static const char *__doc_Solver_setCurrentExpressionAST =
R"doc(Sets the current expression for AST-based evaluation (builds or re-builds an
AST).

This is similar to setCurrentExpression() but constructs an AST pipeline instead
of storing postfix tokens. If the expression is identical to the previously
stored one (and the AST is valid), we skip re-building unless debug is true.)doc";

static const char *__doc_Solver_setUseCache =
R"doc(Toggles whether the solver uses its LRU cache.

When disabled (``useCache`` = false), all subsequent evaluations will parse and
compute results fresh each time. Enabling again restarts caching but does not
retroactively recover old cached values.

Parameter ``useCache``:
    Pass true to enable expression caching, false to disable it.)doc";

static const char *__doc_Solver_symbolTable = R"doc(Symbol table for all declared variables and constants (manages their values).)doc";

static const char *__doc_Solver_validateFunctionDependencies =
R"doc(Validates that a user-defined function's dependencies are valid.

This checks whether each argument used in the function is well-formed, ensuring
no conflicts in naming or reserved keywords. Called during declareFunction().

Parameter ``expression``:
    The user-defined function's body in string form.

Parameter ``args``:
    The list of argument names for that function.

Throws:
    SolverException If an invalid argument name or reference is encountered.)doc";

static const char *__doc_SymbolEntry = R"doc()doc";

static const char *__doc_SymbolEntry_SymbolEntry = R"doc()doc";

static const char *__doc_SymbolEntry_SymbolEntry_2 = R"doc()doc";

static const char *__doc_SymbolEntry_type = R"doc()doc";

static const char *__doc_SymbolEntry_value = R"doc()doc";

static const char *__doc_SymbolTable = R"doc()doc";

static const char *__doc_SymbolTable_cachedSymbolName = R"doc()doc";

static const char *__doc_SymbolTable_cachedSymbolValue = R"doc()doc";

static const char *__doc_SymbolTable_clearVariables = R"doc()doc";

static const char *__doc_SymbolTable_constants = R"doc()doc";

static const char *__doc_SymbolTable_declareConstant = R"doc()doc";

static const char *__doc_SymbolTable_declareVariable = R"doc()doc";

static const char *__doc_SymbolTable_getConstants = R"doc()doc";

static const char *__doc_SymbolTable_getVariablePtr = R"doc()doc";

static const char *__doc_SymbolTable_getVariables = R"doc()doc";

static const char *__doc_SymbolTable_isConstant = R"doc()doc";

static const char *__doc_SymbolTable_isVariable = R"doc()doc";

static const char *__doc_SymbolTable_lookupSymbol = R"doc()doc";

static const char *__doc_SymbolTable_restoreVariables = R"doc()doc";

static const char *__doc_SymbolTable_variableIndex = R"doc()doc";

static const char *__doc_SymbolTable_variables = R"doc()doc";

static const char *__doc_SymbolType = R"doc()doc";

static const char *__doc_SymbolType_CONSTANT = R"doc()doc";

static const char *__doc_SymbolType_VARIABLE = R"doc()doc";

static const char *__doc_Token =
R"doc(Represents a token in a mathematical expression.

When the token is a number, the numeric value is precomputed and stored in the
numericValue field. This saves us from having to call std::stold() during
evaluation.)doc";

static const char *__doc_TokenType =
R"doc(@enum TokenType Represents the type of a token in a mathematical expression.

The TokenType enum defines the types of tokens that can be present in a
mathematical expression.)doc";

static const char *__doc_TokenType_FUNCTION = R"doc(A function (e.g., sin, cos, f))doc";

static const char *__doc_TokenType_NUMBER = R"doc(A numeric constant (e.g., 42, 3.14))doc";

static const char *__doc_TokenType_OPERATOR = R"doc(An operator (e.g., +, -, *, /, ^))doc";

static const char *__doc_TokenType_PAREN = R"doc(A parenthesis (either '(' or ')'))doc";

static const char *__doc_TokenType_SEPARATOR = R"doc(A separator (typically a comma in function arguments))doc";

static const char *__doc_TokenType_UNARY_OPERATOR = R"doc(A unary operator (e.g., negation))doc";

static const char *__doc_TokenType_VARIABLE = R"doc(A variable (e.g., x, y))doc";

static const char *__doc_Token_Token = R"doc()doc";

static const char *__doc_Token_Token_2 = R"doc()doc";

static const char *__doc_Token_numericValue = R"doc(Precomputed numeric value (only valid if type == NUMBER))doc";

static const char *__doc_Token_type = R"doc(The type of the token)doc";

static const char *__doc_Token_value = R"doc(The textual representation (for operators, variables, etc.))doc";

static const char *__doc_Tokenizer =
R"doc(A static utility class for tokenizing mathematical expressions.

The Tokenizer class provides functions to convert a mathematical expression
string into a series of tokens (numbers, variables, operators, etc.) using
regular expressions.)doc";

static const char *__doc_Tokenizer_handleNumberToken = R"doc()doc";

static const char *__doc_Tokenizer_handleOperatorToken = R"doc()doc";

static const char *__doc_Tokenizer_handleVariableOrFunctionToken = R"doc()doc";

static const char *__doc_Tokenizer_isUnaryContext = R"doc()doc";

static const char *__doc_Tokenizer_processMatch = R"doc()doc";

static const char *__doc_Tokenizer_tokenize =
R"doc(Tokenizes a mathematical expression into tokens.

Parameter ``equation``:
    The equation to tokenize.

Returns:
    A vector of tokens representing the equation.)doc";

static const char *__doc_Tokenizer_tokenizeUsingRegex = R"doc()doc";

static const char *__doc_Validator_isValidName =
R"doc(Validates whether a given name is a valid identifier.

The rules are similar to Python variable naming conventions: - Must start with a
letter (A-Z, a-z) or underscore (_) - Subsequent characters can be letters,
digits (0-9), or underscores

Parameter ``name``:
    The name to validate.

Returns:
    true If the name is valid.

Returns:
    false If the name is invalid.)doc";

static const char *__doc_Validator_isValidSyntax =
R"doc(Validate the syntax of the given expression.

Parameter ``expression``:
    The expression to validate.)doc";

static const char *__doc_numberToString = R"doc()doc";

static const char *__doc_postfixToInfix = R"doc()doc";

static const char *__doc_printInfix = R"doc()doc";

static const char *__doc_printPostfix = R"doc()doc";

static const char *__doc_printPrecisionInfo = R"doc()doc";

static const char *__doc_printTokens = R"doc()doc";

static const char *__doc_printTokens_2 =
R"doc(Print the tokens with their types for debugging purposes.

Parameter ``tokens``:
    A vector of Token objects representing the expression.)doc";

static const char *__doc_stringToNumber = R"doc()doc";

static const char *__doc_tokenTypeToString = R"doc()doc";

static const char *__doc_version =
R"doc(Get the software version information.

This function returns a string that contains the current version of the
software, along with the build date, time, and the system platform (Linux or
Windows). It also includes the version of Python being used in the environment.

Returns:
    A string containing version, build date, system platform, and Python
    version.)doc";

#if defined(__GNUG__)
#pragma GCC diagnostic pop
#endif

