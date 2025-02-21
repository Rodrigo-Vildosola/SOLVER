// index.ts
import createSolverModule from './solver_js';
// We import the default from solver_js.js (the Emscripten factory).

// Also re-export the types from solver.d.ts:
export type {
  SolverModule,
  SolverInstance,
} from './solver.d';

// Finally, export the factory so users can do: import { createSolverModule } from './solver'
export { createSolverModule };
