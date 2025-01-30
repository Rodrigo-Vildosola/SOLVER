#!/usr/bin/env python3
import sys
import timeit
import math
import numpy as np
import logging

# Import from your solver binding module (e.g. "from solver import Solver, SolverException")
from solver import Solver, SolverException

def main():
    # Set up logging
    logger = logging.getLogger("SolverPerformance")
    logger.setLevel(logging.INFO)
    console_handler = logging.StreamHandler(sys.stdout)
    console_formatter = logging.Formatter('%(levelname)s - %(message)s')
    console_handler.setFormatter(console_formatter)
    logger.addHandler(console_handler)

    # Create solver instance
    solver = Solver()

    solver.use_cache(False)

    try:
        # Declare constants
        solver.declare_constant("pi", np.pi)
        solver.declare_constant("e", np.e)
        solver.declare_constant("b", 2)
        solver.declare_constant("c", 5)

        # Declare variables
        solver.declare_variable("x", 10)

        # Declare user-defined functions
        solver.declare_function("f", ["x"], "x^2 + 2*x + 1")
        solver.declare_function("g", ["x", "y"], "x * y + x + y")
        solver.declare_function("h", ["x"], "f(g(x, x))")
        solver.declare_function("x", ["x"], "x + 1")  # overshadowing
        solver.declare_function("why", [], "2 + 3")

    except SolverException as e:
        logger.error(f"Initialization Error: {e}")
        sys.exit(1)

    # Example expressions to evaluate
    examples = [
        {"description": "Evaluate h(2)", 
         "expression": "h(2)", 
         "expected_result": 81.0},
        {"description": "Evaluate why()", 
         "expression": "why()", 
         "expected_result": 5},
        {"description": "Evaluate g(f(2), 5)", 
         "expression": "g(f(2), 5)", 
         "expected_result": 59.0},
        {"description": "Evaluate x + 0", 
         "expression": "x + 0", 
         "expected_result": 10},
        {"description": "Evaluate x * 1", 
         "expression": "x * 1", 
         "expected_result": 10},
        {"description": "Evaluate f(4)", 
         "expression": "f(4)", 
         "expected_result": 25.0},
        {"description": "Evaluate (b + 3) * (c + 0)", 
         "expression": "(b + 3) * (c + 0)", 
         "expected_result": 25.0},
        {"description": "Evaluate complex expression", 
         "expression": "3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3", 
         "expected_result": 3.000122},
        {"description": "Evaluate sin(pi/2)", 
         "expression": "sin(pi/2)", 
         "expected_result": 1},
        {"description": "Evaluate f(g(2, 2) + sin(pi/2))", 
         "expression": "f(g(2, 2) + sin(pi/2))", 
         "expected_result": 100},
        {"description": "Evaluate x + 2 * 4 + 3", 
         "expression": "x + 2 * 4 + 3", 
         "expected_result": 21},
        {"description": "Evaluate pi * 5", 
         "expression": "pi * 5", 
         "expected_result": np.pi * 5},
    ]

    # We'll do multiple runs to get average timings
    num_runs = 5
    repetitions_per_expression = 100000

    postfix_times = []
    ast_times = []
    passed_postfix = 0
    passed_ast = 0

    for ex in examples:
        expr = ex["expression"]
        expected = ex["expected_result"]
        desc = ex["description"]

        # We'll measure time for 'evaluate()' (postfix pipeline)
        # using Python's timeit with a small lambda
        def test_postfix():
            val = solver.evaluate(expr, False)
            return val

        # We'll measure time for 'evaluate_ast()' (AST pipeline)
        def test_ast():
            val = solver.evaluate_ast(expr, False)
            return val

        # First, do a single "correctness check" outside timeit
        # Postfix
        try:
            val_p = test_postfix()
            correct_p = math.isclose(val_p, expected, rel_tol=1e-6, abs_tol=1e-6)
            if correct_p:
                passed_postfix += 1
            else:
                logger.warning(f"(POSTFIX) Expression '{expr}' got {val_p}, expected {expected}")
        except SolverException as e:
            logger.error(f"(POSTFIX) Exception evaluating '{expr}': {e}")
            correct_p = False

        # AST
        try:
            val_a = test_ast()
            correct_a = math.isclose(val_a, expected, rel_tol=1e-6, abs_tol=1e-6)
            if correct_a:
                passed_ast += 1
            else:
                logger.warning(f"(AST) Expression '{expr}' got {val_a}, expected {expected}")
        except SolverException as e:
            logger.error(f"(AST) Exception evaluating '{expr}': {e}")
            correct_a = False

        # Now measure performance if correctness didn't fail
        if correct_p:
            # Evaluate postfix multiple times
            postfix_time = timeit.timeit(
                stmt="test_postfix()",
                setup="",
                globals={"test_postfix": test_postfix},
                number=repetitions_per_expression
            )
            # average per call
            avg_postfix = postfix_time / repetitions_per_expression
        else:
            avg_postfix = float('inf')

        if correct_a:
            ast_time = timeit.timeit(
                stmt="test_ast()",
                setup="",
                globals={"test_ast": test_ast},
                number=repetitions_per_expression
            )
            # average per call
            avg_ast = ast_time / repetitions_per_expression
        else:
            avg_ast = float('inf')

        postfix_times.append(avg_postfix)
        ast_times.append(avg_ast)

        logger.info(f"Expression: {expr}\n"
                    f" - Postfix => {avg_postfix*1e6:.3f} us/iter, "
                    f"{'PASS' if correct_p else 'FAIL'}\n"
                    f" - AST     => {avg_ast*1e6:.3f} us/iter, "
                    f"{'PASS' if correct_a else 'FAIL'}\n"
                   )

    # Summaries
    avg_postfix_overall = sum(postfix_times) / len(postfix_times)
    avg_ast_overall = sum(ast_times) / len(ast_times)

    logger.info("\n===== PERFORMANCE SUMMARY =====")
    logger.info(f"Number of expressions tested: {len(examples)}")
    logger.info(f"Postfix pipeline: avg = {avg_postfix_overall*1e6:.3f} us/iter "
                f"(passed {passed_postfix}/{len(examples)})")
    logger.info(f"AST pipeline:     avg = {avg_ast_overall*1e6:.3f} us/iter "
                f"(passed {passed_ast}/{len(examples)})")

if __name__ == "__main__":
    main()
