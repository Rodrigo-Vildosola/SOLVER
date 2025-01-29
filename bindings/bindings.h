#pragma once
#include <pybind11/pybind11.h>

// Function declarations for bindings
void bind_solver(pybind11::module_& m);

void bind_test(pybind11::module_& m);
