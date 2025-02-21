from solver import Solver, version
import numpy as np

solver = Solver()

print(version())

solver.declare_function("f", ["x"], "x * 2 + 5")


x = solver.evaluate("10 + 5 + f(10)")

print(x)
