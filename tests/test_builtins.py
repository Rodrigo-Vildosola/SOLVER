# tests/test_builtins.py
import pytest
import math

def test_sin_pi_over_2(solver_with_defaults):
    val= solver_with_defaults.evaluate("sin(pi/2)")
    assert math.isclose(val,1.0,abs_tol=1e-9)

def test_cos_0(solver_with_defaults):
    val= solver_with_defaults.evaluate("cos(0)")
    assert math.isclose(val,1.0,abs_tol=1e-9)

def test_tan_pi_over_4(solver_with_defaults):
    val= solver_with_defaults.evaluate("tan(pi/4)")
    assert math.isclose(val,1.0,abs_tol=1e-9)

def test_exp_1(solver_with_defaults):
    val= solver_with_defaults.evaluate("exp(1)")
    assert math.isclose(val,2.718282,rel_tol=1e-9)

def test_ln_e(solver_with_defaults):
    val= solver_with_defaults.evaluate("ln(e)")
    assert math.isclose(val,1.0,abs_tol=1e-9)

def test_sqrt_16(solver_with_defaults):
    val= solver_with_defaults.evaluate("sqrt(16)")
    assert math.isclose(val,4.0,abs_tol=1e-9)

def test_sin_cos_pi_over_4(solver_with_defaults):
    # sin(pi/4)+ cos(pi/4) => ~1.4142
    val= solver_with_defaults.evaluate("sin(pi/4) + cos(pi/4)")
    assert math.isclose(val,1.4142135623730951,rel_tol=1e-5)

def test_ln_e_squared(solver_with_defaults):
    # ln(e^2)=2
    val= solver_with_defaults.evaluate("ln(e^2)")
    assert math.isclose(val,2.0,abs_tol=1e-9)

def test_exp_ln_5(solver_with_defaults):
    # exp(ln(5)) =>5
    val= solver_with_defaults.evaluate("exp(ln(5))")
    assert math.isclose(val,5.0,abs_tol=1e-9)

def test_circle_area(solver_with_defaults):
    val= solver_with_defaults.evaluate("circle_area(3)")
    # pi*r^2 => pi*9 => ~28.2743
    assert math.isclose(val,28.274337, rel_tol=1e-9)

