# example_script.py

from logger import Logger
from lib.solver import Solver, SolverException
import numpy as np
from colorama import Fore, Style
import sys

def run_examples():
    # Initialize a separate logger for examples
    example_logger_instance = Logger("ExampleLogger", "example_results.log")
    logger = example_logger_instance.get_logger()

    # Initialize solver and declare functions
    solver = Solver()
    try:
        # Declare constants
        solver.declareConstant("pi", np.pi)
        solver.declareConstant("e", np.e)

        solver.declareVariable("x", 10)

        # Declare user-defined functions
        solver.declareFunction("f", ["x"], "x^2 + 2*x + 1")
        solver.declareFunction("g", ["x", "y"], "x * y + x + y")
        solver.declareFunction("h", ["x"], "f(g(x, x))")
        solver.declareFunction("x", ["x"], "x + 1")
        # Add more function declarations as needed
    except SolverException as e:
        logger.error(f"Initialization Error: {e}")
        sys.exit(1)

    # Example expressions to evaluate
    examples = [
        {"description": "Evaluate f(4)", "expression": "f(4)", "expected_result": 25.0},
        {"description": "Evaluate g(5, 5)", "expression": "g(5, 5)", "expected_result": 35.0},
        {"description": "Evaluate h(2)", "expression": "h(2)", "expected_result": 81.0},

        {"description": "Evaluate -2 + 2", "expression": "-2 + 2", "expected_result": 0.0},
        {"description": "Evaluate -5", "expression": "-5", "expected_result": -5.0},
        {"description": "Evaluate x(1) + x", "expression": "x(1) + x", "expected_result": 12.0},  # Testing negation with power

    ]

    passed_count = 0
    failed_count = 0

    # Run examples
    for example in examples:
        description = example["description"]
        expression = example["expression"]
        expected = example["expected_result"]

        try:
            result = solver.evaluate(expression)
            if np.isclose(result, expected, atol=1e-6):
                logger.info(f"{Fore.GREEN}PASSED{Style.RESET_ALL} - {description}: {expression} = {result}")
                passed_count += 1
            else:
                logger.error(f"{Fore.RED}FAILED{Style.RESET_ALL} - {description}: Expected {expected}, got {result}")
                failed_count += 1
        except SolverException as e:
            logger.error(f"{Fore.RED}FAILED{Style.RESET_ALL} - {description}: Exception occurred: {e}")
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
