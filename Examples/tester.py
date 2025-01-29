# test_script.py
from data import run_all_tests, load_test_suites
from solver import Solver, SolverException
import numpy as np
import logging
import sys

class Logger:
    def __init__(self, logger_name: str):
        self.logger = logging.getLogger(logger_name)
        self.logger.setLevel(logging.INFO)

        # Prevent adding multiple handlers to the same logger
        if not self.logger.handlers:
            # Console Handler
            console_handler = logging.StreamHandler(sys.stdout)
            console_handler.setLevel(logging.INFO)
            console_formatter = logging.Formatter('%(levelname)s - %(message)s')
            console_handler.setFormatter(console_formatter)
            self.logger.addHandler(console_handler)

    def get_logger(self) -> logging.Logger:
        return self.logger

def main():
    # Initialize a separate logger for testing
    logger = Logger("TestLogger").get_logger()

    # Initialize solver and declare functions
    solver = Solver()
    try:
        # Declare constants
        solver.declare_constant("pi", np.pi)
        solver.declare_constant("e", np.e)

        # Declare user-defined functions
        solver.declare_function("f", ["x"], "x^2 + 2*x + 1")
        solver.declare_function("g", ["x", "y"], "x * y + x + y")
        solver.declare_function("h", ["x"], "f(g(x, x))")
        solver.declare_function("k", ["x"], "f(x) + g(x, x)")
        solver.declare_function("m", ["x", "y"], "h(x) + f(g(x, y))")
        solver.declare_function("p", ["x", "y"], "m(x, y) + k(x)")
        solver.declare_function("n", ["x"], "x + z") 
        solver.declare_function("circle_area", ["r"], "pi * r^2")
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
