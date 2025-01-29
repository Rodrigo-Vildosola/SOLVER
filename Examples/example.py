# example_script.py

from logger import Logger
from solver import Solver, SolverException
import numpy as np
import sys

def run_examples():
    example_logger_instance = Logger("ExampleLogger", "example_results.log")
    logger = example_logger_instance.get_logger()

    solver = Solver()
    try:
        # Declare constants
        solver.declareConstant("pi", np.pi)
        solver.declareConstant("e", np.e)
        solver.declareConstant("b", 2)
        solver.declareConstant("c", 5)


        solver.declareVariable("x", 10)

        # Declare user-defined functions
        solver.declareFunction("f", ["x"], "x^2 + 2*x + 1")
        solver.declareFunction("g", ["x", "y"], "x * y + x + y")
        solver.declareFunction("h", ["x"], "f(g(x, x))")
        solver.declareFunction("x", ["x"], "x + 1")
        solver.declareFunction("why", [], "2 + 3")
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
