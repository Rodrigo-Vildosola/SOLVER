import timeit
import numpy as np
import matplotlib.pyplot as plt
from sympy import symbols, lambdify
import numexpr as ne
from solver import Solver, SolverException

def setup_solver():
    """Initialize and return a configured custom solver instance."""
    solver = Solver()
    solver.use_cache(False)
    try:
        solver.declare_constant("pi", np.pi)
        solver.declare_variable("x", 10)
        solver.declare_variable("y", 100)
        solver.declare_function("f", ["x"], "x^2 + (2*x + 1)")
        solver.declare_function("g", ["x", "y"], "x * y + x + y")
        solver.declare_function("h", ["x"], "f(g(x, x))")
    except SolverException as e:
        print(f"Error initializing solver: {e}")
        return None
    return solver

def add_benchmark(expression, description, custom_solver_expr, sympy_expr, numexpr_expr, range_var=None, range_values=None):
    """Helper to add a new benchmark scenario to the benchmark list."""
    benchmarks.append({
        "description": description,
        "expression": expression,
        "custom_solver_expr": custom_solver_expr,
        "sympy_expr": sympy_expr,
        "numexpr_expr": numexpr_expr,
        "range_var": range_var,
        "range_values": range_values
    })

def compare_performance(benchmarks, num_trials=10000, range_trials=100):
    performance_results = []

    for bench in benchmarks:
        description = bench["description"]
        range_var = bench.get("range_var")
        range_values = bench.get("range_values")
        
        # Custom Solver
        def custom_solver_eval():
            solver.set_current_expression(bench["custom_solver_expr"], True)
            return solver.evaluate(bench["custom_solver_expr"])

        # Range-based evaluation
        def custom_solver_range_eval():
            return solver.evaluate_range(range_var, range_values, bench["custom_solver_expr"])

        # SymPy
        def sympy_eval():
            if callable(bench["sympy_expr"]):
                return bench
            else:
                return eval(bench["sympy_expr"])

        # SymPy range-based evaluation
        def sympy_range_eval():
            return [bench["sympy_expr"](val) for val in range_values]

        # NumExpr
        def numexpr_eval():
            return ne.evaluate(bench["numexpr_expr"], local_dict={'x': 10, 'y': 5})

        # NumExpr range-based evaluation
        def numexpr_range_eval():
            return [ne.evaluate(bench["numexpr_expr"], local_dict={'x': val}) for val in range_values]

        # Timing evaluations
        custom_solver_time = timeit.timeit(custom_solver_eval, number=num_trials)
        sympy_time = timeit.timeit(sympy_eval, number=num_trials)
        numexpr_time = timeit.timeit(numexpr_eval, number=num_trials)

        # Time range-based evaluations, if applicable
        if range_var:
            custom_solver_range_time = timeit.timeit(custom_solver_range_eval, number=range_trials)
            sympy_range_time = timeit.timeit(sympy_range_eval, number=range_trials)
            numexpr_range_time = timeit.timeit(numexpr_range_eval, number=range_trials)
        else:
            custom_solver_range_time = sympy_range_time = numexpr_range_time = None

        performance_results.append({
            "description": description,
            "custom_solver_time": custom_solver_time,
            "sympy_time": sympy_time,
            "numexpr_time": numexpr_time,
            "custom_solver_range_time": custom_solver_range_time,
            "sympy_range_time": sympy_range_time,
            "numexpr_range_time": numexpr_range_time
        })

    return performance_results

def plot_results(performance_results):
    descriptions = [result['description'] for result in performance_results]
    custom_solver_times = [result['custom_solver_time'] for result in performance_results]
    sympy_times = [result['sympy_time'] for result in performance_results]
    numexpr_times = [result['numexpr_time'] for result in performance_results]

    range_descriptions = [
        result["description"] + " (Range)" for result in performance_results if result["custom_solver_range_time"] is not None
    ]
    custom_solver_range_times = [result['custom_solver_range_time'] for result in performance_results if result["custom_solver_range_time"] is not None]
    sympy_range_times = [result['sympy_range_time'] for result in performance_results if result["sympy_range_time"] is not None]
    numexpr_range_times = [result['numexpr_range_time'] for result in performance_results if result["numexpr_range_time"] is not None]

    x = np.arange(len(descriptions))
    width = 0.2

    fig, ax = plt.subplots(figsize=(12, 8))
    ax.bar(x - width, custom_solver_times, width, label='Custom Solver')
    ax.bar(x, sympy_times, width, label='SymPy')
    ax.bar(x + width, numexpr_times, width, label='NumExpr')

    # Plot range-based benchmarks
    x_range = np.arange(len(range_descriptions)) + len(descriptions)
    ax.bar(x_range - width, custom_solver_range_times, width, label='Custom Solver (Range)')
    ax.bar(x_range, sympy_range_times, width, label='SymPy (Range)')
    ax.bar(x_range + width, numexpr_range_times, width, label='NumExpr (Range)')

    ax.set_ylabel('Time (seconds)')
    ax.set_title('Performance Comparison: Custom Solver vs SymPy and NumExpr')
    ax.set_xticks(np.concatenate((x, x_range)))
    ax.set_xticklabels(descriptions + range_descriptions, rotation=45, ha="right")
    ax.legend()

    fig.tight_layout()
    plt.show()

# Setup and run benchmarks
if __name__ == "__main__":
    solver = setup_solver()
    if not solver:
        exit()

    benchmarks = []
    range_values = np.linspace(1, 100, 1000)

    # Add benchmarks
    add_benchmark("2 + 2", "Simple addition (2 + 2)", "2 + 2", "2 + 2", "2 + 2")
    add_benchmark("f(10)", "Quadratic function f(10)", "f(10)", lambdify(symbols('x'), symbols('x')**2 + 2*symbols('x') + 1), "x**2 + 2*x + 1")
    add_benchmark("g(5, 5)", "Function g(5, 5)", "g(5, 5)", lambdify([symbols('x'), symbols('y')], symbols('x') * symbols('y') + symbols('x') + symbols('y')), "x * y + x + y")
    add_benchmark("h(2)", "Nested function h(2)", "h(2)", lambdify(symbols('x'), (symbols('x')**2 + 2*symbols('x') + 1).subs(symbols('x'), symbols('x') * symbols('x') + symbols('x') + symbols('x'))), "(x * x + x + x)**2 + 2*(x * x + x + x) + 1")
    add_benchmark("f(x)", "Quadratic function f(x) over range", "f(x)", lambdify(symbols('x'), symbols('x')**2 + 2*symbols('x') + 1), "x**2 + 2*x + 1", "x", range_values)

    # Compare performance and plot
    performance_results = compare_performance(benchmarks)
    plot_results(performance_results)
