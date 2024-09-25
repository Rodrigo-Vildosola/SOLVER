%module solver

%{
#define SWIG_FILE_WITH_INIT
#include "Solver.h"
#include "ExprNode.h"
#include "Token.h"
#include "Exception.h"

// Declare the Python exception object for SolverException
static PyObject* pSolverException;
%}

%include <exception.i>
%include "std_string.i"
%include "std_vector.i"

// Specialize std::vector<std::string> for Python list conversion
%template(StringVector) std::vector<std::string>;

// Specialize std::vector<Token> for Python list conversion
%template(TokenVector) std::vector<Token>;

%template(DoubleVector) std::vector<double>;


// Typemap to convert a Python list of floats to a std::vector<double>
%typemap(in) std::vector<double> (PyObject* obj) {
    if (!PyList_Check($input)) {
        SWIG_exception_fail(SWIG_TypeError, "Expected a Python list of floats");
    }

    Py_ssize_t size = PyList_Size($input);
    $1.reserve(size);

    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* item = PyList_GetItem($input, i);
        if (PyFloat_Check(item)) {
            double val = PyFloat_AsDouble(item);
            $1.push_back(val);
        } else if (PyLong_Check(item)) {
            double val = PyLong_AsDouble(item);
            $1.push_back(val);
        } else {
            SWIG_exception_fail(SWIG_TypeError, "List elements must be float or int");
        }
    }
}

// Typemap to return a std::vector<double> from C++ to Python as a list of floats
%typemap(out) std::vector<double> {
    PyObject* list = PyList_New($1.size());
    for (size_t i = 0; i < $1.size(); ++i) {
        PyObject* item = PyFloat_FromDouble($1[i]);
        PyList_SetItem(list, i, item);
    }
    $result = list;
}

// Typemap for converting a Python list of tuples (TokenType, string) to std::vector<Token>
%typemap(in, numinputs=1) std::vector<Token> (PyObject* obj) {    
    if ($input == nullptr) {
        SWIG_exception_fail(SWIG_TypeError, "Received a null object.");
    }

    if (!PyList_Check($input)) {
        SWIG_exception_fail(SWIG_TypeError, "Expected a list of tokens (tuple: (TokenType, string))");
    }

    Py_ssize_t size = PyList_Size($input);

    $1.reserve(size); // Reserve space in the C++ vector

    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* tuple = PyList_GetItem($input, i);
        if (!PyTuple_Check(tuple) || PyTuple_Size(tuple) != 2) {
            SWIG_exception_fail(SWIG_TypeError, "Each token must be a tuple of (TokenType, string)");
        }

        PyObject* pyType = PyTuple_GetItem(tuple, 0);
        if (!PyLong_Check(pyType)) {
            SWIG_exception_fail(SWIG_TypeError, "Expected an int for token type");
        }
        TokenType type = static_cast<TokenType>(PyLong_AsLong(pyType));

        PyObject* pyValue = PyTuple_GetItem(tuple, 1);
        if (!PyUnicode_Check(pyValue)) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a string for token value");
        }
        std::string value = PyUnicode_AsUTF8(pyValue);

        $1.push_back(Token{type, value});
    }
}

// Typemap for returning std::vector<Token> from C++ to Python
%typemap(out) std::vector<Token> {
    PyObject* list = PyList_New($1.size());
    for (size_t i = 0; i < $1.size(); ++i) {
        PyObject* item = Py_BuildValue("(is)", (int)$1[i].type, $1[i].value.c_str());
        PyList_SetItem(list, i, item);
    }
    $result = list;
}

%init %{
    pSolverException = PyErr_NewException("_solver.SolverException", NULL, NULL);
    Py_INCREF(pSolverException);
    PyModule_AddObject(m, "SolverException", pSolverException);
%}

// Exception handling logic for C++ exceptions
%exception {
    try {
        $action
    } catch (SolverException &e) {
        PyErr_SetString(pSolverException, e.what());
        SWIG_fail;
    } catch (const std::out_of_range &e) {
        SWIG_exception(SWIG_IndexError, ("Index out of range: " + std::string(e.what())).c_str());
    } catch (const std::bad_alloc &e) {
        SWIG_exception(SWIG_MemoryError, ("Memory allocation failed: " + std::string(e.what())).c_str());
    } catch (const std::exception &e) {
        SWIG_exception(SWIG_RuntimeError, ("Standard exception: " + std::string(e.what())).c_str());
    } catch (...) {
        SWIG_exception(SWIG_UnknownError, "An unknown error occurred");
    }
}

// Include the Solver class definition
%include "Solver.h"
%include "Token.h"

%pythoncode %{
class SolverException(Exception):
    def __init__(self, message):
        super().__init__(message)

SolverException = _SOLVER_PYTHON.SolverException
%}
