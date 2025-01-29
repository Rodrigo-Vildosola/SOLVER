#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "solver.h"
#include "exception.h"
#include "docstrings.h"

namespace py = pybind11;

void bind_solver(py::module_ &m) {
    // Register the custom exception type so that Python code can catch SolverException
    py::register_exception<SolverException>(m, "SolverException");

    // Expose the Solver class to Python
    py::class_<Solver>(m, "Solver", DOC(Solver))
        // Constructor
        .def(py::init<size_t>(), 
             py::arg("exprCacheSize") = 100,
             DOC(Solver, Solver))  // If docstrings are defined for constructor

        // Expose methods
        .def("printFunctionExpressions", 
             &Solver::printFunctionExpressions, 
             DOC(Solver, printFunctionExpressions))

        .def("declareConstant", 
             &Solver::declareConstant,
             py::arg("name"),
             py::arg("value"),
             DOC(Solver, declareConstant))

        .def("declareVariable", 
             &Solver::declareVariable,
             py::arg("name"),
             py::arg("value"),
             DOC(Solver, declareVariable))

        .def("evaluate",
             &Solver::evaluate, 
             py::arg("expression"),
             py::arg("debug") = false,
             DOC(Solver, evaluate))

        .def("evaluateForRange",
             &Solver::evaluateForRange,
             py::arg("variable"),
             py::arg("values"),
             py::arg("expression"),
             py::arg("debug") = false,
             DOC(Solver, evaluateForRange))

        // For registering a predefined function in Python,
        // you would need to handle conversion of a Python callable to the C++ callback.
        // If you don't need that, you can simply omit or limit this binding.
        // .def("registerPredefinedFunction",
        //      &Solver::registerPredefinedFunction,
        //      py::arg("name"),
        //      py::arg("callback"),   // This would be a std::function<double(...)> or similar
        //      py::arg("argCount"),
        //      DOC(Solver, registerPredefinedFunction))

        .def("declareFunction",
             &Solver::declareFunction,
             py::arg("name"),
             py::arg("args"),
             py::arg("expression"),
             DOC(Solver, declareFunction))

        .def("clearCache", 
             &Solver::clearCache,
             DOC(Solver, clearCache))

        .def("setUseCache", 
             &Solver::setUseCache,
             py::arg("useCache"),
             DOC(Solver, setUseCache))

        .def("listConstants", 
             &Solver::listConstants,
             DOC(Solver, listConstants))

        .def("listVariables", 
             &Solver::listVariables,
             DOC(Solver, listVariables))

        .def("setCurrentExpression",
             &Solver::setCurrentExpression,
             py::arg("expression"),
             py::arg("debug") = false,
             DOC(Solver, setCurrentExpression))

        .def("getCurrentExpression", 
             &Solver::getCurrentExpression, 
             DOC(Solver, getCurrentExpression));
}
