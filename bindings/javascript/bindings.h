#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <type_traits>
#include "solver.h"
#include "exception.h"

using namespace emscripten;

// This helper receives an exception pointer as an integer
// and returns the exception's message (via SolverException::what()).
std::string getExceptionMessage(int exceptionPtr);

void bind_solver();

template<typename T>
typename std::enable_if<internal::typeSupportsMemoryView<T>(), std::vector<T>>::type
jsArrayToVector(const val& v) {
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
        val memoryView = val(typed_memory_view(vec.size(), vec.data()));
        return val::global("Array").call<val>("from", memoryView);
    } else {
        val jsArray = val::array();
        for (const auto& x : vec) {
            jsArray.call<void>("push", x);
        }
        return jsArray;
    }
}

/**
 * Utility: Convert a 2D vector (vector<vector<T>>) -> JS Array of Arrays
 */
template<typename T>
val matrixToJSArray(const std::vector<std::vector<T>>& mat) {
  val outer = val::array();
  for (auto& row : mat) {
    outer.call<void>("push", vectorToJSArray(row));
  }
  return outer;
}

/**
 * Utility: Convert an unordered_map<std::string, double> -> a JS object { key: value, ... }
 */
inline val mapStringDoubleToJSObject(const std::unordered_map<std::string, double>& data) {
  val obj = val::object();
  for (auto& kv : data) {
    obj.set(kv.first, kv.second);
  }
  return obj;
}


/**
 * The *opposite* direction: Create a C++ lambda that calls into a JS function "val callback",
 * converting the input vector<double> to a JS array, calling callback, 
 * and returning callback's result as a double.
 */
inline std::function<double(const std::vector<double>&)> makeFunctionCallbackFromJS(val jsFunc) {
  return [jsFunc](const std::vector<double>& input) -> double {
    val jsArray = vectorToJSArray(input);
    val result = jsFunc(jsArray);
    return result.as<double>();
  };
}
