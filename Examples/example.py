from lib.solver import Solver, SolverException
import numpy as np
import logging
import sys
import colorama
from colorama import Fore, Style
from typing import List

from logger import Logger
from data import TestCase, TestSuite


colorama.init(autoreset=True)

solver = Solver()

logger = Logger("Example", "example_results.log").get_logger()

# Define test suites and test cases directly in code
def define_test_suites() -> List[TestSuite]:
    suites = []

    # Test Suite 2: Error Handling
    first_suite = TestSuite(name="Example testing")
    first_suite.test_cases.extend([
        TestCase(
            description="Division by Zero",
            expression="x / (y - 10)",
            expect_exception=SolverException,
            setup_variables={"x": 5, "y": 10}
        ),
        TestCase(
            description="Undefined Function Call",
            expression="undefinedFunc(5)",
            expect_exception=SolverException
        ),
        TestCase(
            description="Incorrect Number of Function Arguments",
            expression="f(1, 2)",
            expect_exception=SolverException
        ),
        TestCase(
            description="Function with Undefined Variable 'z'",
            expression="n(5)",
            expect_exception=SolverException
        ),
    ])
    suites.append(first_suite)


    return suites

# Function to run a single test case
def run_test_case(test_case: TestCase):
    try:
        for var, value in test_case.setup_variables.items():
            # Set new variable value
            if var in ['x', 'y']:  # Assuming 'x' and 'y' are global variables
                solver.declareVariable(var, value)
            else:
                solver.declareConstant(var, value)

        # Evaluate the expression
        result = solver.evaluate(test_case.expression, test_case.debug)

        if test_case.expect_exception:
            # If an exception was expected but none was raised
            test_case.passed = False
            test_case.error_message = f"Expected exception {test_case.expect_exception.__name__} but got result {result}."
            logger.error(f"{Fore.RED}FAILED{Style.RESET_ALL} - {test_case.description}: {test_case.error_message}")
        else:
            if isinstance(test_case.expected_result, float) and np.isnan(test_case.expected_result):
                test_case.passed = np.isnan(result)
            else:
                test_case.passed = np.isclose(result, test_case.expected_result, atol=test_case.margin)

            if test_case.passed:
                logger.info(f"{Fore.GREEN}PASSED{Style.RESET_ALL} - {test_case.description}: {test_case.expression} = {result}")
            else:
                test_case.error_message = f"Expected {test_case.expected_result}, got {result}."
                logger.error(f"{Fore.RED}FAILED{Style.RESET_ALL} - {test_case.description}: {test_case.error_message}")

    except SolverException as e:
        if test_case.expect_exception and isinstance(e, test_case.expect_exception):
            test_case.passed = True
            logger.info(f"{Fore.GREEN}PASSED{Style.RESET_ALL} - {test_case.description}: Expected exception '{e}'")
        else:
            test_case.passed = False
            expected = test_case.expect_exception.__name__ if test_case.expect_exception else "No exception"
            test_case.error_message = f"Expected {expected}, got exception '{e}'."
            logger.error(f"{Fore.RED}FAILED{Style.RESET_ALL} - {test_case.description}: {test_case.error_message}")


# Function to run all test suites and collect results
def run_all_tests(suites: List[TestSuite]):
    total_passed = 0
    total_tests = 0
    logger.info("\n=== Starting All Test Suites ===")
    for suite in suites:
        logger.info(f"\n=== Running Test Suite: {suite.name} ===")
        for test_case in suite.test_cases:
            run_test_case(test_case)
            if test_case.passed:
                total_passed += 1
            total_tests += 1
        # Summary for the suite
        passed_in_suite = sum(tc.passed for tc in suite.test_cases)
        logger.info(f"=== Test Suite '{suite.name}' Summary: {passed_in_suite}/{len(suite.test_cases)} PASSED ===\n")
    # Overall summary
    logger.info(f"=== Overall Test Summary: {total_passed}/{total_tests} PASSED ===\n")

# Function to initialize and prepare the testing environment
def initialize_tests():
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

# Function to print a detailed report of the test results
def print_report(suites: List[TestSuite]):
    print("\n=== Detailed Test Report ===\n")
    for suite in suites:
        print(f"Test Suite: {suite.name}")
        for test_case in suite.test_cases:
            status = f"{Fore.GREEN}PASSED{Style.RESET_ALL}" if test_case.passed else f"{Fore.RED}FAILED{Style.RESET_ALL}"
            print(f"  [{status}] {test_case.description}")
            if not test_case.passed:
                print(f"    Error: {test_case.error_message}")
        print("\n")
    # Calculate overall summary
    total_passed = sum(tc.passed for suite in suites for tc in suite.test_cases)
    total_tests = sum(len(suite.test_cases) for suite in suites)
    print(f"=== Overall Test Summary: {total_passed}/{total_tests} PASSED ===\n")

# Main function to execute the testing script
def main():
    initialize_tests()
    suites = define_test_suites()
    run_all_tests(suites)
    print_report(suites)

# Entry point
if __name__ == "__main__":
    main()
