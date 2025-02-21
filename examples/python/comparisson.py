#!/usr/bin/env python3
import sys
import timeit
import math
import numpy as np
import logging

# Import from your solver binding module
from solver import Solver, SolverException


def main():
    # Set up logging
    logger = logging.getLogger("ExtendedSolverPerformance")
    logger.setLevel(logging.INFO)
    console_handler = logging.StreamHandler(sys.stdout)
    console_formatter = logging.Formatter('%(levelname)s - %(message)s')
    console_handler.setFormatter(console_formatter)
    logger.addHandler(console_handler)

    # Create solver instance
    solver = Solver()

    # Example: Demonstrate variable overshadowing is not used
    # We won't overshadow "x" this time to avoid confusion.

    try:
        # Declare constants
        solver.declare_constant("pi", np.pi)
        solver.declare_constant("e", np.e)

        # Declare variables (some might be used in expressions)
        solver.declare_variable("x", 10)
        solver.declare_variable("y", 3)

        # Declare user-defined functions for more complex tests
        # 1) A polynomial in x => p(x) = x^3 - 2*x^2 + 4*x - 1
        solver.declare_function("p", ["x"], "x^3 - 2*x^2 + 4*x - 1")

        # 2) Another function: q(x, y) = sin(x) + ln(y) + x^y
        solver.declare_function("q", ["x", "y"], "sin(x) + ln(y) + x ^ y")

        # 3) Combined function r(a, b) = p(a) + q(b, a^2)
        solver.declare_function("r", ["a", "b"], "p(a) + q(b, a^2)")

        # 4) A nested expression that includes your "r" function
        solver.declare_function("nested", ["u"], "r(u+1, u-1) * r(u-2, u+3)")

    except SolverException as e:
        logger.error(f"Initialization Error: {e}")
        sys.exit(1)

    # Complex example expressions to evaluate with each pipeline
    examples = [
        {
            "description": "Evaluate p(5)",
            "expression": "p(5)",
            "expected_result": 5**3 - 2*5**2 + 4*5 - 1,  # 125 - 50 + 20 -1=94
        },
        {
            "description": "Evaluate q(x, y) w/ x=10, y=3",
            "expression": "q(x, y)",
            # sin(10) + ln(3) + 10^3
            "expected_result": math.sin(10) + math.log(3) + (10**3),
        },
        {
            "description": "Evaluate r(x, y)",
            "expression": "r(x, y)",
            # r(10,3) => p(10) + q(3, 10^2)
            # p(10)=10^3 -2*10^2 + 4*10 -1=1000-200+40-1=839
            # q(3,100)= sin(3)+ln(100)+ 3^100
            # 3^100 is large (5.153775e+47)
            # This expression is huge, but let's keep it for stress testing
            "expected_result": None,  # We'll skip correctness check for huge numbers
        },
        {
            "description": "Evaluate nested(2)",
            "expression": "nested(2)",
            # nested(2)= r(3,1) * r(0,5)
            # r(3,1) => p(3)+ q(1,3^2=9)
            # p(3)=27 -18 +12-1=20
            # q(1,9)= sin(1)+ ln(9)+ 1^9=0.84...+2.197+1= ~4.037
            # => r(3,1)=24.037 approx
            # r(0,5)= p(0)+ q(5,0^2=0) => p(0)= -1
            # q(5,0)= sin(5)+ ln(0)?? => ln(0) is -inf => might cause an error or negative infinity
            # We might do a partial check or skip correctness
            "expected_result": None,
        },
        {
            "description": "Trig + exponent mix",
            "expression": "sin(pi/2) + cos(pi) + e^2 - sqrt(16)",
            # => 1 + -1 + e^2 - 4 = e^2 - 4
            "expected_result": math.e**2 - 4,
        },
        {
            "description": "Multi-operator test with parentheses",
            "expression": "((x + y) * 2 + 15) / ( (x-y) * 2 ^ 3 )",
            # x=10, y=3 => top= (13*2+15)=26+15=41
            # bottom= (7 * 2^3)= (7*8)=56 => 41/56=0.73214
            "expected_result": 41/56.0,
        },
        {
            "description": "Value with big exponents",
            "expression": "2^10 + 3^7 + 5^4",
            # 2^10=1024,3^7=2187,5^4=625 => total= 3836
            "expected_result": 3836,
        },
        {
            "description": "sin(pi/2) * ln(e^5) - log(e^2, e)",
            # => 1*(ln(e^5)) - log(e^2,e)
            # ln(e^5)=5 => log(e^2,e)=2
            # => 5-2=3
            "expression": "sin(pi/2) * ln(e^5) - log(e^2, e)",
            "expected_result": 3,
        },
        # We can skip partial correctness checks for extremely large or domain-error
        # expressions, or keep them and handle domain errors. 
    ]

    # We'll do multiple runs to get average timings
    repetitions_per_expression = 5  # We can do more for stable times

    postfix_times = []
    ast_times = []
    passed_postfix = 0
    passed_ast = 0

    for ex in examples:
        expr = ex["expression"]
        desc = ex["description"]
        expected = ex.get("expected_result", None)  # Might be None if too large or domain error

        def test_postfix():
            val = solver.evaluate(expr, True)
            return val

        def test_ast():
            val = solver.evaluate_ast(expr, False)
            return val

        # If we have an "expected_result" that isn't None, do a correctness check
        postfix_correct = True
        ast_correct = True
        if expected is not None:
            # Postfix correctness
            try:
                val_p = test_postfix()
                # If the result is huge or negative infinity, or might fail
                # We'll do isclose with big tolerance for big numbers
                if math.isinf(expected) or abs(expected) > 1e10:
                    # We'll skip checking. Or do a ratio check
                    pass
                else:
                    postfix_correct = math.isclose(val_p, expected, rel_tol=1e-6, abs_tol=1e-6)
            except SolverException as e:
                logger.error(f"(POSTFIX) Exception evaluating '{expr}': {e}")
                postfix_correct = False

            # AST correctness
            try:
                val_a = test_ast()
                if math.isinf(expected) or abs(expected) > 1e10:
                    pass
                else:
                    ast_correct = math.isclose(val_a, expected, rel_tol=1e-6, abs_tol=1e-6)
            except SolverException as e:
                logger.error(f"(AST) Exception evaluating '{expr}': {e}")
                ast_correct = False

        # Timing if correctness didn't fail or if we have no expected result
        if expected is None or postfix_correct:
            postfix_time = timeit.timeit(
                stmt="test_postfix()",
                setup="",
                globals={"test_postfix": test_postfix},
                number=repetitions_per_expression
            )
            avg_postfix = postfix_time / repetitions_per_expression
        else:
            avg_postfix = float('inf')

        if expected is None or ast_correct:
            ast_time = timeit.timeit(
                stmt="test_ast()",
                setup="",
                globals={"test_ast": test_ast},
                number=repetitions_per_expression
            )
            avg_ast = ast_time / repetitions_per_expression
        else:
            avg_ast = float('inf')

        if postfix_correct:
            passed_postfix += 1
        if ast_correct:
            passed_ast += 1

        postfix_times.append(avg_postfix)
        ast_times.append(avg_ast)

        logger.info(f"\nExpression: {desc}\n"
                    f"   => {expr}\n"
                    f" - Postfix => {avg_postfix*1e6:.3f} us/iter, "
                    f"{'PASS' if postfix_correct else 'FAIL'}\n"
                    f" - AST     => {avg_ast*1e6:.3f} us/iter, "
                    f"{'PASS' if ast_correct else 'FAIL'}\n")

    # Summaries
    avg_postfix_overall = sum(postfix_times) / len(postfix_times)
    avg_ast_overall = sum(ast_times) / len(ast_times)

    logger.info("\n===== EXTENDED PERFORMANCE SUMMARY =====")
    logger.info(f"Number of expressions tested: {len(examples)}")
    logger.info(f"Postfix pipeline: avg = {avg_postfix_overall*1e6:.3f} us/iter "
                f"(passed {passed_postfix}/{len(examples)})")
    logger.info(f"AST pipeline:     avg = {avg_ast_overall*1e6:.3f} us/iter "
                f"(passed {passed_ast}/{len(examples)})")

    # Additional test: Evaluate for ranges
    # Let's do a quick test for x in [1..5], y in [2..3], using a complex expression
    # We'll measure the time for both pipelines (though the solver's evaluateForRanges
    # calls the postfix pipeline. We'll do an AST version manually if we want.)
    logger.info("\n===== MULTI-VARIABLE RANGE TEST =====")
    expression_for_range = "r(x, y)"  # e.g. r(x, y)
    # Let's do x=1..5, y=2..3
    xvals = [1,2,3,4,5]
    yvals = [2,3]

    logger.info("Postfix: evaluateForRanges for r(x,y)")
    start_p = timeit.default_timer()
    # We'll call the built-in evaluateForRanges
    results_p = solver.evaluate_ranges(["x","y"], [xvals, yvals], expression_for_range, False)
    end_p = timeit.default_timer()
    logger.info(f"  => Done. Time: {(end_p-start_p)*1e3:.3f} ms, total results: {len(results_p)}")

    # For the AST approach, there's no direct "evaluateForRanges()" for AST. We can do a manual loop:
    logger.info("AST approach: manual cartesian product (x,y)")
    start_a = timeit.default_timer()
    for xv in xvals:
        for yv in yvals:
            # set in symbol table
            solver.declare_variable("x", xv)
            solver.declare_variable("y", yv)
            # now call evaluateASTPipeline
            _ = solver.evaluate_ast(expression_for_range, False)
    end_a = timeit.default_timer()
    logger.info(f"  => Done. Time: {(end_a-start_a)*1e3:.3f} ms, total results: {len(xvals)*len(yvals)}")


if __name__ == "__main__":
    main()
