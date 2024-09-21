from lib.solver import Solver, SolverException
import matplotlib.pyplot as plt
import numpy as np

# Initialize solver
solver = Solver()

# Utility function for evaluating and printing results
def test_expression(expression):
    try:
        result = solver.evaluate(expression)
        print(f"Evaluating: {expression} -> Result: {result}")
    except SolverException as e:
        print(f"Error evaluating {expression}: {e}")

# Test different expressions with variables, functions, and ranges
def main():
    try:
        # Declare global variables
        solver.declareVariable("x", 0, True)   # Global x
        solver.declareVariable("y", 0, True)   # Global y
        solver.declareVariable("pi", np.pi, True)  # Declare global pi

        # Test with basic variables
        print("=== Testing Variables ===")
        test_expression("x + y")  # Expected: 0

        # Update the values of x and y and test again
        solver.declareVariable("x", 5, False)  # Local x
        solver.declareVariable("y", 10, False) # Local y
        test_expression("x + y")  # Expected: 15

        # Test more complex expressions
        print("\n=== Testing Complex Expressions ===")
        test_expression("x * y + x^2")  # Expected: 5 * 10 + 5^2 = 75
        test_expression("y / x")  # Expected: 10 / 5 = 2

        # Test division by zero (expecting an exception)
        test_expression("x / (y - 10)")  # This will cause a division by zero

        # Declare a custom function f(x) = pi * x^2 (uses the global variable pi)
        print("\n=== Declaring and Testing Custom Function f(x) with global pi ===")
        solver.declareFunction("f", ["x"], "pi * x^2")
        test_expression("f(3)")  # Expected: pi * 3^2 = 28.2743...
        test_expression("f(5)")  # Expected: pi * 5^2 = 78.5398...

        # Declare a custom function g(x, y) = x * y + z (z is not passed, should throw an error)
        print("\n=== Declaring and Testing Custom Function g(x, y) with missing z ===")
        solver.declareFunction("g", ["x", "y"], "x * y + z")
        test_expression("g(3, 4)")  # This should raise an error because z is not defined

        # Test plotting a range of values using the custom function f(x)
        print("\n=== Plotting Results for f(x) ===")
        x_values = np.linspace(-10, 10, 400)
        f_results = solver.evaluateForRange("x", x_values.tolist(), "f(x)")

        plt.plot(x_values, f_results, label="f(x) = pi * x^2")
        plt.title("Plot of f(x) = pi * x^2")
        plt.xlabel("x")
        plt.ylabel("f(x)")
        plt.grid(True)
        plt.legend()
        plt.show()

    except SolverException as e:
        print(f"Solver error: {e}")

if __name__ == "__main__":
    main()
