#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "solver.h"
#include "exception.h"
#include "bindings.h"


EMSCRIPTEN_BINDINGS(solver_module) {
    emscripten::function("getExceptionMessage", &getExceptionMessage);

    // Bind the Solver class.
    bind_solver();
}
