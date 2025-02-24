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
print(solver.list_functions())
solver.reset()
print(solver.list_functions())

