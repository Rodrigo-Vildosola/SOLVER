# tests/test_error_handling.py
import pytest
from solver import SolverException

def test_division_by_zero(solver_with_defaults):
    solver_with_defaults.declare_variable("x", 5)
    solver_with_defaults.declare_variable("y", 10)
    # => 5/(10-10)=5/0 => exception
    with pytest.raises(SolverException):
        solver_with_defaults.evaluate("x / (y - 10)")

def test_undefined_function(solver_with_defaults):
    with pytest.raises(SolverException):
        solver_with_defaults.evaluate("undefinedFunc(5)")

def test_incorrect_num_args(solver_with_defaults):
    # f was declared with 1 arg, let's attempt 2
    with pytest.raises(SolverException):
        solver_with_defaults.evaluate("f(1, 2)")

def test_undefined_variable_in_function(solver_with_defaults):
    # Suppose we define n(x)= x + z => 'z' is undefined
    solver_with_defaults.declare_function("n", ["x"], "x + z")
    with pytest.raises(SolverException):
        solver_with_defaults.evaluate("n(5)")
