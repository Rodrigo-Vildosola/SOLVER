#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "solver.h"
#include "exception.h"
#include "bindings.h"
#include <type_traits>


using namespace emscripten;

// This helper receives an exception pointer as an integer
// and returns the exception's message (via SolverException::what()).
std::string getExceptionMessage(int exceptionPtr) {
    return reinterpret_cast<SolverException*>(exceptionPtr)->what();
}

val getException(int exceptionPtr) {
    // Reinterpret the integer as a pointer to SolverException.
    SolverException* exception = reinterpret_cast<SolverException*>(exceptionPtr);

    if (!exception) {
        return val::global("Error").new_(std::string("Null exception pointer received."));
    }

    // Create a JavaScript Error object with the exception message.
    val jsError = val::global("Error").new_(std::string(exception->what()));

    // Attach additional metadata
    jsError.set("cppType", std::string(typeid(*exception).name())); // Exception type
    jsError.set("message", std::string(exception->what())); // Actual message
    jsError.set("cppExceptionPointer", exceptionPtr); // Original pointer (for debugging)

    // Simulate a C++ stack trace (only useful if integrated with a logging mechanism)
    std::stringstream trace;
    trace << "C++ Exception: " << typeid(*exception).name() << "\n";
    trace << "Message: " << exception->what() << "\n";
    trace << "Pointer: " << exceptionPtr;
    
    jsError.set("stack", trace.str());

    return jsError;
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
    return vectorToJSArray(result);
}


val listConstantsWrapper(const Solver& solver) {
  auto c = solver.listConstants();
  return mapStringDoubleToJSObject(c);
}

val listVariablesWrapper(const Solver& solver) {
  auto v = solver.listVariables();
  return mapStringDoubleToJSObject(v);
}

val listFunctionsWrapper(const Solver& solver) {
  auto v = solver.listFunctions();
  return vectorToJSArray(v);
}

val generateAnimationDataWrapper(Solver& solver,
                                 const std::string& expression,
                                 const std::string& variable,
                                 double start,
                                 double end,
                                 size_t steps) {
  auto [xValues, yValues] = solver.generateAnimationData(expression, variable, start, end, steps);

  val out = val::object();
  out.set("x", vectorToJSArray(xValues));
  out.set("y", vectorToJSArray(yValues));
  return out;
}

val generateContourDataWrapper(Solver& solver,
                               const std::string& expression,
                               const std::string& variable1,
                               const std::string& variable2,
                               double start1,
                               double end1,
                               size_t steps1,
                               double start2,
                               double end2,
                               size_t steps2) {
  auto [xVals, yVals, zVals] = solver.generateContourData(expression, variable1, variable2,
                                                          start1, end1, steps1,
                                                          start2, end2, steps2);
  val out = val::object();
  out.set("x", vectorToJSArray(xVals));
  out.set("y", vectorToJSArray(yVals));
  out.set("z", matrixToJSArray(zVals));
  return out;
}

void bind_solver() {
    class_<Solver>("Solver")
        .constructor<size_t>()

        .function("declareConstant", &Solver::declareConstant)
        .function("declareVariable", &Solver::declareVariable)
        .function("declareFunction", &declareFunctionWrapper)

        .function("evaluate", &Solver::evaluate)
        .function("evaluateRange", &evaluateForRangeWrapper)
        .function("evaluateRanges", &evaluateForRangesWrapper)

        .function("clearCache", &Solver::clearCache)
        .function("useCache", &Solver::setUseCache)
        .function("reset", &Solver::reset)

        .function("printFunctionExpressions", &Solver::printFunctionExpressions)

        .function("listConstants", &listConstantsWrapper)
        .function("listVariables", &listVariablesWrapper)
        .function("listFunctions", &listFunctionsWrapper)

        .function("dump", &Solver::dumpState)
        .function("load", &Solver::loadState)

        .function("setCurrentExpression", &Solver::setCurrentExpression)
        .function("getCurrentExpression", &Solver::getCurrentExpression)

        // Functions returning tuples need wrappers to produce JS objects:
        .function("generateAnimationData", &generateAnimationDataWrapper)
        .function("generateContourData", &generateContourDataWrapper)
        ;
}
