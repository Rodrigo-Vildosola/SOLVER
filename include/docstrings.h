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


static const char *__doc_ExprNode = R"doc()doc";

static const char *__doc_ExprNode_2 = R"doc()doc";

static const char *__doc_ExprNode_ExprNode = R"doc()doc";

static const char *__doc_ExprNode_ExprNode_2 = R"doc()doc";

static const char *__doc_ExprNode_ExprNode_3 = R"doc()doc";

static const char *__doc_ExprNode_arguments = R"doc()doc";

static const char *__doc_ExprNode_destroy = R"doc()doc";

static const char *__doc_ExprNode_left = R"doc()doc";

static const char *__doc_ExprNode_operator_delete = R"doc()doc";

static const char *__doc_ExprNode_operator_new = R"doc()doc";

static const char *__doc_ExprNode_right = R"doc()doc";

static const char *__doc_ExprNode_type = R"doc()doc";

static const char *__doc_ExprNode_value = R"doc()doc";

static const char *__doc_ExpressionTree_getFunctionArgCount =
R"doc(Retrieves the expected argument count for a given function.

Parameter ``functionName``:
    The name of the function.

Parameter ``functions``:
    Map of available functions with argument details.

Returns:
    Number of arguments the function expects.

Throws:
    SolverException if the function is not found.)doc";

static const char *__doc_ExpressionTree_getPrecedence =
R"doc(Retrieves the precedence level of a given operator.

Parameter ``op``:
    The operator as a string.

Returns:
    Integer representing the precedence level.)doc";

static const char *__doc_ExpressionTree_handleFunctionArgumentSeparator =
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

static const char *__doc_ExpressionTree_handleParentheses =
R"doc(Handles closing parentheses in the Shunting Yard algorithm. Pops operators from
the stack until an opening parenthesis is found.

Parameter ``operatorStack``:
    Stack of operators.

Parameter ``outputQueue``:
    Queue for output tokens in postfix notation.

Throws:
    SolverException if parentheses are mismatched.)doc";

static const char *__doc_ExpressionTree_isLeftAssociative =
R"doc(Determines if an operator is left-associative.

Parameter ``op``:
    The operator as a string.

Returns:
    True if the operator is left-associative, false otherwise.)doc";

static const char *__doc_ExpressionTree_parseExpression =
R"doc(Parses a list of tokens into an expression tree.

Parameter ``tokens``:
    A vector of tokens representing the expression.

Parameter ``functions``:
    A map of available functions with their details.

Returns:
    Pointer to the root of the parsed expression tree (ExprNode).)doc";

static const char *__doc_ExpressionTree_processFunction =
R"doc(Processes a function token and its arguments from the node stack, creating a
function node in the expression tree.

Parameter ``token``:
    The function token to process.

Parameter ``nodeStack``:
    Stack of expression nodes to pop arguments from.

Parameter ``functions``:
    Map of available functions with argument details.

Returns:
    Pointer to the function expression node created.)doc";

static const char *__doc_ExpressionTree_processOperator =
R"doc(Processes an operator token and its operands from the node stack, creating an
operator node in the expression tree.

Parameter ``token``:
    The operator token to process.

Parameter ``nodeStack``:
    Stack of expression nodes to pop operands from.

Returns:
    Pointer to the operator expression node created.)doc";

static const char *__doc_ExpressionTree_processOperatorStack =
R"doc(Manages the operator stack according to precedence and associativity rules.

Parameter ``token``:
    The current operator token.

Parameter ``operatorStack``:
    Stack of operators.

Parameter ``outputQueue``:
    Queue for output tokens in postfix notation.)doc";

static const char *__doc_ExpressionTree_shuntingYard =
R"doc(Converts an infix expression represented by tokens to postfix notation using the
Shunting Yard algorithm.

Parameter ``tokens``:
    A vector of tokens representing the expression in infix notation.

Returns:
    A queue of tokens in postfix notation.)doc";

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

static const char *__doc_MemoryPool = R"doc()doc";

static const char *__doc_MemoryPool_MemoryPool = R"doc()doc";

static const char *__doc_MemoryPool_allocate = R"doc()doc";

static const char *__doc_MemoryPool_allocatePool = R"doc()doc";

static const char *__doc_MemoryPool_clearPool = R"doc()doc";

static const char *__doc_MemoryPool_construct = R"doc()doc";

static const char *__doc_MemoryPool_deallocate = R"doc()doc";

static const char *__doc_MemoryPool_destroy = R"doc()doc";

static const char *__doc_MemoryPool_freeList = R"doc()doc";

static const char *__doc_MemoryPool_memoryBlocks = R"doc()doc";

static const char *__doc_MemoryPool_poolSize = R"doc()doc";

static const char *__doc_MultiPassSimplifier = R"doc()doc";

static const char *__doc_MultiPassSimplifier_MultiPassSimplifier = R"doc()doc";

static const char *__doc_MultiPassSimplifier_initializePasses = R"doc()doc";

static const char *__doc_MultiPassSimplifier_passes = R"doc()doc";

static const char *__doc_MultiPassSimplifier_simplify = R"doc()doc";

static const char *__doc_MultiPassSimplifier_symbolTable = R"doc()doc";

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

static const char *__doc_Simplification_RewriteEngine = R"doc()doc";

static const char *__doc_Simplification_RewriteEngine_addRule = R"doc()doc";

static const char *__doc_Simplification_RewriteEngine_applyRules = R"doc()doc";

static const char *__doc_Simplification_RewriteEngine_rules = R"doc()doc";

static const char *__doc_Simplification_RewriteRule = R"doc()doc";

static const char *__doc_Simplification_RewriteRule_apply = R"doc()doc";

static const char *__doc_Simplification_RewriteRule_matches = R"doc()doc";

static const char *__doc_Simplification_RewriteRule_pattern = R"doc()doc";

static const char *__doc_Simplification_RewriteRule_replacement = R"doc()doc";

static const char *__doc_Solver =
R"doc(A class for evaluating mathematical expressions, managing variables, constants,
and user-defined functions.

The Solver class provides methods for declaring constants, variables, user-
defined functions, and evaluating mathematical expressions. It supports both
predefined and custom functions. The class also supports memoization for caching
function and expression results, with an option to disable caching if necessary.)doc";

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
R"doc(Constructor for the Solver class.

Initializes the Solver instance and registers built-in functions.)doc";

static const char *__doc_Solver_cacheEnabled = R"doc(Flag to enable or disable cache)doc";

static const char *__doc_Solver_clearCache =
R"doc(Clears the cache when variables or constants change.

This clears both the function and expression caches to ensure correct
recalculation of results.)doc";

static const char *__doc_Solver_currentExpression = R"doc(The current expression being evaluated)doc";

static const char *__doc_Solver_currentPostfix = R"doc()doc";

static const char *__doc_Solver_declareConstant =
R"doc(Declare a constant in the solver's symbol table.

Parameter ``name``:
    The name of the constant.

Parameter ``value``:
    The value of the constant.)doc";

static const char *__doc_Solver_declareFunction =
R"doc(Declare a user-defined function.

Declares a function in the solver's function table, allowing the user to define
functions with expressions and variables.

Parameter ``name``:
    The name of the function.

Parameter ``args``:
    A vector of argument names for the function.

Parameter ``expression``:
    The mathematical expression defining the function.)doc";

static const char *__doc_Solver_declareVariable =
R"doc(Declare a variable in the solver's symbol table.

Parameter ``name``:
    The name of the variable.

Parameter ``value``:
    The initial value of the variable.)doc";

static const char *__doc_Solver_evaluate =
R"doc(Evaluate a mathematical expression.

This method parses and simplifies the expression (if not cached), evaluates the
expression, and returns the result.

Parameter ``expression``:
    The mathematical expression to evaluate.

Parameter ``debug``:
    If true, enables debug output during evaluation.

Returns:
    The result of evaluating the expression.)doc";

static const char *__doc_Solver_evaluateForRange =
R"doc(Evaluate a mathematical expression over a range of values for a given variable.

The method parses and simplifies the expression once, then evaluates the
expression tree for each value in the range.

Parameter ``variable``:
    The name of the variable to iterate over.

Parameter ``values``:
    A vector of values for the variable.

Parameter ``expression``:
    The mathematical expression to evaluate.

Parameter ``debug``:
    If true, enables debug output during evaluation.

Returns:
    A vector of results for each value in the range.)doc";

static const char *__doc_Solver_expressionCache = R"doc(LRU Cache for expression evaluation results)doc";

static const char *__doc_Solver_functions = R"doc()doc";

static const char *__doc_Solver_generateCacheKey =
R"doc(Generates a unique cache key for a given expression or function call.

Parameter ``base``:
    The base string for the cache key (typically the expression or function
    name).

Parameter ``args``:
    The arguments used in the expression or function.

Returns:
    A unique string to use as the cache key.)doc";

static const char *__doc_Solver_getCurrentExpression =
R"doc(Get the current expression being evaluated.

Returns:
    The current expression being evaluated.)doc";

static const char *__doc_Solver_invalidateCaches =
R"doc(Invalidate cached results.

This function is called when the solver's state changes, such as when variables
or constants are declared.)doc";

static const char *__doc_Solver_listConstants =
R"doc(Lists all declared constants.

Returns:
    A map of constant names and their values.)doc";

static const char *__doc_Solver_listVariables =
R"doc(Lists all declared variables.

Returns:
    A map of variable names and their values.)doc";

static const char *__doc_Solver_parse =
R"doc(Parses and simplifies a mathematical expression.

Parameter ``expression``:
    The mathematical expression to parse.

Parameter ``debug``:
    If true, enables debug output during parsing and simplification.

Returns:
    A unique pointer to the simplified expression tree (ExprNode).)doc";

static const char *__doc_Solver_printFunctionExpressions = R"doc()doc";

static const char *__doc_Solver_registerBuiltInFunctions =
R"doc(Register built-in mathematical functions like sin, cos, etc.

This method is called during initialization to register standard mathematical
functions.)doc";

static const char *__doc_Solver_registerPredefinedFunction =
R"doc(Register a predefined function with a callback.

Registers a function in the solver's function table using a predefined callback
for execution.

Parameter ``name``:
    The name of the function.

Parameter ``callback``:
    A function callback to execute when the function is called.

Parameter ``argCount``:
    The number of arguments the function expects.)doc";

static const char *__doc_Solver_setCurrentExpression =
R"doc(Sets the current expression and parses it into an expression tree.

This method stores the expression and its parsed tree, so future evaluations of
the same expression can use the cached tree.

Parameter ``expression``:
    The mathematical expression to set as the current expression.

Parameter ``debug``:
    If true, enables debug output during parsing and simplification.)doc";

static const char *__doc_Solver_setUseCache =
R"doc(Set cache usage flag.

Parameter ``useCache``:
    If true, enables caching of results. If false, disables caching.)doc";

static const char *__doc_Solver_symbolTable = R"doc()doc";

static const char *__doc_Solver_validateFunctionDependencies =
R"doc(Validate that a function's dependencies (arguments) are valid.

Parameter ``expression``:
    The function expression.

Parameter ``args``:
    The arguments that the function depends on.)doc";

static const char *__doc_SymbolEntry = R"doc()doc";

static const char *__doc_SymbolEntry_SymbolEntry = R"doc()doc";

static const char *__doc_SymbolEntry_SymbolEntry_2 = R"doc()doc";

static const char *__doc_SymbolEntry_type = R"doc()doc";

static const char *__doc_SymbolEntry_value = R"doc()doc";

static const char *__doc_SymbolTable = R"doc()doc";

static const char *__doc_SymbolTable_cachedSymbolEntry = R"doc()doc";

static const char *__doc_SymbolTable_cachedSymbolName = R"doc()doc";

static const char *__doc_SymbolTable_clearVariables = R"doc()doc";

static const char *__doc_SymbolTable_clone = R"doc()doc";

static const char *__doc_SymbolTable_declareConstant = R"doc()doc";

static const char *__doc_SymbolTable_declareVariable = R"doc()doc";

static const char *__doc_SymbolTable_entries = R"doc()doc";

static const char *__doc_SymbolTable_getConstants = R"doc()doc";

static const char *__doc_SymbolTable_getEntries = R"doc()doc";

static const char *__doc_SymbolTable_getVariables = R"doc()doc";

static const char *__doc_SymbolTable_isConstant = R"doc()doc";

static const char *__doc_SymbolTable_isVariable = R"doc()doc";

static const char *__doc_SymbolTable_lookupSymbol = R"doc()doc";

static const char *__doc_SymbolTable_restoreVariables = R"doc()doc";

static const char *__doc_SymbolType = R"doc()doc";

static const char *__doc_SymbolType_CONSTANT = R"doc()doc";

static const char *__doc_SymbolType_VARIABLE = R"doc()doc";

static const char *__doc_Token =
R"doc(Represents a token in a mathematical expression.

A Token consists of a type (from the TokenType enum) and a string value. It is
used to break down mathematical expressions into manageable components.)doc";

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

static const char *__doc_Token_type = R"doc(The type of the token (e.g., NUMBER, OPERATOR))doc";

static const char *__doc_Token_value = R"doc(The string value of the token (e.g., "42", "+", "x"))doc";

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

static const char *__doc_postfixToInfix = R"doc()doc";

static const char *__doc_printBoxedContent =
R"doc(Print a boxed content block for displaying debug information.

This method creates a dynamic box based on the length of the content.

Parameter ``content``:
    The content to be displayed inside the box.)doc";

static const char *__doc_printBoxedHeader =
R"doc(Print a boxed header with colored output for debug mode.

Parameter ``title``:
    The title of the header to be displayed.)doc";

static const char *__doc_printInfix = R"doc()doc";

static const char *__doc_printPostfix = R"doc()doc";

static const char *__doc_printTokens = R"doc()doc";

static const char *__doc_printTokens_2 =
R"doc(Print the tokens with their types for debugging purposes.

Parameter ``tokens``:
    A vector of Token objects representing the expression.)doc";

static const char *__doc_printTree =
R"doc(Print the expression tree for debugging purposes, including function
definitions.

This function recursively prints the tree structure and handles special cases
for functions, showing inline arguments and sub-trees for functions.

Parameter ``node``:
    The current node of the expression tree.

Parameter ``out``:
    The output stream to print to (usually std::cout).

Parameter ``functions``:
    The function map to handle user-defined functions.

Parameter ``depth``:
    The depth of the current node (used for indentation).

Parameter ``prefix``:
    The string prefix for the current node (used for formatting).

Parameter ``isLeft``:
    Whether the current node is a left child.)doc";

static const char *__doc_tokenTypeToString = R"doc()doc";

#if defined(__GNUG__)
#pragma GCC diagnostic pop
#endif

