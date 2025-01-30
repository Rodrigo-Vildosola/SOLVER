# tests/test_variables.py
import pytest
import math

def test_x_plus_0(solver_with_defaults):
    # x=0 => x+0=0
    solver_with_defaults.declare_variable("x", 0)
    result = solver_with_defaults.evaluate("x + 0")
    assert math.isclose(result, 0.0, abs_tol=1e-9)

def test_x_plus_y(solver_with_defaults):
    # x=5, y=10 => x+y=15
    solver_with_defaults.declare_variable("x", 5)
    solver_with_defaults.declare_variable("y", 10)
    result = solver_with_defaults.evaluate("x + y")
    assert math.isclose(result, 15.0, abs_tol=1e-9)

def test_x_minus_y(solver_with_defaults):
    # x=10,y=5 => x-y=5
    solver_with_defaults.declare_variable("x", 10)
    solver_with_defaults.declare_variable("y", 5)
    result = solver_with_defaults.evaluate("x - y")
    assert math.isclose(result, 5.0, abs_tol=1e-9)

def test_x_times_1(solver_with_defaults):
    # x=6 => x*1=6
    solver_with_defaults.declare_variable("x", 6)
    result = solver_with_defaults.evaluate("x * 1")
    assert math.isclose(result, 6.0, abs_tol=1e-9)

def test_y_divided_by_x(solver_with_defaults):
    # x=4,y=16 => y/x=4
    solver_with_defaults.declare_variable("x", 4)
    solver_with_defaults.declare_variable("y", 16)
    result = solver_with_defaults.evaluate("y / x")
    assert math.isclose(result, 4.0, abs_tol=1e-9)

def test_variable_shadowing(solver_with_defaults):
    # We try re-declare x, then do x+10
    solver_with_defaults.declare_variable("x", 1)
    solver_with_defaults.declare_variable("x", 5)  # Overwrite old value
    result = solver_with_defaults.evaluate("x + 10")
    assert math.isclose(result, 15.0, abs_tol=1e-9)

def test_decimal_variables(solver_with_defaults):
    # x=2.5 => x^2=6.25 => x+ x^2=2.5+6.25=8.75
    solver_with_defaults.declare_variable("x", 2.5)
    result = solver_with_defaults.evaluate("x + x^2")
    assert math.isclose(result, 8.75, abs_tol=1e-9)

def test_multiple_variables_expression(solver_with_defaults):
    # x=3, y=4 => x^2+ y^2 => 9+16=25
    solver_with_defaults.declare_variable("x", 3)
    solver_with_defaults.declare_variable("y", 4)
    result = solver_with_defaults.evaluate("x^2 + y^2")
    assert math.isclose(result, 25.0, abs_tol=1e-9)

def test_unset_variable_gives_exception(solver_with_defaults):
    # We do not set 'x' => solver should raise an exception if 'x' is used
    with pytest.raises(Exception):
        solver_with_defaults.evaluate("x + 1")

def test_temporary_var_override(solver_with_defaults):
    # x=5 => x+2=7
    solver_with_defaults.declare_variable("x", 5)
    val1 = solver_with_defaults.evaluate("x + 2")
    assert math.isclose(val1, 7.0, abs_tol=1e-9)

    # Now override x=10 => expression changes to 12
    solver_with_defaults.declare_variable("x", 10)
    val2 = solver_with_defaults.evaluate("x + 2")
    assert math.isclose(val2, 12.0, abs_tol=1e-9)
