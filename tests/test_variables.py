# tests/test_variables.py
import pytest
import math

def test_x_plus_0(solver_with_defaults):
    # We can declare variable x = 0 
    solver_with_defaults.declare_variable("x", 0)
    result = solver_with_defaults.evaluate("x + 0")
    assert math.isclose(result, 0.0, abs_tol=1e-9)

def test_x_plus_y(solver_with_defaults):
    solver_with_defaults.declare_variable("x", 5)
    solver_with_defaults.declare_variable("y", 10)
    result = solver_with_defaults.evaluate("x + y")
    assert math.isclose(result, 15.0, abs_tol=1e-9)
