# Solver

**Solver** is a high-performance mathematical expression solver and parsing library written in C++ with Python bindings. It is designed to evaluate complex mathematical expressions—including those involving variables, constants, and user-defined functions—using advanced parsing techniques, caching, and optimization strategies. The library is ideal for scientific computing, data analysis, and applications where performance is critical.

## Features

- **Expression Parsing:**  
  Parse infix expressions into tokenized, flattened postfix representations or abstract syntax trees (AST) for evaluation.
  
- **Built-In & User-Defined Functions:**  
  Easily register standard functions (e.g., `sin`, `cos`, `tan`) as well as declare custom functions using string expressions.

- **Caching and Performance:**  
  Uses two separate Least Recently Used (LRU) caches: one for evaluated results and one for compiled expressions. This minimizes repeated work by reusing parsed and compiled expressions.
  
- **Multiple Evaluation Modes:**  
  Evaluate single expressions, bulk evaluations over a range of values, or compute cartesian products of multiple variable ranges.
  
- **C++ and Python Integration:**  
  Compiled as a C++ library and exposed to Python via PyBind11. Use it in your Python projects or embed it in performance-critical C++ code.

- **AST and Postfix Evaluation:**  
  Supports both AST-based evaluation and simplified postfix evaluation pipelines, with built-in support for common algebraic simplifications.

## Requirements

- **Language & Compiler:**  
  C++23-compliant compiler
  
- **Build Tools:**  
  The build process uses CMake internally, but you must build the library via the provided Python setup script.
  
- **Python:**  
  Python 3.8 or later

- **Dependencies:**  
  - [PyBind11](https://github.com/pybind/pybind11)  
  - CMake, clang (these are listed in `install_requires` and managed by the build process)

## Manual Installation & Build

The recommended way to build **Solver** is using the provided `setup.py` script. The build process leverages CMake internally with Python-provided definitions and then automatically copies the resulting shared library (and stub files) to the examples directory.

1. **Clone the repository:**

   ```bash
   git clone https://github.com/Rodrigo-Vildosola/SOLVER.git
   cd SOLVER
   ```

2. **Build the library using the setup script:**

   ```bash
   python setup.py build
   ```

   This command:
   
   - Invokes CMake (with definitions supplied by the Python build commands)
   - Compiles the C++ library and Python bindings
   - Copies the compiled artifacts (including shared libraries, stub files, and an `__init__.py`) to the appropriate directories (for example, the `examples` directory)

3. **Install the library (optional):**

   If you want to install **Solver** for system-wide use, run:

   ```bash
   python setup.py install
   ```

   This installs the Python module so you can simply `import solver` in your projects.

> **Note:** Directly invoking CMake is not supported because some definitions are provided by the Python functions in the build scripts.

## Usage Examples

Below is an example showing how to use **Solver** to evaluate the cartesian products using evaluate for ranges to claculate and matplotlib to 3d plot the results in Python:

```python
import numpy as np
import matplotlib.pyplot as plt
from solver import Solver, SolverException

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
```

## API Overview

The **Solver** class exposes methods to:

- **Declare Symbols:**  
  - `declare_constant(name, value)`  
  - `declare_variable(name, value)`

- **Expression Evaluation:**  
  - `evaluate(expression, debug=False)`  
  - `evaluate_range(variable, values, expression, debug=False)`  
  - `evaluate_ranges(variables, valuesSets, expression, debug=False)`

- **Function Registration:**  
  - `declare_function(name, args, expression)`

- **Caching:**  
  - Enable or disable caching with `use_cache(useCache)`  

- **Parsing:**  
  - `set_current_expression(expression, debug)` parses an expression into a flattened postfix form  

For more details, please refer to the inline documentation and source code.

## Contributing

Contributions to **Solver** are welcome! Please follow these guidelines:

1. Fork the repository.
2. Create a feature branch.
3. Write tests and documentation as needed.
4. Submit a pull request with a clear description of your changes.

## License

This project is licensed under the [MIT License](LICENSE).

## Author

**Rodrigo Vildosola**  
[GitHub Repository](https://github.com/Rodrigo-Vildosola/SOLVER)

---

Happy solving!
