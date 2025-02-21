// test.mjs
import { createSolverModule, SolverModule } from './solver';

async function main() {
  // "Solver" is the class_ we exposed in EMSCRIPTEN_BINDINGS
  const mod: SolverModule = await createSolverModule();

  const Solver = mod.Solver;

  // Create a solver instance with an LRU cache size of 100
  const solver = new Solver(100);

  const args = ["x", "y"];
  solver.declare_function("f", args, "x + 8 + y");

  // Declare a variable and evaluate an expression
  solver.declare_variable("x", 3);
  const result1 = solver.evaluate("x^2 + 1");
  console.log("Result of x^2 + 1:", result1);

  // Evaluate a range of values
  const xs = Array.from([1, 2, 3, 4, 5]);
  console.log("xs =", xs, "typeof xs =", typeof xs, "isArray =", Array.isArray(xs));

  const results2 = solver.evaluate_range("x", xs, "2*x + 1", false);
  console.log("Results for 2*x + 1 over", xs, ":", results2);

  // Declare a constant and use it
  solver.declare_constant("pi", 3.141592653589793);
  const result3 = solver.evaluate("sin(pi / 2)");
  console.log("Result of sin(pi/2):", result3);
};

main().catch(console.error);
