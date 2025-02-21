#include <emscripten/bind.h>
#include "solver.h"
#include "exception.h"

using namespace emscripten;

// Example: Expose a subset of your API. You can follow similar patterns as with Pybind11.
EMSCRIPTEN_BINDINGS(solver_module) {
    class_<Solver>("Solver")
        .constructor<size_t>()
        .function("declareConstant", &Solver::declareConstant)
        .function("declareVariable", &Solver::declareVariable)
        .function("evaluate", &Solver::evaluate)
        // … add other methods as desired …
        ;

}
