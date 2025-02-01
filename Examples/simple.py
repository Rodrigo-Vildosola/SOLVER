from solver import Solver
import numpy as np



solver = Solver()

solver.use_cache(False)

solver.declare_constant("pi", np.pi)
solver.declare_variable("x", 10)

result = solver.evaluate("tan(pi / 4)", True)


print(f"The results of the solver are: {result}")
