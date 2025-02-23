// index.ts
import createSolverModule from './_solver';
// We import the default from solver_js.js (the Emscripten factory).

// Also re-export the types from solver.d.ts:
export type {
  SolverModule,
  SolverInstance,
} from './_solver.d';

// Finally, export the factory so users can do: import { createSolverModule } from './solver'
export { createSolverModule };

