# tests/test_unary_minus.py
import pytest
import math

def test_triple_minus(solver_with_defaults):
    # ---5 => -(-(-5))
    val= solver_with_defaults.evaluate("---5")
    assert math.isclose(val, -5.0, abs_tol=1e-9)

def test_quad_minus(solver_with_defaults):
    # ----5 => 5
    val= solver_with_defaults.evaluate("----5")
    assert math.isclose(val, 5.0, abs_tol=1e-9)

def test_nested_minus_parens(solver_with_defaults):
    # -(-(-(-5))) => 5
    val= solver_with_defaults.evaluate("-(-(-(-5)))")
    assert math.isclose(val, 5.0, abs_tol=1e-9)

def test_minus_minus_with_variable(solver_with_defaults):
    solver_with_defaults.declare_variable("x",10)
    # --(x + -5) => ?
    val= solver_with_defaults.evaluate("--(x + -5)")
    assert math.isclose(val, 5.0, abs_tol=1e-9)

def test_double_minus_mult(solver_with_defaults):
    # --(2 * -3) => -6
    # Explanation: (2* -3)= -6 => then -- => -6 =>  ...
    val= solver_with_defaults.evaluate("--(2 * -3)")
    assert math.isclose(val, -6.0, abs_tol=1e-9)

# And so on for all your unary minus test cases
