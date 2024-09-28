# test_script.py
from data import run_all_tests, load_test_suites
from report import print_report
from logger import Logger
from solver import Solver, SolverException
import numpy as np
import sys


def main():
    # Initialize a separate logger for testing
    test_logger_instance = Logger("TestLogger", "test_results.log")
    logger = test_logger_instance.get_logger()

    # Initialize solver and declare functions
    solver = Solver()
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
        solver.declareFunction("n", ["x"], "x + z") 
        solver.declareFunction("circle_area", ["r"], "pi * r^2")
    except SolverException as e:
        logger.error(f"Initialization Error: {e}")
        sys.exit(1)

    # Load test suites from JSON file
    try:
        suites = load_test_suites("test_suites.json", logger)
    except Exception as e:
        logger.error(f"Failed to load test suites: {e}")
        sys.exit(1)

    # Run all tests
    test_result = run_all_tests(solver, suites, logger)

    # If test_result indicates a failure, exit with non-zero code
    if not test_result:
        logger.error("Tests failed.")
        sys.exit(1)

    # Optionally, print detailed report
    # print_report(suites)

if __name__ == "__main__":
    main()
