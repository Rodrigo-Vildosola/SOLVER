#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <solver.h>
#include <string>
#include <functional>
#include <memory>
#include <iostream>
#include "bindings.h"
#include "misc.h"

namespace py = pybind11;

PYBIND11_MODULE(MODULE_NAME, m) {
    m.doc() = "Python bindings for the solver C++ math expression parsing and solving library.";

    bind_solver(m);

    m.def("version", &version, DOC(version));
}
