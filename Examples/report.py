# report_utils.py

from data import TestSuite
# from colorama import Fore, Style
from typing import List

def print_report(suites: List[TestSuite]):
    print("\n=== Detailed Test Report ===\n")
    for suite in suites:
        print(f"Test Suite: {suite.name}")
        for test_case in suite.test_cases:
            status = f"PASSED" if test_case.passed else f"FAILED"
            print(f"  [{status}] {test_case.description}")
            if not test_case.passed:
                print(f"    Error: {test_case.error_message}")
        print("\n")
    # Calculate overall summary
    total_passed = sum(tc.passed for suite in suites for tc in suite.test_cases)
    total_tests = sum(len(suite.test_cases) for suite in suites)
    print(f"=== Overall Test Summary: {total_passed}/{total_tests} PASSED ===\n")
