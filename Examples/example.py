from lib.solver import Solver
def test_equation(solver, equation):
    print(f"Testing equation: {equation}")

    # Tokenize the equation (this function should exist in your C++ class)
    tokens = solver.tokenize(equation)
    if not tokens:
        print(f"Error: Tokenization failed for equation: {equation}")
        return
    
    # print(tokens)
    expr_tree = solver.parseExpression(tokens)
    print(expr_tree.left.value)

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

    # tokens = [(3, 'sh'), (4, '('), (1, 'x'), (4, ')'), (2, '*'), (3, 'sh'), (4, '('), (1, 'y'), (4, ')')]
    # print("Tokens before passing to C++:", tokens)
    # print("Type of tokens:", type(tokens))
    # for t in tokens:
    #     print("Tuple:", t, " Type of tuple:", type(t))
    # expr_tree = solver.parseExpression(tokens)
    # print(expr_tree.value)


if __name__ == "__main__":
    main()
