#pragma once

#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <type_traits>
#include "solver.h"
#include "exception.h"

using namespace emscripten;

// This helper receives an exception pointer as an integer
// and returns the exception's message (via SolverException::what()).
std::string getExceptionMessage(int exceptionPtr);
val getException(int exceptionPtr);


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
 * Converts a 2D std::vector (matrix) into a JavaScript array of arrays.
 */
template<typename T>
inline val matrixToJSArray(const std::vector<std::vector<T>>& mat) {
    val outer = val::array();
    for (const auto& row : mat) {
        outer.call<void>("push", vectorToJSArray(row));
    }
    return outer;
}

/**
 * Converts an unordered_map<string, double> to a native JavaScript object.
 */
inline val mapStringDoubleToJSObject(const std::unordered_map<std::string, double>& data) {
    val obj = val::object();
    for (const auto& kv : data) {
        obj.set(kv.first, kv.second);
    }
    return obj;
}


/**
 * Creates a C++ lambda that wraps a JavaScript function.
 * The lambda accepts a vector<double>, converts it to a JS array,
 * calls the JS function, and returns its result as a double.
 */
inline std::function<double(const std::vector<double>&)> makeFunctionCallbackFromJS(val jsFunc) {
    // Check that jsFunc is callable. This is a minimal check.
    if (!jsFunc.isUndefined() && !jsFunc.isNull() && jsFunc.typeOf().as<std::string>() == "function") {
        return [jsFunc](const std::vector<double>& input) -> double {
            val jsArray = vectorToJSArray(input);
            val result = jsFunc(jsArray);
            return result.as<double>();
        };
    } else {
        return [](const std::vector<double>&) -> double {
            throw std::runtime_error("Invalid JS callback provided to makeFunctionCallbackFromJS.");
        };
    }
}

// A helper macro to wrap a call in a try/catch block and rethrow a JS Error.
#define WRAP_EXCEPTIONS(returnExpr)                  \
  try {                                              \
    return returnExpr;                               \
  } catch (const SolverException& e) {               \
    throw val::global("Error").new_(std::string(e.what()));  \
  } catch (const std::exception& e) {                \
    throw val::global("Error").new_(std::string(e.what()));  \
  } catch (...) {                                    \
    throw val::global("Error").new_(std::string("Unknown C++ exception")); \
  }

// For void-returning functions
#define WRAP_EXCEPTIONS_VOID(expr)                   \
  try {                                              \
    expr;                                          \
    return;                                          \
  } catch (const SolverException& e) {               \
    throw val::global("Error").new_(std::string(e.what()));  \
  } catch (const std::exception& e) {                \
    throw val::global("Error").new_(std::string(e.what()));  \
  } catch (...) {                                    \
    throw val::global("Error").new_(std::string("Unknown C++ exception")); \
  }

