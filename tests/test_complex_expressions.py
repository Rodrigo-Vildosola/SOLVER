# tests/test_complex_expressions.py
import pytest
import math

def test_x_times_y_plus_x_squared(solver_with_defaults):
    solver_with_defaults.declare_variable("x", 5)
    solver_with_defaults.declare_variable("y", 10)
    # x * y + x^2 => 5*10 + 25 => 75
    result = solver_with_defaults.evaluate("x * y + x^2")
    assert math.isclose(result, 75.0, abs_tol=1e-9)

def test_y_div_x(solver_with_defaults):
    solver_with_defaults.declare_variable("x", 5)
    solver_with_defaults.declare_variable("y", 10)
    # y / x => 10/5 => 2
    result = solver_with_defaults.evaluate("y / x")
    assert math.isclose(result, 2.0, abs_tol=1e-9)
