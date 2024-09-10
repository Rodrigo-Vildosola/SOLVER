%module solver

%{
#define SWIG_FILE_WITH_INIT
#include "Core/Solver.h"
#include "Core/ExprNode.h"
#include "Core/Token.h"
%}

%include <exception.i>
%include "std_string.i"
%include "std_vector.i"

// Specialize std::vector<std::string> for Python list conversion
%template(StringVector) std::vector<std::string>;

// Specialize std::vector<Token> for Python list conversion
%template(TokenVector) std::vector<Token>;

// Typemap for converting a Python list of tuples (TokenType, string) to std::vector<Token>
%typemap(in, numinputs=1) std::vector<Token> (PyObject* obj) {    
    if ($input == nullptr) {  // Use $input instead of obj or obj2
        SWIG_exception_fail(SWIG_TypeError, "Received a null object.");
    }

    if (!PyList_Check($input)) {  // Again, use $input here
        SWIG_exception_fail(SWIG_TypeError, "Expected a list of tokens (tuple: (TokenType, string))");
    }

    Py_ssize_t size = PyList_Size($input);  // Use $input instead of obj or obj2

    $1.reserve(size); // Reserve space in the C++ vector

    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* tuple = PyList_GetItem($input, i);  // Use $input here
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

// Typemap for returning ExprNode* from C++ to Python
%typemap(out) ExprNode* {
    if ($1 == nullptr) {
        Py_RETURN_NONE;
    } else {
        $result = SWIG_NewPointerObj($1, SWIGTYPE_p_ExprNode, SWIG_POINTER_OWN);  // SWIG will manage memory
    }
}

// Include the Solver class definition
%include "Core/Solver.h"
%include "Core/ExprNode.h"
%include "Core/Token.h"
