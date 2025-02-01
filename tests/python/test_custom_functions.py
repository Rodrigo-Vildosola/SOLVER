# tests/test_custom_functions.py
import pytest
import math
from solver import SolverException

def test_f_3(solver_with_defaults):
    # f(3) => 3^2 + 2*3 +1= 9+6+1=16
    result = solver_with_defaults.evaluate("f(3)")
    assert math.isclose(result, 16.0, abs_tol=1e-9)

def test_f_negative(solver_with_defaults):
    # f(-1) => (-1)^2 + 2*(-1)+1=1-2+1=0
    result = solver_with_defaults.evaluate("f(-1)")
    assert math.isclose(result, 0.0, abs_tol=1e-9)

def test_g_3_4(solver_with_defaults):
    # g(3,4)=>3*4+3+4=12+3+4=19
    result = solver_with_defaults.evaluate("g(3,4)")
    assert math.isclose(result, 19.0, abs_tol=1e-9)

def test_g_negative2_2_plus1(solver_with_defaults):
    # g(-2,2)+1 => [(-2)*2 + -2+2] +1= (-4-2+2)+1= -4 => -4
    val= solver_with_defaults.evaluate("g(-2,2)+1")
    assert math.isclose(val, -3.0, abs_tol=1e-9)

def test_h_2(solver_with_defaults):
    # h(2)=> f(g(2,2))
    # g(2,2)=2*2+2+2=4+2+2=8 => f(8)= 8^2+2*8+1=64+16+1=81
    val= solver_with_defaults.evaluate("h(2)")
    assert math.isclose(val,81.0, abs_tol=1e-9)

def test_k_3(solver_with_defaults):
    # k(3)= f(3)+ g(3,3)=16+(3*3+3+3)=16+(9+3+3)=31
    val= solver_with_defaults.evaluate("k(3)")
    assert math.isclose(val,31.0, abs_tol=1e-9)

def test_m_2_10(solver_with_defaults):
    # m(x,y)= h(x)+ f(g(x,y))
    # h(2)=81, g(2,10)=2*10+2+10=20+2+10=32 => f(32)= 32^2+64+1=1024+64+1=1089 => total=81+1089=1170
    val= solver_with_defaults.evaluate("m(2,10)")
    assert math.isclose(val,1170.0, abs_tol=1e-9)

def test_p_2_10(solver_with_defaults):
    # p(x,y)= m(x,y)+k(x)=> 1170+( f(2)+g(2,2) )
    # f(2)=2^2+4+1=9, g(2,2)=8 => total= 9+8=17 => 1170+17=1187
    val= solver_with_defaults.evaluate("p(2,10)")
    assert math.isclose(val,1187.0, abs_tol=1e-9)

def test_n_5(solver_with_defaults):
    # if we do n(5)-> we haven't declared n(x)=..., or if we do it's x+z => fails
    # If you do: solver_with_defaults.declare_function("n", ["x"], "x + z")
    with pytest.raises(SolverException):
        solver_with_defaults.evaluate("n(5)")
