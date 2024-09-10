from lib.solver import Solver

def print_tree(node, depth=0, prefix="", is_left=True):
    """
    Recursively prints the tree structure of the expression.
    """
    if node:
        # Print the current node
        print(prefix, end="")

        # Add an appropriate connector (either a corner or a line)
        if depth == 0:
            print(node.value)
        else:
            print("├── " if is_left else "└── ", end="")
            print(node.value)

        # Prepare the prefix for the children
        child_prefix = prefix + ("│   " if is_left else "    ")

        # Print the left subtree first
        if node.left:
            print_tree(node.left, depth + 1, child_prefix, True)

        # Print the right subtree
        if node.right:
            print_tree(node.right, depth + 1, child_prefix, False)

def test_equation(solver, equation):
    print(f"Testing equation: {equation}")

    # Tokenize the equation
    tokens = solver.tokenize(equation)
    if not tokens:
        print(f"Error: Tokenization failed for equation: {equation}")
        return

    # Parse the tokens into an expression tree
    expr_tree = solver.parseExpression(tokens)
    if not expr_tree:
        print(f"Error: Parsing failed for equation: {equation}")
        return

    # Print the parsed expression tree
    print("Expression tree:")
    print(expr_tree.left.value)
    # print_tree(expr_tree)

    # Evaluate the equation
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
