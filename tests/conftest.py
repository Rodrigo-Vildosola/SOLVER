# tests/conftest.py

import pytest
import numpy as np
from solver import Solver, SolverException

@pytest.fixture
def solver_with_defaults():
    """
    Returns a Solver instance with default constants and standard user-defined functions
    (like f, g, h, etc.) declared if you wish to keep them for all tests.
    """
    s = Solver()
    # Declare standard constants
    s.declare_constant("pi", np.pi)
    s.declare_constant("e", np.e)
    
    # Optionally declare any user-defined functions you want in every suite
    # For instance:
    s.declare_function("f", ["x"], "x^2 + 2*x + 1")
    s.declare_function("g", ["x", "y"], "x * y + x + y")
    s.declare_function("h", ["x"], "f(g(x, x))")
    s.declare_function("k", ["x"], "f(x) + g(x, x)")
    s.declare_function("m", ["x", "y"], "h(x) + f(g(x, y))")
    s.declare_function("p", ["x", "y"], "m(x, y) + k(x)")
    # Maybe we also define circle_area:
    s.declare_function("circle_area", ["r"], "pi * r^2")
    
    return s
