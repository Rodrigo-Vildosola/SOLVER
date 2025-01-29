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
    predefined and custom functions. The class also supports memoization for caching
    function and expression results, with an option to disable caching if necessary.
    """
    @staticmethod
    def _pybind11_conduit_v1_(*args, **kwargs):
        ...
    def __init__(self, exprCacheSize: int = 100) -> None:
        """
        Constructor for the Solver class.
        
        Initializes the Solver instance and registers built-in functions.
        """
    def clear_cache(self) -> None:
        """
        Clears the cache when variables or constants change.
        
        This clears both the function and expression caches to ensure correct
        recalculation of results.
        """
    def declare_constant(self, name: str, value: float) -> None:
        """
        Declare a constant in the solver's symbol table.
        
        Parameter ``name``:
            The name of the constant.
        
        Parameter ``value``:
            The value of the constant.
        """
    def declare_function(self, name: str, args: list[str], expression: str) -> None:
        """
        Declare a user-defined function.
        
        Declares a function in the solver's function table, allowing the user to define
        functions with expressions and variables.
        
        Parameter ``name``:
            The name of the function.
        
        Parameter ``args``:
            A vector of argument names for the function.
        
        Parameter ``expression``:
            The mathematical expression defining the function.
        """
    def declare_variable(self, name: str, value: float) -> None:
        """
        Declare a variable in the solver's symbol table.
        
        Parameter ``name``:
            The name of the variable.
        
        Parameter ``value``:
            The initial value of the variable.
        """
    def evaluate(self, expression: str, debug: bool = False) -> float:
        """
        Evaluate a mathematical expression.
        
        This method parses and simplifies the expression (if not cached), evaluates the
        expression, and returns the result.
        
        Parameter ``expression``:
            The mathematical expression to evaluate.
        
        Parameter ``debug``:
            If true, enables debug output during evaluation.
        
        Returns:
            The result of evaluating the expression.
        """
    def evaluate_range(self, variable: str, values: list[float], expression: str, debug: bool = False) -> list[float]:
        """
        Evaluate a mathematical expression over a range of values for a given variable.
        
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
            A vector of results for each value in the range.
        """
    def get_current_expression(self) -> str:
        """
        Get the current expression being evaluated.
        
        Returns:
            The current expression being evaluated.
        """
    def list_constants(self) -> dict[str, float]:
        """
        Lists all declared constants.
        
        Returns:
            A map of constant names and their values.
        """
    def list_variables(self) -> dict[str, float]:
        """
        Lists all declared variables.
        
        Returns:
            A map of variable names and their values.
        """
    def print_function_expressions(self) -> None:
        ...
    def set_current_expression(self, expression: str, debug: bool = False) -> None:
        """
        Sets the current expression and parses it into an expression tree.
        
        This method stores the expression and its parsed tree, so future evaluations of
        the same expression can use the cached tree.
        
        Parameter ``expression``:
            The mathematical expression to set as the current expression.
        
        Parameter ``debug``:
            If true, enables debug output during parsing and simplification.
        """
    def use_cache(self, useCache: bool) -> None:
        """
        Set cache usage flag.
        
        Parameter ``useCache``:
            If true, enables caching of results. If false, disables caching.
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
