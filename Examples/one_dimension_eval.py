#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

# Import from your solver binding module
# If your built module is named differently, adjust the import accordingly.
from solver import Solver, SolverException

def main():
    # Create a solver instance
    solver = Solver()

    # We'll sweep a single variable 'x' from 0 to 2π
    x_values = np.linspace(0, 2*np.pi, 200)

    # Evaluate multiple expressions for the same x range
    expressions = [
        "sin(x)",
        "cos(x)",
        "x^2",
        "x^3 - 5*x + 3"
    ]

    # Plot each expression's result against x
    plt.figure(figsize=(10, 6))
    for expr in expressions:
        try:
            # Evaluate expr for the array of x_values
            results = solver.evaluate_range("x", x_values.tolist(), expr)

            # Plot
            plt.plot(x_values, results, label=expr)
        except SolverException as e:
            print(f"Error evaluating '{expr}': {e}")

    # Add labels/legend
    plt.title("Single Variable Expressions")
    plt.xlabel("x")
    plt.ylabel("f(x)")
    plt.legend()
    plt.grid(True)

    # Display the plot
    plt.show()

if __name__ == "__main__":
    main()
