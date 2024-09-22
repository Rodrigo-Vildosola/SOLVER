from lib.solver import Solver, SolverException
import matplotlib.pyplot as plt
import numpy as np

# Initialize solver
solver = Solver()

# Utility function for evaluating and printing results
def test_expression(expression):
    try:
        result = solver.evaluate(expression, True)
        print(f"Evaluating: {expression} -> Result: {result}")
    except SolverException as e:
        print(f"Error evaluating {expression}: {e}")

# Test different expressions with variables, functions, and ranges
def main():
    try:
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
        test_expression("g(-2, 2) + 1")  # Expected: -2 * 2 + (-2) + 2 = -4

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
        # y_values = np.linspace(0, 10, 100)
        # g_results = solver.evaluateForRange("x", x_values.tolist(), "g(x, 5)")  # y = 5

        # plt.plot(x_values, g_results, label="g(x, 5) = x * 5 + x + 5")
        # plt.title("Plot of g(x, 5) = x * 5 + x + 5")
        # plt.xlabel("x")
        # plt.ylabel("g(x, 5)")
        # plt.grid(True)
        # plt.legend()
        # plt.show()

    except SolverException as e:
        print(f"Solver error: {e}")

if __name__ == "__main__":
    main()
