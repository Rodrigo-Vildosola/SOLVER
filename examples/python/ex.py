from solver import Solver, version
import numpy as np

solver = Solver()

print(version())

solver.declare_function("f", ["x"], "x + 30")
# solver.declare_variable("x", 15)
solver.declare_constant("pi", np.pi)

roots = solver.solve_equation("f(x) * x ^ --2 == 10", "x")

print("Roots:", roots)

