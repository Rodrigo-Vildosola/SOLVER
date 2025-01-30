"""
Python bindings for the solver C++ math expression parsing and solving library.
"""
from __future__ import annotations
__all__ = ['Solver', 'SolverException', 'version']
class Solver:
    """
    A class for evaluating mathematical expressions, managing variables, constants,
    and user-defined functions.
    
    The Solver class provides methods for declaring constants, variables, user-
    defined functions, and evaluating mathematical expressions. It supports both
    predefined (built-in) and custom functions. It can also use memoization to cache
    expression results, with an option to enable or disable caching on demand.
    """
    @staticmethod
    def _pybind11_conduit_v1_(*args, **kwargs):
        ...
    def __init__(self, cache_size: int = 100) -> None:
        """
        Constructs a Solver instance and registers built-in functions.
        
        By default, initializes an LRU cache for expression results of size
        ``exprCacheSize.`` Also starts the profiling session (if enabled) and calls
        registerBuiltInFunctions() to load functions such as sin, cos, etc.
        
        Parameter ``exprCacheSize``:
            The maximum number of entries the expression cache can hold (default 100).
        """
    def clear_cache(self) -> None:
        """
        Clears the solver's expression cache and function cache (if any).
        
        This is a direct way to force the solver to discard all cached results. After
        calling, the next evaluations will re-parse and re-compute the expression
        outcomes from scratch.
        """
    def declare_constant(self, name: str, value: float) -> None:
        """
        Declares a constant in the symbol table.
        
        This inserts a new constant ``name`` with value ``value`` into the symbol table,
        replacing any existing entry with the same name. Once declared, the constant
        cannot be altered by normal assignment. Any existing caches are invalidated to
        reflect the new symbol.
        
        Parameter ``name``:
            The name of the constant (e.g., "pi").
        
        Parameter ``value``:
            The numeric value of the constant (e.g., 3.14159).
        """
    def declare_function(self, name: str, args: list[str], expression: str) -> None:
        """
        Declares a user-defined function in terms of an expression and parameter list.
        
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
            a function by the same name already exists.
        """
    def declare_variable(self, name: str, value: float) -> None:
        """
        Declares (or re-declares) a variable in the symbol table.
        
        This inserts a new variable ``name`` with initial value ``value`` into the
        symbol table. If the variable already exists, its value is updated. Declaring a
        variable also invalidates any cached expression results to ensure correctness if
        that variable is used in expressions.
        
        Parameter ``name``:
            The name of the variable (e.g., "x").
        
        Parameter ``value``:
            The numeric value to assign to the variable.
        """
    def evaluate(self, expression: str, debug: bool = False) -> float:
        """
        Evaluates a mathematical expression and returns its numeric result.
        
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
            runtime error.
        """
    def evaluate_ast(self, expression: str, debug: bool = False) -> float:
        """
        Evaluate an expression using the AST pipeline.
        
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
            SolverException on parse errors, unknown symbols, etc.
        """
    def evaluate_range(self, variable: str, values: list[float], expression: str, debug: bool = False) -> list[float]:
        """
        Evaluates a mathematical expression for each value in a range of inputs for one
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
            evaluation.
        """
    def evaluate_ranges(self, variables: list[str], valuesSets: list[list[float]], expression: str, debug: bool = False) -> list[float]:
        """
        Evaluates a single expression across multiple variables, each with a range of
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
            etc.
        """
    def get_current_expression(self) -> str:
        """
        Retrieves the most recently set expression string.
        
        This is simply the raw expression as last passed to setCurrentExpression().
        
        Returns:
            The current expression string.
        """
    def list_constants(self) -> dict[str, float]:
        """
        Lists all declared constants.
        
        Provides a snapshot of the symbol table's constant entries, mapping constant
        names to their numeric values.
        
        Returns:
            An unordered_map from constant name to double value.
        """
    def list_variables(self) -> dict[str, float]:
        """
        Lists all declared variables.
        
        Provides a snapshot of the symbol table's variable entries, mapping variable
        names to their current numeric values.
        
        Returns:
            An unordered_map from variable name to double value.
        """
    def print_function_expressions(self) -> None:
        """
        Prints expressions (postfix or inlined) for all registered functions to stdout.
        
        This can be useful for debugging or understanding how user-defined functions
        have been internally flattened to postfix representation.
        """
    def set_current_expression(self, expression: str, debug: bool = False) -> None:
        """
        Sets the expression to be evaluated and parses it into a postfix representation.
        
        If the given expression is different from the current one (or if there's no
        valid current postfix), it tokenizes and converts the expression to postfix,
        storing both. If the same expression was already set and parsed, it reuses the
        stored postfix unless ``debug`` is true (in which case debug info may be
        printed).
        
        Parameter ``expression``:
            The new expression to parse and set as current.
        
        Parameter ``debug``:
            If true, prints out the final postfix and related debug info.
        """
    def set_current_expression_ast(self, expression: str, debug: bool = False) -> None:
        """
        Sets the current expression for AST-based evaluation (builds or re-builds an
        AST).
        
        This is similar to setCurrentExpression() but constructs an AST pipeline instead
        of storing postfix tokens. If the expression is identical to the previously
        stored one (and the AST is valid), we skip re-building unless debug is true.
        """
    def use_cache(self, useCache: bool) -> None:
        """
        Toggles whether the solver uses its LRU cache.
        
        When disabled (``useCache`` = false), all subsequent evaluations will parse and
        compute results fresh each time. Enabling again restarts caching but does not
        retroactively recover old cached values.
        
        Parameter ``useCache``:
            Pass true to enable expression caching, false to disable it.
        """
class SolverException(Exception):
    pass
def version() -> str:
    """
    Get the software version information.
    
    This function returns a string that contains the current version of the
    software, along with the build date, time, and the system platform (Linux or
    Windows). It also includes the version of Python being used in the environment.
    
    Returns:
        A string containing version, build date, system platform, and Python
        version.
    """
