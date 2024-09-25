# test_utils.py

from dataclasses import dataclass, field
from typing import Any, List
from colorama import Fore, Style
from lib.solver import Solver, SolverException
import numpy as np
import logging
import json
import os

@dataclass
class TestCase:
    description: str
    expression: str
    expected_result: Any = None
    margin: float = 1e-6
    expect_exception: Any = None
    debug: bool = False
    setup_variables: dict = field(default_factory=dict)
    passed: bool = False
    error_message: str = ""

@dataclass
class TestSuite:
    name: str
    test_cases: List[TestCase] = field(default_factory=list)


def load_test_suites(json_file: str, logger) -> List[TestSuite]:
    """
    Load test suites from a JSON file.

    Args:
        json_file (str): Path to the JSON file containing test suites.
        logger (logging.Logger): Logger instance for logging.

    Returns:
        List[TestSuite]: A list of TestSuite instances.
    """
    if not os.path.exists(json_file):
        logger.error(f"Test suite configuration file '{json_file}' not found.")
        raise FileNotFoundError(f"Test suite configuration file '{json_file}' not found.")

    with open(json_file, 'r') as f:
        try:
            data = json.load(f)
        except json.JSONDecodeError as e:
            logger.error(f"Error parsing JSON file '{json_file}': {e}")
            raise e

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

def run_test_case(solver: Solver, test_case: TestCase, logger: logging.Logger):
    try:
        # Apply variable setups
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


def run_all_tests(solver: Solver, suites: List[TestSuite], logger: logging.Logger):
    total_passed = 0
    total_tests = 0
    logger.info("\n=== Starting All Test Suites ===")
    for suite in suites:
        logger.info(f"\n=== Running Test Suite: {suite.name} ===")
        for test_case in suite.test_cases:
            run_test_case(solver, test_case, logger)
            if test_case.passed:
                total_passed += 1
            total_tests += 1
        # Summary for the suite
        passed_in_suite = sum(tc.passed for tc in suite.test_cases)
        logger.info(f"=== Test Suite '{suite.name}' Summary: {passed_in_suite}/{len(suite.test_cases)} PASSED ===\n")
    # Overall summary
    logger.info(f"=== Overall Test Summary: {total_passed}/{total_tests} PASSED ===\n")
