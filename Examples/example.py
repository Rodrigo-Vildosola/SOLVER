from lib.solver import Solver, SolverException
import matplotlib.pyplot as plt
import numpy as np

# Initialize solver
solver = Solver()

# Utility function for evaluating and printing results
def test_expression(expression, debug = False):
    try:
        result = solver.evaluate(expression, debug)
        print(f"Evaluating: {expression} -> Result: {result}")
    except SolverException as e:
        print(f"Error evaluating {expression}: {e}")

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
        test_expression("x + 0")  # Expected: 0

        # Update the values of x and y and test again
        solver.declareVariable("x", 5)  # Local x
        solver.declareVariable("y", 10) # Local y
        test_expression("x + y")  # Expected: 15

        # Test more complex expressions
        print("\n=== Testing Complex Expressions ===")
        test_expression("x * y + x^2")  # Expected: 5 * 10 + 5^2 = 75
        test_expression("y / x")  # Expected: 10 / 5 = 2

        # Test division by zero (expecting an exception)
        test_expression("x / (y - 10)")  # This will cause a division by zero

        # Declare a custom function f(x) = x^2 + 2*x + 1
        print("\n=== Declaring and Testing Custom Function f(x) ===")
        solver.declareFunction("f", ["x"], "x^2 + 2*x + 1")
        test_expression("f(3)")  # Expected: 3^2 + 2*3 + 1 = 16
        test_expression("f(-1)")  # Expected: (-1)^2 + 2*(-1) + 1 = 0

        # Declare another function g(x, y) = x * y + x + y
        print("\n=== Declaring and Testing Custom Function g(x, y) ===")
        solver.declareFunction("g", ["x", "y"], "x * y + x + y")
        test_expression("g(3, 4)")  # Expected: 3 * 4 + 3 + 4 = 19
        test_expression("g(-2, 2) + 1")  # Expected: (-2 * 2 + (-2) + 2) + 1 = (-4 + (-2) + 2) + 1 = (-4) + 1 = -3

        # Declare nested functions
        print("\n=== Declaring and Testing Nested Functions ===")
        solver.declareFunction("h", ["x"], "f(g(x, x))")
        test_expression("h(2)")  # Compute g(2,2)=2*2+2+2=8; then f(8)=8^2+2*8+1=64+16+1=81

        # Test with built-in functions and constants
        print("\n=== Testing Built-in Functions and Constants ===")
        test_expression("sin(pi / 2)")  # Expected: sin(π/2) = 1
        test_expression("cos(0)")       # Expected: cos(0) = 1
        test_expression("tan(pi / 4)")  # Expected: tan(π/4) ≈ 1
        test_expression("exp(1)")       # Expected: e^1 = e
        test_expression("ln(e)")       # Expected: ln(e) = 1
        test_expression("sqrt(16)")     # Expected: 4

        # Test complex expression combining constants and functions
        print("\n=== Testing Complex Expressions with Constants and Functions ===")
        test_expression("sin(pi / 4) + cos(pi / 4)", True)  # Expected: √2
        test_expression("ln(e^2)")  # Expected: ln(e^2) = 2
        test_expression("exp(ln(5))")  # Expected: 5

        # Test function with constants
        solver.declareFunction("circlearea", ["r"], "pi * r^2")
        test_expression("circlearea(3)")  # Expected: π * 3^2 ≈ 28.274

        # Test nested built-in functions
        test_expression("sqrt(sin(pi / 2))")  # Expected: sqrt(1) = 1
        test_expression("ln(exp(5))")        # Expected: 5

        # Test plotting a range of values using the custom function f(x)
        # print("\n=== Plotting Results for f(x) ===")
        # x_values = np.linspace(-10, 10, 400)
        # f_results = solver.evaluateForRange("x", x_values.tolist(), "f(x)")

        # plt.plot(x_values, f_results, label="f(x) = x^2 + 2x + 1")
        # plt.title("Plot of f(x) = x^2 + 2x + 1")
        # plt.xlabel("x")
        # plt.ylabel("f(x)")
        # plt.grid(True)
        # plt.legend()
        # plt.show()

        # # Testing a range for another custom expression with x and y
        # print("\n=== Testing Range for g(x, y) ===")
        # x_values = np.linspace(0, 10, 100)
        # g_results = solver.evaluateForRange("x", x_values.tolist(), "g(x, 5)")  # y = 5

        # plt.plot(x_values, g_results, label="g(x, 5) = x * 5 + x + 5")
        # plt.title("Plot of g(x, 5) = x * 5 + x + 5")
        # plt.xlabel("x")
        # plt.ylabel("g(x, 5)")
        # plt.grid(True)
        # plt.legend()
        # plt.show()

        # # Test plotting built-in functions
        # print("\n=== Plotting Built-in Functions ===")
        # x_values = np.linspace(-2 * np.pi, 2 * np.pi, 400)
        # sin_results = solver.evaluateForRange("x", x_values.tolist(), "sin(x)")
        # cos_results = solver.evaluateForRange("x", x_values.tolist(), "cos(x)")

        # plt.plot(x_values, sin_results, label="sin(x)")
        # plt.plot(x_values, cos_results, label="cos(x)")
        # plt.title("Plot of sin(x) and cos(x)")
        # plt.xlabel("x")
        # plt.ylabel("Function value")
        # plt.grid(True)
        # plt.legend()
        # plt.show()

    except SolverException as e:
        print(f"Solver error: {e}")

if __name__ == "__main__":
    main()
