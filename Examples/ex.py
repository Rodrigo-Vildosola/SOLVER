from solver import Solver, version
import numpy as np

solver = Solver()

print(version())

solver.declare_variable("x", 50)
solver.declare_function("f", ["x"], "x * 2 + 5")
x = solver.evaluate_range("x", list(range(10)), "10 + f(x)")


print(x)
