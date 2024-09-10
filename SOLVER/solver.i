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

%typemap(in) std::vector<Token> (PyObject* obj) {
    std::cout << "[DEBUG] Typemap invoked for std::vector<Token>" << std::endl;
    
    if (obj == nullptr) {
        std::cout << "[ERROR] Received null object in typemap." << std::endl;
        SWIG_exception_fail(SWIG_TypeError, "Received a null object.");
    }

    if (!PyList_Check(obj)) {
        std::cout << "[ERROR] Expected a Python list but got something else." << std::endl;
        SWIG_exception_fail(SWIG_TypeError, "Expected a list of tokens (tuple: (TokenType, string))");
    }

    Py_ssize_t size = PyList_Size(obj);
    std::cout << "[DEBUG] Received list of size: " << size << std::endl;

    $1.reserve(size); // Reserve space in the C++ vector

    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* tuple = PyList_GetItem(obj, i);
        if (!PyTuple_Check(tuple) || PyTuple_Size(tuple) != 2) {
            std::cout << "[ERROR] Expected a tuple of (TokenType, string)" << std::endl;
            SWIG_exception_fail(SWIG_TypeError, "Each token must be a tuple of (TokenType, string)");
        }

        PyObject* pyType = PyTuple_GetItem(tuple, 0);
        if (!PyLong_Check(pyType)) {
            std::cout << "[ERROR] Expected an int for token type" << std::endl;
            SWIG_exception_fail(SWIG_TypeError, "Expected an int for token type");
        }
        TokenType type = static_cast<TokenType>(PyLong_AsLong(pyType));

        PyObject* pyValue = PyTuple_GetItem(tuple, 1);
        if (!PyUnicode_Check(pyValue)) {
            std::cout << "[ERROR] Expected a string for token value" << std::endl;
            SWIG_exception_fail(SWIG_TypeError, "Expected a string for token value");
        }
        std::string value = PyUnicode_AsUTF8(pyValue);

        std::cout << "[DEBUG] Adding token: " << value << " of type " << type << std::endl;
        $1.push_back(Token{type, value});
    }

    std::cout << "[DEBUG] Finished processing the token list" << std::endl;
}


// Typemap for returning std::vector<Token> from C++ to Python
%typemap(out) std::vector<Token> {
    std::cout << "[DEBUG] Entering output typemap for std::vector<Token>" << std::endl;
    PyObject* list = PyList_New($1.size());
    for (size_t i = 0; i < $1.size(); ++i) {
        PyObject* item = Py_BuildValue("(is)", (int)$1[i].type, $1[i].value.c_str());
        PyList_SetItem(list, i, item);
    }
    $result = list;
    std::cout << "[DEBUG] Exiting output typemap for std::vector<Token>" << std::endl;
}

// Typemap for returning ExprNode* from C++ to Python
%typemap(out) ExprNode* {
    std::cout << "[DEBUG] Entering output typemap for ExprNode*" << std::endl;
    if ($1 == nullptr) {
        std::cout << "[DEBUG] ExprNode* is null, returning None" << std::endl;
        Py_RETURN_NONE;
    } else {
        $result = SWIG_NewPointerObj($1, SWIGTYPE_p_ExprNode, SWIG_POINTER_OWN);  // SWIG will manage memory
        std::cout << "[DEBUG] Returning ExprNode* to Python" << std::endl;
    }
}

// Include the Solver class definition
%include "Core/Solver.h"
%include "Core/ExprNode.h"
%include "Core/Token.h"
