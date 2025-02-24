from solver import Solver, version
import numpy as np

solver = Solver()

print(version())

solver.declare_function("f", ["x"], "x + 2 + 5")
solver.declare_variable("x", 15)
solver.declare_constant("pi", np.pi)

x = solver.evaluate("x + 5 + f(pi)", True)

print("Result:", x)

dump = solver.dump()

newSolver = Solver()

newSolver.load(dump)


x = newSolver.evaluate("x + 5 + f(pi)", True)

print("New result:", x)

print()
print(newSolver.list_functions())
