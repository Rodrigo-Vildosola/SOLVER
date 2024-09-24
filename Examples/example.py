from lib.solver import Solver, SolverException
import matplotlib.pyplot as plt
import numpy as np
import logging
import sys
import colorama
from colorama import Fore, Style

# Initialize colorama
colorama.init(autoreset=True)

# Initialize solver
solver = Solver()

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(sys.stdout),
        logging.FileHandler('test_results.log')
    ]
)

# Create a logger for this module
logger = logging.getLogger(__name__)

# Define a TestCase dataclass for better structure
from dataclasses import dataclass, field
from typing import Callable, Any, List

@dataclass
class TestCase:
    description: str
    expression: str
    expected_result: Any = None
    margin: float = 1e-6
    expect_exception: Any = None
    debug: bool = False
    passed: bool = False
    error_message: str = ""

@dataclass
class TestSuite:
    name: str
    test_cases: List[TestCase] = field(default_factory=list)

# Define test suites
def create_test_suites():
    suites = []

    # Test Suite 1: Variables
    variables_suite = TestSuite(name="Variables")
    variables_suite.test_cases.extend([
        TestCase(description="x + 0", expression="x + 0", expected_result=5.0),
        TestCase(description="x + y", expression="x + y", expected_result=15.0),
    ])
    suites.append(variables_suite)

    # Test Suite 2: Complex Expressions
    complex_suite = TestSuite(name="Complex Expressions")
    complex_suite.test_cases.extend([
        TestCase(description="x * y + x^2", expression="x * y + x^2", expected_result=75.0),
        TestCase(description="y / x", expression="y / x", expected_result=2.0),
    ])
    suites.append(complex_suite)

    # Test Suite 3: Error Handling
    error_suite = TestSuite(name="Error Handling")
    error_suite.test_cases.extend([
        TestCase(description="Division by zero", expression="x / (y - 10)", expect_exception=SolverException),
        TestCase(description="Undefined function call", expression="undefinedFunc(5)", expect_exception=SolverException),
        TestCase(description="Incorrect number of function arguments", expression="f(1, 2)", expect_exception=SolverException),
        TestCase(description="Function with undefined variable 'z'", expression="n(5)", expect_exception=SolverException),
    ])
    suites.append(error_suite)

    # Test Suite 4: Custom Functions
    custom_functions_suite = TestSuite(name="Custom Functions")
    custom_functions_suite.test_cases.extend([
        TestCase(description="f(3)", expression="f(3)", expected_result=16.0),
        TestCase(description="f(-1)", expression="f(-1)", expected_result=0.0),
        TestCase(description="g(3, 4)", expression="g(3, 4)", expected_result=19.0),
        TestCase(description="g(-2, 2) + 1", expression="g(-2, 2) + 1", expected_result=-3.0),
        TestCase(description="h(2)", expression="h(2)", expected_result=1089.0),
        TestCase(description="k(3)", expression="k(3)", expected_result=31.0),
        TestCase(description="m(2, 10)", expression="m(2, 10)", expected_result=1187.0),
        TestCase(description="p(2, 10)", expression="p(2, 10)", expected_result=1201.0),
        TestCase(description="n(5)", expression="n(5)", expect_exception=SolverException),
    ])
    suites.append(custom_functions_suite)

    # Test Suite 5: Built-in Functions and Constants
    built_in_suite = TestSuite(name="Built-in Functions and Constants")
    built_in_suite.test_cases.extend([
        TestCase(description="sin(pi / 2)", expression="sin(pi / 2)", expected_result=1.0),
        TestCase(description="cos(0)", expression="cos(0)", expected_result=1.0),
        TestCase(description="tan(pi / 4)", expression="tan(pi / 4)", expected_result=1.0),
        TestCase(description="exp(1)", expression="exp(1)", expected_result=np.e),
        TestCase(description="ln(e)", expression="ln(e)", expected_result=1.0),
        TestCase(description="sqrt(16)", expression="sqrt(16)", expected_result=4.0),
        TestCase(description="sin(pi / 4) + cos(pi / 4)", expression="sin(pi / 4) + cos(pi / 4)", expected_result=np.sqrt(2), margin=1e-5),
        TestCase(description="ln(e^2)", expression="ln(e^2)", expected_result=2.0),
        TestCase(description="exp(ln(5))", expression="exp(ln(5))", expected_result=5.0),
        TestCase(description="circlearea(3)", expression="circlearea(3)", expected_result=28.274333882308138),
        TestCase(description="sqrt(sin(pi / 2))", expression="sqrt(sin(pi / 2))", expected_result=1.0),
        TestCase(description="ln(exp(5))", expression="ln(exp(5))", expected_result=5.0),
        TestCase(description="f(2.5)", expression="f(2.5)", expected_result=12.25),
    ])
    suites.append(built_in_suite)

    return suites

# Utility function for evaluating and printing results
def test_expression(test_case: TestCase):
    try:
        result = solver.evaluate(test_case.expression, test_case.debug)
        if test_case.expect_exception:
            # If an exception was expected but none was raised
            test_case.passed = False
            test_case.error_message = f"Expected exception {test_case.expect_exception.__name__} but got result {result}."
            logger.error(f"{Fore.RED}FAILED{Style.RESET_ALL} - {test_case.description}: {test_case.error_message}")
        else:
            if isinstance(test_case.expected_result, Exception):
                # If an exception was expected, but none was raised
                test_case.passed = False
                test_case.error_message = f"Expected exception {test_case.expected_result.__name__} but got result {result}."
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
            logger.info(f"{Fore.GREEN}PASSED{Style.RESET_ALL} - {test_case.description}: Expected exception {e}")
        else:
            test_case.passed = False
            expected = test_case.expect_exception.__name__ if test_case.expect_exception else "No exception"
            test_case.error_message = f"Expected {expected}, got exception '{e}'."
            logger.error(f"{Fore.RED}FAILED{Style.RESET_ALL} - {test_case.description}: {test_case.error_message}")

# Function to run a test suite
def run_test_suite(suite: TestSuite):
    logger.info(f"\n=== Running Test Suite: {suite.name} ===")
    for test_case in suite.test_cases:
        test_expression(test_case)
    # Summary for the suite
    passed = sum(tc.passed for tc in suite.test_cases)
    total = len(suite.test_cases)
    logger.info(f"=== Test Suite '{suite.name}' Summary: {passed}/{total} PASSED ===\n")

# Function to display the main menu
def display_menu(suites: List[TestSuite]):
    while True:
        print("\n=== Solver Testing Menu ===")
        print("Select a test suite to run:")
        for idx, suite in enumerate(suites, start=1):
            print(f"{idx}. {suite.name}")
        print(f"{len(suites)+1}. Run All Test Suites")
        print(f"{len(suites)+2}. Exit")
        
        choice = input("Enter your choice (number): ")
        
        if not choice.isdigit():
            print(f"{Fore.YELLOW}Invalid input. Please enter a number.{Style.RESET_ALL}")
            continue
        
        choice = int(choice)
        
        if 1 <= choice <= len(suites):
            run_test_suite(suites[choice-1])
        elif choice == len(suites)+1:
            for suite in suites:
                run_test_suite(suite)
            # After running all, display overall summary
            display_overall_summary(suites)
        elif choice == len(suites)+2:
            print("Exiting the testing program.")
            break
        else:
            print(f"{Fore.YELLOW}Invalid choice. Please select a valid option.{Style.RESET_ALL}")

# Function to display overall summary
def display_overall_summary(suites: List[TestSuite]):
    total_passed = 0
    total_tests = 0
    for suite in suites:
        passed = sum(tc.passed for tc in suite.test_cases)
        total_passed += passed
        total_tests += len(suite.test_cases)
    logger.info(f"\n=== Overall Test Summary: {total_passed}/{total_tests} PASSED ===\n")

# Function to initialize and prepare the testing environment
def initialize_tests():
    try:
        # Declare constants
        solver.declareConstant("pi", np.pi)
        solver.declareConstant("e", np.e)

        # Declare global variables
        solver.declareVariable("x", 5)   # x
        solver.declareVariable("y", 10)   # y

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

# Function to run all tests without menu (optional)
def run_all_tests(suites: List[TestSuite]):
    for suite in suites:
        run_test_suite(suite)
    display_overall_summary(suites)

# Function to create colored logs in the console
def setup_colored_logging():
    pass  # Already handled by colorama in test_expression

# Main function
def main():
    initialize_tests()
    suites = create_test_suites()
    display_menu(suites)

# Entry point
if __name__ == "__main__":
    main()
