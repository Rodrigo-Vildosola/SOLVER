#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "solver.h"
#include "exception.h"
#include "misc.h"
#include "bindings.h"


EMSCRIPTEN_BINDINGS(solver_module) {
    emscripten::function("getExceptionMessage", &getExceptionMessage);

    emscripten::function("version", &version);

    // Bind the Solver class.
    bind_solver();
}
