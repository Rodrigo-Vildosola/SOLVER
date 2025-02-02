#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

# Import from your solver binding module
from solver import Solver, SolverException

def main():
    # Create a solver instance
    solver = Solver()

    # Let's define two variables: x and y
    # We will evaluate an expression across the cartesian product of x_vals and y_vals.
    x_vals = np.linspace(-2, 2, 50)
    y_vals = np.linspace(-2, 2, 50)

    expression = "x^3 + y^4"

    try:
        results = solver.evaluate_ranges(
            ["x", "y"],
            [x_vals.tolist(), y_vals.tolist()],
            expression,
            debug=False
        )
    except SolverException as e:
        print(f"Error evaluating expression '{expression}': {e}")
        return

    # The results come back as a single flat array of length len(x_vals)*len(y_vals).
    # We reshape to (len(x_vals), len(y_vals)) for plotting on a grid.
    Z = np.array(results).reshape(len(x_vals), len(y_vals))

    # Create meshgrid for plotting
    X, Y = np.meshgrid(x_vals, y_vals)

    # We'll do a 3D surface plot
    fig = plt.figure(figsize=(10, 6))
    ax = fig.add_subplot(111, projection='3d')

    # Plot the surface
    ax.plot_surface(X, Y, Z, cmap='viridis', edgecolor='none')
    ax.set_title(f"3D Surface Plot of {expression}")
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("f(x, y)")

    # 2D contour plot in a separate figure
    fig2, ax2 = plt.subplots(figsize=(8, 6))
    contour = ax2.contourf(X, Y, Z, levels=30, cmap='viridis')
    fig2.colorbar(contour, ax=ax2)
    ax2.set_title(f"Contour Plot of {expression}")
    ax2.set_xlabel("x")
    ax2.set_ylabel("y")

    plt.show()

if __name__ == "__main__":
    main()
