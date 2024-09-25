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

        # Declare user-defined functions
        solver.declareFunction("f", ["x"], "x^2 + 2*x + 1")
        solver.declareFunction("g", ["x", "y"], "x * y + x + y")
        solver.declareFunction("h", ["x"], "f(g(x, x))")
        # Add more function declarations as needed
    except SolverException as e:
        logger.error(f"Initialization Error: {e}")
        sys.exit(1)

    # Example expressions to evaluate
    examples = [
        {"description": "Evaluate f(4)", "expression": "f(4)", "expected_result": 25.0},
        {"description": "Evaluate g(5, 5)", "expression": "g(5, 5)", "expected_result": 35.0},
        {"description": "Evaluate h(2)", "expression": "h(2)", "expected_result": 81.0},
    ]

    # Run examples
    for example in examples:
        description = example["description"]
        expression = example["expression"]
        expected = example["expected_result"]

        try:
            result = solver.evaluate(expression)
            if np.isclose(result, expected, atol=1e-6):
                logger.info(f"{Fore.GREEN}PASSED{Style.RESET_ALL} - {description}: {expression} = {result}")
            else:
                logger.error(f"{Fore.RED}FAILED{Style.RESET_ALL} - {description}: Expected {expected}, got {result}")
        except SolverException as e:
            logger.error(f"{Fore.RED}FAILED{Style.RESET_ALL} - {description}: Exception occurred: {e}")

    # Optionally, print a summary or detailed report
    print("\n=== Example Execution Summary ===")
    for example in examples:
        description = example["description"]
        # Assuming you have a way to track pass/fail per example
        # This implementation logs to files, so you might parse the log or maintain counters
    print("=== Check 'example_results.log' for detailed results ===\n")

if __name__ == "__main__":
    run_examples()
