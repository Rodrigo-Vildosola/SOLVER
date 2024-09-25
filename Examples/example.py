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
        {"description": "Evaluate --5 (double unary minus)", "expression": "--5", "expected_result": 5.0},
        {"description": "Evaluate -(x + 5)", "expression": "-(x + 5)", "expected_result": -15},  
        {"description": "Evaluate -(3 + 2)", "expression": "-(3 + 2)", "expected_result": -5.0},
        {"description": "Evaluate -(f(3))", "expression": "-(f(3))", "expected_result": -16.0},  # f(3) = 16
        {"description": "Evaluate -(2 * 3) + 5", "expression": "-(2 * 3) + 5", "expected_result": -1.0},
        {"description": "Evaluate -g(2, 3)", "expression": "-g(2, 3)", "expected_result": -11.0},  # g(2, 3) = 11
        {"description": "Evaluate --g(1, 4)", "expression": "--g(1, 4)", "expected_result": 9.0},  # g(1, 4) = 10

        # Additional stress tests for unary minus
        {"description": "Evaluate ---5 (triple unary minus)", "expression": "---5", "expected_result": -5.0},
        {"description": "Evaluate ----5 (quadruple unary minus)", "expression": "----5", "expected_result": 5.0},
        {"description": "Evaluate -(-(-(-5)))", "expression": "-(-(-(-5)))", "expected_result": 5.0},
        {"description": "Evaluate -(-(x + 5))", "expression": "-(-(x + 5))", "expected_result": 15.0},
        {"description": "Evaluate -(-(f(3)))", "expression": "-(-(f(3)))", "expected_result": 16.0},  # f(3) = 16
        {"description": "Evaluate -(-g(1, 4))", "expression": "-(-g(1, 4))", "expected_result": 9.0},  # g(1, 4) = 10
        {"description": "Evaluate -(2 + -(3))", "expression": "-(2 + -(3))", "expected_result": 1.0},
        {"description": "Evaluate -(f(-g(1, 2)))", "expression": "-(f(-g(1, 2)))", "expected_result": -16.0},  # g(1,2) = 5, f(-5) = 1
        {"description": "Evaluate -(f(--g(1, 2)))", "expression": "-(f(--g(1, 2)))", "expected_result": -36.0},  # g(1,2) = 5, f(5) = 36
        {"description": "Evaluate --(2 * -3)", "expression": "--(2 * -3)", "expected_result": -6.0},
        {"description": "Evaluate --(x + -5)", "expression": "--(x + -5)", "expected_result": 5.0},  # x = 10
        {"description": "Evaluate -(-2 * 3 + -5)", "expression": "-(-2 * 3 + -5)", "expected_result": 11.0},
        {"description": "Evaluate --(--(--5))", "expression": "--(--(--5))", "expected_result": 5.0},
        {"description": "Evaluate ----(--5)", "expression": "----(--5)", "expected_result": 5.0},
        {"description": "Evaluate --f(-g(2, 3))", "expression": "--f(-g(2, 3))", "expected_result": 100.0},  # g(2,3) = 11, f(-11) = 1
        {"description": "Evaluate -(f(2) + -g(3, 4))", "expression": "-(f(2) + -g(3, 4))", "expected_result": 10.0},  # f(2)=9, g(3,4)=19, -(9 + -19) = 10
        {"description": "Evaluate -2^2", "expression": "-2^2", "expected_result": -4.0},  # Testing negation with power

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
                logger.info(f"{Fore.GREEN}PASSED{Style.RESET_ALL} - {description}: {expression} = {result}")
                passed_count += 1
            else:
                logger.error(f"{Fore.RED}FAILED{Style.RESET_ALL} - {description}: Expected {expected}, got {result}")
                failed_count += 1
        except SolverException as e:
            logger.error(f"{Fore.RED}FAILED{Style.RESET_ALL} - {description}: Exception occurred: {e}")
            failed_count += 1

    # Optionally, print a summary or detailed report
    logger.info(f"=== Example Execution Summary ===")
    logger.info(f"Total Tests: {passed_count + failed_count}")
    logger.info(f"Passed: {passed_count}")
    logger.info(f"Failed: {failed_count}")
    logger.info("=== End of Execution ===")

if __name__ == "__main__":
    run_examples()
