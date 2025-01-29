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
        .def("print_function_expressions", 
             &Solver::printFunctionExpressions, 
             DOC(Solver, printFunctionExpressions))

        .def("declare_constant", 
             &Solver::declareConstant,
             py::arg("name"),
             py::arg("value"),
             DOC(Solver, declareConstant))

        .def("declare_variable", 
             &Solver::declareVariable,
             py::arg("name"),
             py::arg("value"),
             DOC(Solver, declareVariable))

        .def("evaluate",
             &Solver::evaluate, 
             py::arg("expression"),
             py::arg("debug") = false,
             DOC(Solver, evaluate))

        .def("evaluate_range",
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

        .def("declare_function",
             &Solver::declareFunction,
             py::arg("name"),
             py::arg("args"),
             py::arg("expression"),
             DOC(Solver, declareFunction))

        .def("clear_cache", 
             &Solver::clearCache,
             DOC(Solver, clearCache))

        .def("use_cache", 
             &Solver::setUseCache,
             py::arg("useCache"),
             DOC(Solver, setUseCache))

        .def("list_constants", 
             &Solver::listConstants,
             DOC(Solver, listConstants))

        .def("list_variables", 
             &Solver::listVariables,
             DOC(Solver, listVariables))

        .def("set_current_expression",
             &Solver::setCurrentExpression,
             py::arg("expression"),
             py::arg("debug") = false,
             DOC(Solver, setCurrentExpression))

        .def("get_current_expression", 
             &Solver::getCurrentExpression, 
             DOC(Solver, getCurrentExpression));
}
