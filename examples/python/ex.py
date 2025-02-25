from solver import Solver, version
import numpy as np

solver = Solver()

print(version())

solver.declare_function("f", ["x"], "x + 2 + 5")
solver.declare_variable("x", 15)
solver.declare_constant("pi", np.pi)

x = solver.evaluate("x + 5 + f(pi)", True)

print("Result:", x)


print()
print(solver.list_constants())
solver.delete_constant("pi")
print(solver.list_constants())

solution = solver.solve_equation("x + 5 = 10", "x", 0.0, tolerance=1e-6, max_iterations=10000)

print(solution)
