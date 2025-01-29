#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <solver.h>
#include <string>
#include <functional>
#include <memory>
#include <iostream>

namespace py = pybind11;

PYBIND11_MODULE(MODULE_NAME, m) {
    m.doc() = "Python bindings for the bootest C++ testing library.";

}
