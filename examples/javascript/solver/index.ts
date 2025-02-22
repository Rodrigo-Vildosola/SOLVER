// index.ts
import createSolverModule from './_solver';
// We import the default from solver_js.js (the Emscripten factory).

// Also re-export the types from solver.d.ts:
export type {
  SolverModule,
  SolverInstance,
  VectorDouble,
  VectorString,
  VectorFloat,
  VectorOfVectorDouble
} from './_solver.d';

// Finally, export the factory so users can do: import { createSolverModule } from './solver'
export { createSolverModule };

/**
 * Converts a plain JavaScript array to an Embind vector.
 *
 * @param VectorConstructor - The constructor for the vector type (e.g. mod.VectorString or mod.VectorDouble).
 * @param items - The JavaScript array to convert.
 * @returns An instance of the Embind vector with all items added.
 */
export function arrayToVector<T>(
  VectorConstructor: { new(): { push_back(item: T): void } },
  items: T[]
): InstanceType<typeof VectorConstructor> {
  const vector = new VectorConstructor();
  for (const item of items) {
    vector.push_back(item);
  }
  return vector;
}
