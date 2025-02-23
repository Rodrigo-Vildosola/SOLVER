#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "solver.h"
#include "exception.h"
#include <type_traits>


using namespace emscripten;

// Original function pointer types (unchanged)
using EvaluateForRangeSig = std::vector<double> (Solver::*)(
    const std::string&,
    const std::vector<double>&,
    const std::string&,
    bool
);

using EvaluateForRangesSig = std::vector<double> (Solver::*)(
    const std::vector<std::string>&,
    const std::vector<std::vector<double>>&,
    const std::string&,
    bool
);

// This helper receives an exception pointer as an integer
// and returns the exception's message (via std::exception::what()).
std::string getExceptionMessage(int exceptionPtr) {
  try {
    // Re-throw the pointer, then catch it as a std::exception*
    throw reinterpret_cast<std::exception*>(exceptionPtr);
  } catch (std::exception* e) {
    std::cout << "I am here" << std::endl;
    return e->what();
  } catch (...) {
    return "Unknown exception";
  }
}

// Generic function to convert a JS array (val) to a std::vector<T>.
// If T supports a memory view (typically numeric types), we use the optimized conversion.
template<typename T>
typename std::enable_if<internal::typeSupportsMemoryView<T>(), std::vector<T>>::type
jsArrayToVector(const val& v) {
    // For numeric types, use convertJSArrayToNumberVector which leverages typed arrays.
    return convertJSArrayToNumberVector<T>(v);
}

// For types that do not support memory views (e.g. std::string), use a simple loop.
template<typename T>
typename std::enable_if<!internal::typeSupportsMemoryView<T>(), std::vector<T>>::type
jsArrayToVector(const val& v) {
    return vecFromJSArray<T>(v);
}

// Generic function to convert a std::vector<T> to a native JS array (val).
// For numeric types, we create a typed memory view and then use Array.from() to produce a native array.
// For other types, we fall back to creating an empty JS array and pushing each element.
template<typename T>
val vectorToJSArray(const std::vector<T>& vec) {
    if constexpr (internal::typeSupportsMemoryView<T>()) {
        // Create a typed memory view on the C++ vector's data.
        val memoryView = val(typed_memory_view(vec.size(), vec.data()));
        // Use Array.from() to convert the typed array into a normal JS Array.
        return val::global("Array").call<val>("from", memoryView);
    } else {
        // For non-numeric types, construct the JS array element by element.
        val jsArray = val::array();
        for (const auto& x : vec) {
            jsArray.call<void>("push", x);
        }
        return jsArray;
    }
}

/**
 * Wrapper for declareFunction:
 * Accepts a JS array for the argument names and converts it to a std::vector<std::string>.
 */
void declareFunctionWrapper(Solver& solver, const std::string& name, const val& jsArgs, const std::string& expression) {
    // Convert a JS array to a std::vector<std::string>
    std::vector<std::string> args = jsArrayToVector<std::string>(jsArgs);
    solver.declareFunction(name, args, expression);
}

/**
 * Wrapper for evaluateForRange:
 * Accepts a JS array for the values, converts it to std::vector<double>,
 * and returns the result as a plain JS array.
 */
val evaluateForRangeWrapper(Solver& solver, const std::string& variable, const val& jsValues, const std::string& expression, bool debug) {
    // Convert the JS array to std::vector<double>
    std::vector<NUMBER_TYPE> values = jsArrayToVector<NUMBER_TYPE>(jsValues);
    std::vector<NUMBER_TYPE> result = solver.evaluateForRange(variable, values, expression, debug);

    return vectorToJSArray(result);
}

/**
 * Wrapper for evaluateForRanges:
 * Accepts a JS array for variable names and a JS array of arrays for the values,
 * converts them, and returns the result as a plain JS array.
 */
val evaluateForRangesWrapper(Solver& solver, const val& jsVariables, const val& jsValuesSets, const std::string& expression, bool debug) {
    // Convert the JS array of variable names to std::vector<std::string>
    std::vector<std::string> variables = jsArrayToVector<std::string>(jsVariables);
    
    // For the valuesSets, each element is a JS array; convert each one.
    std::vector<std::vector<NUMBER_TYPE>> valuesSets;
    int outerLength = jsValuesSets["length"].as<int>();
    for (int i = 0; i < outerLength; i++) {
        val innerArray = jsValuesSets[i];
        std::vector<NUMBER_TYPE> inner = jsArrayToVector<NUMBER_TYPE>(innerArray);
        valuesSets.push_back(inner);
    }
    
    std::vector<NUMBER_TYPE> result = solver.evaluateForRanges(variables, valuesSets, expression, debug);
    // Manually convert the result to a native JS array:
    return vectorToJSArray(result);
}


EMSCRIPTEN_BINDINGS(solver_module) {
    // Register std::vector types so they are automatically converted to/from JS arrays.
    register_vector<double>("VectorDouble");
    register_vector<float>("VectorFloat");
    register_vector<std::string>("VectorString");
    register_vector<std::vector<double>>("VectorOfVectorDouble");

    function("getExceptionMessage", &getExceptionMessage);

    // Bind the Solver class.
    class_<Solver>("Solver")
        .constructor<size_t>()
        .function("declare_constant", &Solver::declareConstant)
        .function("declare_variable", &Solver::declareVariable)
        // Replace the original functions with our wrappers:
        .function("declare_function", &declareFunctionWrapper)
        .function("evaluate", &Solver::evaluate)
        .function("evaluate_range", &evaluateForRangeWrapper)
        .function("evaluate_ranges", &evaluateForRangesWrapper)
        .function("clear_cache", &Solver::clearCache)
        .function("use_cache", &Solver::setUseCache)
        ;
}
