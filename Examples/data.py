
from dataclasses import dataclass, field
from typing import Any, List
import sys
import numpy as np
from lib.solver import SolverException

@dataclass
class TestCase:
    description: str
    expression: str
    expected_result: Any = None
    margin: float = 1e-6
    expect_exception: Any = None
    debug: bool = False
    setup_variables: dict = field(default_factory=dict) 
    passed: bool = False
    error_message: str = ""

@dataclass
class TestSuite:
    name: str
    test_cases: List[TestCase] = field(default_factory=list)


# Function to initialize and prepare the testing environment
def initialize_tests(solver, logger):
    try:
        # Declare constants
        solver.declareConstant("pi", np.pi)
        solver.declareConstant("e", np.e)

        # Declare user-defined functions
        solver.declareFunction("f", ["x"], "x^2 + 2*x + 1")
        solver.declareFunction("g", ["x", "y"], "x * y + x + y")
        solver.declareFunction("h", ["x"], "f(g(x, x))")
        solver.declareFunction("k", ["x"], "f(x) + g(x, x)")
        solver.declareFunction("m", ["x", "y"], "h(x) + f(g(x, y))")
        solver.declareFunction("p", ["x", "y"], "m(x, y) + k(x)")
        solver.declareFunction("n", ["x"], "x + z")  # This function uses undefined variable 'z'
        solver.declareFunction("circlearea", ["r"], "pi * r^2")
    except SolverException as e:
        logger.error(f"Initialization Error: {e}")
        sys.exit(1)
