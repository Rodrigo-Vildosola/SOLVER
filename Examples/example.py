from lib.solver import Solver
def test_equation(solver, equation):
    print(f"Testing equation: {equation}")

    # Tokenize the equation (this function should exist in your C++ class)
    tokens = solver.tokenize(equation)
    if not tokens:
        print(f"Error: Tokenization failed for equation: {equation}")
        return
    
    print(tokens)
    expr_tree = solver.parseExpression(tokens)
    print(expr_tree)

    # Evaluate the equation (this function should exist in your C++ class)
    evaluated_value = solver.evaluate(equation)
    print(f"Evaluated expression value: {evaluated_value}")
    print("--------------------------\n")

def main():
    solver = Solver()

    # Declare variables
    solver.declareVariable("x", 1)
    solver.declareVariable("y", 2)
    solver.declareVariable("z", 3)
    solver.declareVariable("w", 4)

    # Declare custom functions
    solver.declareFunction("sh", ["y"], "y + 3")
    solver.declareFunction("f", ["x"], "x * 2 + 1")

    # Test expressions
    expressions = [
        "sh(x) * sh(y)",
        "f(z) + sh(w)",
        "(x^2 + 4 * y) / (3 - z)"
    ]

    for expression in expressions:
        test_equation(solver, expression)

if __name__ == "__main__":
    main()
