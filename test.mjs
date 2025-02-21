// test.mjs
import SolverModule from './solver_js.js';

// Emscripten with MODULARIZE=1 exports a default function 
// that returns a promise resolving to the module instance.
SolverModule().then((solverModule) => {
  // "Solver" is the class_ we exposed in EMSCRIPTEN_BINDINGS
  const Solver = solverModule.Solver;

  // Create a solver instance with an LRU cache size of 100
  const solver = new Solver(100);

  // Declare a variable and evaluate an expression
  solver.declareVariable("x", 3);
  const result1 = solver.evaluate("x^2 + 1");
  console.log("Result of x^2 + 1:", result1);

  // // Evaluate a range of values
  // const xs = [0, 1, 2, 3, 4];
  // const results2 = solver.evaluate_range("x", xs, "2*x + 1", false);
  // console.log("Results for 2*x + 1 over", xs, ":", results2);

  // Declare a constant and use it
  solver.declareConstant("pi", 3.141592653589793);
  const result3 = solver.evaluate("sin(pi / 2)");
  console.log("Result of sin(pi/2):", result3);
}).catch(err => {
  console.error("Failed to load solver_js module:", err);
});
