# example_script.py

import logging
from solver import Solver, SolverException
import numpy as np
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

def run_examples():
    logger = Logger("SimpleExpressions").get_logger()

    solver = Solver()
    try:
        # Declare constants
        solver.declare_constant("pi", np.pi)
        solver.declare_constant("e", np.e)
        solver.declare_constant("b", 2)
        solver.declare_constant("c", 5)


        solver.declare_variable("x", 10)

        solver.declare_function("f", ["x"], "x^2 + 2*x + 1")
        solver.declare_function("g", ["x", "y"], "x * y + x + y")
        solver.declare_function("h", ["x"], "f(g(x, x))")
        solver.declare_function("x", ["x"], "x + 1")
        solver.declare_function("why", [], "2 + 3")
        # Add more function declarations as needed
    except SolverException as e:
        logger.error(f"Initialization Error: {e}")
        sys.exit(1)

    # Example expressions to evaluate
    examples = [
        {"description": "Evaluate h(2)", "expression": "h(2)", "expected_result": 81.0},
        {"description": "Evaluate why()", "expression": "why()", "expected_result": 5},
        {"description": "Evaluate g(5, 5)", "expression": "g(f(2), 5)", "expected_result": 59.0},
        {"description": "Evaluate x + 0", "expression": "x + 0", "expected_result": 10},
        {"description": "Evaluate x * 1", "expression": "x * 1", "expected_result": 10},
        {"description": "Evaluate f(4)", "expression": "f(4)", "expected_result": 25.0},
        {"description": "Evaluate (b + 3) * c", "expression": "(b + 3) * (c + 0)", "expected_result": 25.0},
        {"description": "Evaluate 3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3", "expression": "3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3", "expected_result": 3.0001},
        {"description": "Evaluate sin(pi/2)", "expression": "sin(pi/2)", "expected_result": 1},
        {"description": "Evaluate f(g(2, 2) + sin(pi/2))", "expression": "f(g(2, 2) + sin(pi/2))", "expected_result": 100},
        {"description": "Evaluate x + 2 * 4 + 3", "expression": "x + 2 * 4 + 3", "expected_result": 21},

    ]

    passed_count = 0
    failed_count = 0

    # Run examples
    for example in examples:
        description = example["description"]
        expression = example["expression"]
        expected = example["expected_result"]

        try:
            result = solver.evaluate(expression, True)
            if np.isclose(result, expected, atol=1e-6):
                logger.info(f"PASSED - {description}: {expression} = {result}")
                passed_count += 1
            else:
                logger.error(f"FAILED - {description}: Expected {expected}, got {result}")
                failed_count += 1
        except SolverException as e:
            logger.error(f"FAILED - {description}: Exception occurred: {e}")
            failed_count += 1

    print()

    # Optionally, print a summary or detailed report
    logger.info(f"=== Example Execution Summary ===")
    logger.info(f"Total Tests: {passed_count + failed_count}")
    logger.info(f"Passed: {passed_count}")
    logger.info(f"Failed: {failed_count}")
    logger.info("=== End of Execution ===")

if __name__ == "__main__":
    run_examples()
