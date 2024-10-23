import timeit
import numpy as np
import matplotlib.pyplot as plt
from sympy import symbols, lambdify
import numexpr as ne
from solver import Solver, SolverException

def compare_performance():
    # Initialize custom solver
    solver = Solver()
    solver.setUseCache(False)
    try:
        solver.declareConstant("pi", np.pi)
        solver.declareVariable("x", 10)
        solver.declareVariable("y", 100)

        solver.declareFunction("f", ["x"], "x^2 + 2*x + 1")
        solver.declareFunction("g", ["x", "y"], "x * y + x + y")
        solver.declareFunction("h", ["x"], "f(g(x, x))")
    except SolverException as e:
        print(f"Error initializing solver: {e}")
        return

    # SymPy setup
    x_sym, y_sym = symbols('x y')
    f_sympy = x_sym**2 + 2*x_sym + 1
    g_sympy = x_sym * y_sym + x_sym + y_sym
    h_sympy = f_sympy.subs(x_sym, g_sympy)
    
    f_lambdified = lambdify(x_sym, f_sympy)
    g_lambdified = lambdify([x_sym, y_sym], g_sympy)
    h_lambdified = lambdify(x_sym, h_sympy)

    # NumExpr setup: Expanding functions into raw expressions
    ne_expr_f = "x**2 + 2*x + 1"
    ne_expr_g = "x * y + x + y"
    ne_expr_h = "(x * x + x + x)**2 + 2*(x * x + x + x) + 1"  # Expanded version of f(g(x, x))

    # Define expressions to benchmark
    expressions = [
        {
            "description": "Simple addition (2 + 2)",
            "custom_solver_expr": "2 + 2",
            "sympy_expr": "2 + 2",
            "numexpr_expr": "2 + 2",
        },
        {
            "description": "Quadratic function f(10)",
            "custom_solver_expr": "f(10)",
            "sympy_expr": f_lambdified,
            "numexpr_expr": ne_expr_f,
        },
        {
            "description": "Function g(5, 5)",
            "custom_solver_expr": "g(5, 5)",
            "sympy_expr": g_lambdified,
            "numexpr_expr": ne_expr_g,
        },
        {
            "description": "Nested function h(2)",
            "custom_solver_expr": "h(2)",
            "sympy_expr": h_lambdified,
            "numexpr_expr": ne_expr_h,
        }
    ]

    num_trials = 100000
    performance_results = []

    # Benchmark each expression
    for expr in expressions:
        description = expr["description"]

        # Custom Solver Benchmark
        def custom_solver_eval():
            solver.setCurrentExpression(expr["custom_solver_expr"], True)
            return solver.evaluate(expr["custom_solver_expr"])

        # SymPy Benchmark
        def sympy_eval():
            if isinstance(expr["sympy_expr"], str):
                return eval(expr["sympy_expr"])
            else:
                if "y" in expr["sympy_expr"].__code__.co_varnames:
                    return expr["sympy_expr"](10, 5)
                else:
                    return expr
        # NumExpr Benchmark
        def numexpr_eval():
            return ne.evaluate(expr["numexpr_expr"], local_dict={'x': 10, 'y': 5})

        # Time each solver
        custom_solver_time = timeit.timeit(custom_solver_eval, number=num_trials)
        sympy_time = timeit.timeit(sympy_eval, number=num_trials)
        numexpr_time = timeit.timeit(numexpr_eval, number=num_trials)

        # Store results
        performance_results.append({
            "description": description,
            "custom_solver_time": custom_solver_time,
            "sympy_time": sympy_time,
            "numexpr_time": numexpr_time
        })

        # Print results
        print(f"{description}:")
        print(f"  Custom Solver Time: {custom_solver_time:.6f} seconds")
        print(f"  SymPy Time: {sympy_time:.6f} seconds")
        print(f"  NumExpr Time: {numexpr_time:.6f} seconds\n")

    return performance_results

def plot_results(performance_results):
    # Extract the data
    descriptions = [result['description'] for result in performance_results]
    custom_solver_times = [result['custom_solver_time'] for result in performance_results]
    sympy_times = [result['sympy_time'] for result in performance_results]
    numexpr_times = [result['numexpr_time'] for result in performance_results]

    # Plotting the results
    x = np.arange(len(descriptions))  # Label locations
    width = 0.2  # Bar width

    fig, ax = plt.subplots(figsize=(10, 6))

    ax.bar(x - width, custom_solver_times, width, label='Custom Solver')
    ax.bar(x, sympy_times, width, label='SymPy')
    ax.bar(x + width, numexpr_times, width, label='NumExpr')

    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_ylabel('Time (seconds)')
    ax.set_title('Performance Comparison of Custom Solver vs SymPy and NumExpr')
    ax.set_xticks(x)
    ax.set_xticklabels(descriptions, rotation=45, ha="right")
    ax.legend()

    fig.tight_layout()

    # Show plot
    plt.show()

if __name__ == "__main__":
    performance_results = compare_performance()
    plot_results(performance_results)
