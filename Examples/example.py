from lib.solver import Solver, SolverException
import matplotlib.pyplot as plt
import numpy as np
import logging
import sys
import colorama
from colorama import Fore, Style
import json
import os

# Initialize colorama
colorama.init(autoreset=True)

# Initialize solver
solver = Solver()

# Configure logging
# Set FileHandler to 'w' mode to reset the log file on initialization
logging.basicConfig(
    level=logging.INFO,
    format='%(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(sys.stdout),
        logging.FileHandler('test_results.log', mode='w')  # Overwrite the log file each run
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
    setup_variables: dict = field(default_factory=dict)  # New field
    passed: bool = False
    error_message: str = ""

@dataclass
class TestSuite:
    name: str
    test_cases: List[TestCase] = field(default_factory=list)

# Function to load test suites from a JSON file
def load_test_suites(json_file: str) -> List[TestSuite]:
    if not os.path.exists(json_file):
        logger.error(f"Test suite configuration file '{json_file}' not found.")
        sys.exit(1)
    
    with open(json_file, 'r') as f:
        try:
            data = json.load(f)
        except json.JSONDecodeError as e:
            logger.error(f"Error parsing JSON file '{json_file}': {e}")
            sys.exit(1)
    
    suites = []
    for suite_data in data.get("suites", []):
        suite = TestSuite(name=suite_data.get("name", "Unnamed Suite"))
        for tc_data in suite_data.get("test_cases", []):
            # Convert 'expect_exception' from string to actual exception class
            expect_exc = tc_data.get("expect_exception")
            if expect_exc:
                if expect_exc == "SolverException":
                    expect_exc = SolverException
                else:
                    logger.warning(f"Unknown exception '{expect_exc}' in test case '{tc_data.get('description', '')}'. Skipping exception mapping.")
                    expect_exc = None
            test_case = TestCase(
                description=tc_data.get("description", "No Description"),
                expression=tc_data.get("expression", ""),
                expected_result=tc_data.get("expected_result"),
                margin=tc_data.get("margin", 1e-6),
                expect_exception=expect_exc,
                debug=tc_data.get("debug", False),
                setup_variables=tc_data.get("setup_variables", {})
            )
            suite.test_cases.append(test_case)
        suites.append(suite)
    return suites

# Utility function for evaluating and printing results
def test_expression(test_case: TestCase):
    try:
        # Apply variable setups
        previous_variables = {}
        for var, value in test_case.setup_variables.items():
            # Set new variable value
            if var in ['x', 'y']:  # Assuming 'x' and 'y' are global variables
                solver.declareVariable(var, value)
            else:
                solver.declareConstant(var, value)
        
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
            logger.info(f"{Fore.GREEN}PASSED{Style.RESET_ALL} - {test_case.description}: Expected exception '{e}'")
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

# Main function
def main():
    initialize_tests()
    suites = load_test_suites("test_suites.json")  # Ensure the JSON file is in the same directory
    display_menu(suites)

# Entry point
if __name__ == "__main__":
    main()
