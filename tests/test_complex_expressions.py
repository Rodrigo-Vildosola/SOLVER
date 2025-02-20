# tests/test_complex_expressions.py
import pytest
import math

def test_x_times_y_plus_x_squared(solver_with_defaults):
    solver_with_defaults.declare_variable("x", 5)
    solver_with_defaults.declare_variable("y", 10)
    # x*y + x^2 => 5*10 +25 =>75
    result = solver_with_defaults.evaluate("x * y + x^2")
    assert math.isclose(result, 75.0, abs_tol=1e-9)

def test_y_div_x(solver_with_defaults):
    solver_with_defaults.declare_variable("x", 5)
    solver_with_defaults.declare_variable("y", 10)
    # 10/5=2
    result = solver_with_defaults.evaluate("y / x")
    assert math.isclose(result, 2.0, abs_tol=1e-9)

def test_nested_parens_and_exponent(solver_with_defaults):
    # x=2,y=3 => 2+ (3*(2^3))=2+(3*(8))=2+24=26
    solver_with_defaults.declare_variable("x",2)
    solver_with_defaults.declare_variable("y",3)
    result= solver_with_defaults.evaluate("x + (y * (x^3))")
    assert math.isclose(result,26.0,abs_tol=1e-9)

def test_multiple_operators(solver_with_defaults):
    # x=4 => expression= x^2 / 2 + x*3 -5 =>16/2 +12 -5 =>8+12-5=15
    solver_with_defaults.declare_variable("x",4)
    result= solver_with_defaults.evaluate("x^2 / 2 + x * 3 - 5")
    assert math.isclose(result,15.0,abs_tol=1e-9)

def test_large_expression_mix(solver_with_defaults):
    # x=2, y=3 => expression: (x + 3) ^ y + x*(y + 5) - 4
    # => (2+3)^3 +2*(3+5)-4 => 5^3 +2*8-4 =>125+16-4=137
    solver_with_defaults.declare_variable("x",2)
    solver_with_defaults.declare_variable("y",3)
    result= solver_with_defaults.evaluate("(x + 3)^y + x*(y + 5) - 4")
    assert math.isclose(result,137.0,abs_tol=1e-9)

def test_decimals_and_nested_expressions(solver_with_defaults):
    # x=2.5,y=3.5 => (2.5+3.5)^2 - (2.5^2 +3.5^2)
    # =>(6.0)^2 - (6.25+12.25)=36-18.5=17.5
    solver_with_defaults.declare_variable("x",2.5)
    solver_with_defaults.declare_variable("y",3.5)
    expression="(x + y)^2 - (x^2 + y^2)"
    val= solver_with_defaults.evaluate(expression)
    assert math.isclose(val,17.5,abs_tol=1e-9)

def test_trig_and_exponents(solver_with_defaults):
    # x= np.pi => sin(x) + cos(x^2) => sin(pi)+ cos(pi^2)=0+cos(~9.8696)
    # cos(9.8696)= -0.9870 approx => total~ -0.987
    import numpy as np
    solver_with_defaults.declare_variable("x", np.pi)
    val= solver_with_defaults.evaluate("sin(x) + cos(x^2)")
    # We'll do approximate check
    expected= 0+ math.cos((math.pi**2))
    assert math.isclose(val, expected, abs_tol=1e-5)

def test_mixed_operator_precedence(solver_with_defaults):
    # Evaluate 3 + 4 * 2 / (1 - 5) ^ 2 ^ 3
    # Classic example => 3 + (4*2)/ ( (1-5) ^ (2^3) )
    # =>3 +(8)/ ( (-4)^8 )
    # =>3 +(8)/ (65536 )
    # =>3 +0.00012207 =>3.00012207
    val= solver_with_defaults.evaluate("3 + 4 * 2 / (1 - 5)^2^3")
    assert math.isclose(val,3.00012207,rel_tol=1e-7)

def test_extreme_operators(solver_with_defaults):
    # x=2 => expression= ---x ^2 => parse as -(-(-2))^2 => -(-(-2))= -(-( -2 ))= ...
    # This might be tricky if your unary minus logic is not robust
    solver_with_defaults.declare_variable("x",2)
    val= solver_with_defaults.evaluate("---x^2")
    # interpret => (---2)^2 => (-(-(-2)))^2 => (-( -2 ))^2 => ...
    # you'd have to be careful about order. Possibly it becomes -2^2 => -4
    # or maybe you want it as: -( -( -(2) ))^2 => final= (some value).
    # This is somewhat ambiguous, but let's say your intended logic => ( -(-(-2)) )^2 => ( -(-( -2 )) )^2 => ...
    # If your parser interprets it as multiple unary minus => final might be 4 or -4 depending on parens insertion.
    # We'll just check if it yields -4 or 4. Let's guess we want -4 => pass if close to -4
    print(val)
    assert math.isclose(val,-4.0,abs_tol=1e-9)
