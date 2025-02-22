// test.mjs
import { createSolverModule, SolverModule, arrayToVector, vectorToArray } from './solver';

async function main() {
  // "Solver" is the class_ we exposed in EMSCRIPTEN_BINDINGS
  const mod: SolverModule = await createSolverModule();

  const { Solver, VectorString, VectorDouble } = mod;

  // Create a solver instance with an LRU cache size of 100
  const solver = new Solver(100);

  const argVector = arrayToVector(VectorString, ["x"]);

  solver.declare_function("f", argVector, "x + 8");

  // Declare a variable and evaluate an expression
  solver.declare_variable("x", 3);
  const result1 = solver.evaluate("x^2 + 1");
  console.log("Result of x^2 + 1:", result1);

  // Evaluate a range of values
  const xs = [1, 2, 3, 4, 5];
  const xsVector = arrayToVector(VectorDouble, xs);
  const results2 = solver.evaluate_range("x", xsVector, "2*x + 1", false);
  const resultsArray = vectorToArray<number, typeof results2>(results2);
  console.log("Results for 2*x + 1 over", xs, ":", resultsArray);

  // Declare a constant and use it
  solver.declare_constant("pi", 3.141592653589793);
  const result3 = solver.evaluate("sin(pi / 2) + f(x)", true);
  console.log("Result of sin(pi / 2) + f(x):", result3);
};

main().catch(console.error);
