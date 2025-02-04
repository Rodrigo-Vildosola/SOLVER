from solver import Solver
import numpy as np
import matplotlib.pyplot as plt



solver = Solver()

solver.use_cache(False)

solver.declare_constant("pi", np.pi)
solver.declare_variable("x", 10)
solver.declare_function("func", ["z"], "z + 20")

result = solver.evaluate("tan(pi / 4)", True)

print(f"The results of the solver are: {result}")

result = solver.evaluate("3 + (x + 8)", True)

print(f"The results of the solver are: {result}")

x_values = np.linspace(0, 100, 1000)

# results = solver.evaluate_range("x", x_values.tolist(), "x^3 + 2*x + 1", True)

# plt.plot(x_values, results, label="x^3 + 2x + 1")
# plt.title("Single Variable Expressions")
# plt.xlabel("x")
# plt.ylabel("f(x)")
# plt.legend()
# plt.grid(True)

# # Display the plot
# plt.show()
