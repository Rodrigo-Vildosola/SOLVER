from lib.solver import Solver, SolverException
import matplotlib.pyplot as plt
import numpy as np
import logging
import sys



# Initialize solver
solver = Solver()

# Utility function for evaluating and printing results
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

def test_expression(expression, expected_result=None, margin=1e-6, debug=False):
    try:
        result = solver.evaluate(expression, debug)
        if expected_result is not None:
            if isinstance(expected_result, Exception):
                # If an exception was expected, but none was raised
                logger.error(f"Evaluating: {expression} -> Result: {result} | Expected Exception: {expected_result.__name__} | FAILED (No Exception Raised)")
            else:
                if np.isnan(expected_result) and np.isnan(result):
                    test_passed = True
                else:
                    test_passed = np.isclose(result, expected_result, atol=margin)
                status = "PASSED" if test_passed else "FAILED"
                logger.info(f"Evaluating: {expression} -> Result: {result} | Expected: {expected_result} | {status}")
        else:
            logger.info(f"Evaluating: {expression} -> Result: {result}")
    except SolverException as e:
        if expected_result is not None and isinstance(expected_result, type) and issubclass(expected_result, Exception):
            if isinstance(e, expected_result):
                status = "PASSED (Expected Exception)"
            else:
                status = f"FAILED (Unexpected Exception: {e})"
        else:
            status = f"FAILED (Exception: {e})"
        logger.error(f"Error evaluating {expression}: {e} | {status}")

# Test different expressions with variables, functions, and ranges
def main():
    try:
        # Declare constants
        solver.declareConstant("pi", np.pi)
        solver.declareConstant("e", np.e)

        # Declare global variables
        solver.declareVariable("x", 0)   # Global x
        solver.declareVariable("y", 0)   # Global y

        # Test with basic variables
        print("=== Testing Variables ===")
        test_expression("x + 0", expected_result=0.0)  # Expected: 0

        # Update the values of x and y and test again
        solver.declareVariable("x", 5)  # Local x
        solver.declareVariable("y", 10) # Local y
        test_expression("x + y", expected_result=15.0)  # Expected: 15

        # Test more complex expressions
        print("\n=== Testing Complex Expressions ===")
        test_expression("x * y + x^2", expected_result=75.0)  # Expected: 75
        test_expression("y / x", expected_result=2.0)  # Expected: 2

        # Test division by zero (expecting an exception)
        test_expression("x / (y - 10)", expected_result=SolverException)  # Should raise Division by zero

        # Declare a custom function f(x) = x^2 + 2*x + 1
        print("\n=== Declaring and Testing Custom Function f(x) ===")
        solver.declareFunction("f", ["x"], "x^2 + 2*x + 1")
        test_expression("f(3)", expected_result=16.0)  # Expected: 16
        test_expression("f(-1)", expected_result=0.0)  # Expected: 0

        # Declare another function g(x, y) = x * y + x + y
        print("\n=== Declaring and Testing Custom Function g(x, y) ===")
        solver.declareFunction("g", ["x", "y"], "x * y + x + y")
        test_expression("g(3, 4)", expected_result=19.0)  # Expected: 19
        test_expression("g(-2, 2) + 1", expected_result=-3.0)  # Expected: -3

        # Declare nested functions
        print("\n=== Declaring and Testing Nested Functions ===")
        solver.declareFunction("h", ["x"], "f(g(x, x))")
        test_expression("h(2)", expected_result=81.0)  # Expected: 81

        # Test with built-in functions and constants
        print("\n=== Testing Built-in Functions and Constants ===")
        test_expression("sin(pi / 2)", expected_result=1.0)  # Expected: 1
        test_expression("cos(0)", expected_result=1.0)       # Expected: 1
        test_expression("tan(pi / 4)", expected_result=1.0)  # Expected: 1
        test_expression("exp(1)", expected_result=np.e)       # Expected: e
        test_expression("ln(e)", expected_result=1.0)         # Expected: 1
        test_expression("sqrt(16)", expected_result=4.0)      # Expected: 4

        # Test complex expression combining constants and functions
        print("\n=== Testing Complex Expressions with Constants and Functions ===")
        test_expression("sin(pi / 4) + cos(pi / 4)", expected_result=np.sqrt(2), margin=1e-5, debug=True)  # Expected: ~1.4142
        test_expression("ln(e^2)", expected_result=2.0)  # Expected: 2
        test_expression("exp(ln(5))", expected_result=5.0)  # Expected: 5

        # Test function with constants
        solver.declareFunction("circlearea", ["r"], "pi * r^2")
        test_expression("circlearea(3)", expected_result=28.274333882308138)  # Expected: ~28.274

        # Test nested built-in functions
        test_expression("sqrt(sin(pi / 2))", expected_result=1.0)  # Expected: 1
        test_expression("ln(exp(5))", expected_result=5.0)        # Expected: 5

        # Additional Tests

        # a. Testing Undefined Functions
        print("\n=== Testing Undefined Function ===")
        test_expression("undefinedFunc(5)", expected_result=SolverException)  # Should raise an error

        # b. Testing Incorrect Number of Function Arguments
        print("\n=== Testing Incorrect Number of Function Arguments ===")
        test_expression("f(1, 2)", expected_result=SolverException)  # Should raise an error

        # c. Testing User-Defined Function with Complex Expressions
        print("\n=== Declaring and Testing Custom Function k(x) ===")
        solver.declareFunction("k", ["x"], "f(x) + g(x, x)")
        test_expression("k(3)", expected_result=31.0)  # Expected: 31

        # d. Testing Function with Nested Functions and Multiple Levels
        print("\n=== Declaring and Testing Custom Function m(x, y) ===")
        solver.declareFunction("m", ["x", "y"], "h(x) + f(g(x, y))")
        test_expression("m(2, 10)", expected_result=1170.0)  # Expected: 1170.0

        # e. Testing Function with Variable Scope
        print("\n=== Testing Function with Undefined Variable Scope ===")
        solver.declareFunction("n", ["x"], "x + z")
        test_expression("n(5)", expected_result=SolverException)  # Should raise an error

        # f. Testing Functions with Expressions as Arguments
        print("\n=== Testing Function with Expressions as Arguments ===")
        # Assuming g(2,3) = 2*3 +2 +3=11; h(1)=f(g(1,1))=f(1*1 +1 +1)=f(3)=16; f(11+16)=f(27)=27^2 +2*27 +1=729 +54 +1=784
        test_expression("f(g(2, 3) + h(1))", expected_result=784.0)  # Expected: 784.0

        # g. Testing Functions with Negative and Zero Arguments
        print("\n=== Testing Functions with Negative and Zero Arguments ===")
        test_expression("f(0)", expected_result=1.0)   # Expected: 1
        test_expression("f(-5)", expected_result=16.0)  # Expected: 16

        # h. Testing Multiple Nested Functions
        print("\n=== Declaring and Testing Multiple Nested Functions ===")
        # Declare p(x, y) = m(x, y) + k(x)
        solver.declareFunction("p", ["x", "y"], "m(x, y) + k(x)")
        test_expression("p(2, 10)", expected_result=1170.0 + 17.0)  # Expected: 1201.0

        # i. Testing Functions with Floating-Point Precision
        print("\n=== Testing Functions with Floating-Point Arguments ===")
        test_expression("f(2.5)", expected_result=12.25)  # Expected: 12.25

        # j. Testing Function Re-definition (Overwriting Existing Function)
        print("\n=== Testing Function Re-definition ===")
        test_expression("f(1)", expected_result=4.0)  # Existing function f(x)=x^2 +2x +1
        try:
            solver.declareFunction("f", ["x"], "x + 1")  # Should raise an error
            # If no exception, mark as failed
            logger.error("Declaring function 'f' again: FAILED (No Exception Raised)")
        except SolverException as e:
            logger.info(f"Declaring function 'f' again: PASSED (Expected Exception: {e})")


    except SolverException as e:
        print(f"Solver error: {e}")

if __name__ == "__main__":
    main()
