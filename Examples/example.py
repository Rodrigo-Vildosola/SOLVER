from lib.solver import Solver
import matplotlib.pyplot as plt
import numpy as np

def plot_expression(solver, expression, x_range=(-10, 10), num_points=400):
    """
    Plots the given mathematical expression over the specified x range.

    Parameters:
    solver (Solver): The solver object used to evaluate the expression.
    expression (str): The mathematical expression to plot (e.g., "x^2 + 3*x + 2").
    x_range (tuple): The range of x values to plot (start, end).
    num_points (int): The number of points to calculate between the range.
    """
    x_values = np.linspace(x_range[0], x_range[1], num_points)
    y_values = []

    for x in x_values:
        # Replace 'x' in the expression with the current x value
        expr = expression.replace('x', f'({x})')
        try:
            # Evaluate the expression using the solver
            y = solver.evaluate(expr)
            y_values.append(y)
        except Exception as e:
            print(f"Error evaluating expression at x = {x}: {e}")
            y_values.append(np.nan)  # Use NaN for plotting errors

    # Plot the results using matplotlib
    plt.figure(figsize=(10, 6))
    plt.plot(x_values, y_values, label=expression)
    plt.xlabel('x')
    plt.ylabel('f(x)')
    plt.title(f'Plot of {expression}')
    plt.legend()
    plt.grid(True)
    plt.show()

def main():
    solver = Solver()

    # Declare variables
    solver.declareVariable("x", 0)  # Default variable declaration

    # Declare custom functions if needed
    solver.declareFunction("sh", ["x"], "x + 3")
    solver.declareFunction("f", ["x"], "x * 2 + 1")

    # Expressions to plot
    expressions = [
        "x^2 + 3*x - 2",
        "sh(x) * sh(x)",
        "f(x) + sh(x)",
        "(x^2 + 4 * x) / (3 - x)",
        "2 * sin(x)",
        "exp(x) - 2",
        "log(x) * x",
        "x^2 + 4 * x + 4",
        "1 / (x - 2)",  # This will show infinity near x=2
    ]

    # for expression in expressions:
    #     plot_expression(solver, expression, x_range=(-10, 10))

    # Linear equations to solve
    linear_equations = [
        "2 * x + 5 = 20 + x",
        "3 * x - 4 = 2 * x + 6",
        "x - 3 = 2 * x + 1",
        "-2 * x + 4 = x - 2",
        "5 = 5 + x",
        "x / 2 = 3",
        "0 = x + 5",
    ]

    for equation in linear_equations:
        try:
            solution = solver.solveLinearEquation(equation)
            print(f"Solution for '{equation}': x = {solution}")
        except Exception as e:
            print(f"Error solving '{equation}': {e}")

if __name__ == "__main__":
    main()
