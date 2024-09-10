%module solver

%{
#define SWIG_FILE_WITH_INIT
#include "Core/Solver.h"
%}

%include <exception.i>
%include "std_string.i"
%include "std_vector.i"

// Specialize std::vector<std::string> for Python list conversion
%template(StringVector) std::vector<std::string>;

// Specialize std::vector<Token> for Python list conversion
%template(TokenVector) std::vector<Token>;


%typemap(out) std::vector<Token> {
    PyObject *list = PyList_New($1.size());
    for (size_t i = 0; i < $1.size(); ++i) {
        PyObject *item = Py_BuildValue("(is)", (int)$1[i].type, $1[i].value.c_str());
        PyList_SetItem(list, i, item);
    }
    $result = list;
}


// Include the Solver class definition
%include "Core/Solver.h"

// Expose the Solver class and only the necessary methods
%{
    // You can include any helper code here if needed
%}

%pythoncode %{
    # Optional: Add any Python-specific code if necessary
%}
