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
%typemap(in) std::vector<Token> (PyObject* obj) {
    std::cout << "Entering the typemap for std::vector<Token>..." << std::endl;
    
    if (!PyList_Check(obj)) {
        std::cout << "Error: Expected a Python list" << std::endl;
        SWIG_exception_fail(SWIG_TypeError, "Expected a list of tokens");
    }
    
    Py_ssize_t size = PyList_Size(obj);
    $1.reserve(size);  // Reserve space in the C++ vector
    
    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* tuple = PyList_GetItem(obj, i);
        if (!PyTuple_Check(tuple) || PyTuple_Size(tuple) != 2) {
            std::cout << "Error: Expected a tuple of (TokenType, string) for each token" << std::endl;
            SWIG_exception_fail(SWIG_TypeError, "Each token must be a tuple of (TokenType, string)");
        }

        // Extract the token type (first element of the tuple)
        PyObject* pyType = PyTuple_GetItem(tuple, 0);
        if (!PyLong_Check(pyType)) {
            std::cout << "Error: Expected an integer for TokenType" << std::endl;
            SWIG_exception_fail(SWIG_TypeError, "Expected an int for token type");
        }
        TokenType type = static_cast<TokenType>(PyLong_AsLong(pyType));

        // Extract the token value (second element of the tuple)
        PyObject* pyValue = PyTuple_GetItem(tuple, 1);
        if (!PyUnicode_Check(pyValue)) {
            std::cout << "Error: Expected a string for token value" << std::endl;
            SWIG_exception_fail(SWIG_TypeError, "Expected a string for token value");
        }
        std::string value = PyUnicode_AsUTF8(pyValue);

        // Print the extracted values for debugging
        std::cout << "Token Type: " << PyLong_AsLong(pyType) << " | Token Value: " << PyUnicode_AsUTF8(pyValue) << std::endl;

        // Add the Token to the C++ vector
        $1.push_back(Token{type, value});
    }

    std::cout << "Exiting the typemap for std::vector<Token>" << std::endl;
}


%typemap(out) std::vector<Token> {
    PyObject *list = PyList_New($1.size());
    for (size_t i = 0; i < $1.size(); ++i) {
        PyObject *item = Py_BuildValue("(is)", (int)$1[i].type, $1[i].value.c_str());
        PyList_SetItem(list, i, item);
    }
    $result = list;
}

%typemap(out) ExprNode* {
    $result = SWIG_NewPointerObj($1, SWIGTYPE_p_ExprNode, SWIG_POINTER_OWN);
}

// Include the Solver class definition
%include "Core/Solver.h"
%include "Core/ExprNode.h"
%include "Core/Token.h"


