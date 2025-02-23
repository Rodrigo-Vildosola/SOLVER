// test.mjs
import { createSolverModule, SolverModule} from './solver/index.js';

async function main() {
  // "Solver" is the class_ we exposed in EMSCRIPTEN_BINDINGS
  const mod: SolverModule = await createSolverModule();

  const { Solver, getExceptionMessage, getException } = mod;

  // Create a solver instance with an LRU cache size of 100
  const solver = new Solver(100);

  solver.declare_function("f", ["x"], "x + 8");

  // Declare a variable and evaluate an expression
  solver.declare_variable("x", 3);
  const result1 = solver.evaluate("x^2 + 1");
  console.log("Result of x^2 + 1:", result1);

  // Evaluate a range of values
  const xs = [1, 2, 3, 4, 5];
  const results2 = solver.evaluate_range("x", xs, "2*x + 1", false);
  console.log("Results for 2*x + 1 over", xs, ":", results2);

  // Declare a constant and use it
  solver.declare_constant("pi", 3.141592653589793);
  const result3 = solver.evaluate("sin(pi / 2) + f(x)", false);
  console.log("Result of sin(pi / 2) + f(x):", result3);

  // Invalid variable name, should throw the message: Invalid variable name '1'.
  try {
    // Call some Solver method that might throw.
    solver.declare_variable("1", 10);
  } catch (ex) {
    // ex is a raw pointer number; call our helper to get a message.
    console.error("C++ Exception: " + getException(ex));
  }

  solver.delete()
  console.log("Solver deleted")
};

main().catch((err) => {
  console.error("Error: ", err);
});
