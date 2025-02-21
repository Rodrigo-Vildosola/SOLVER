#include <emscripten/bind.h>
#include "solver.h"
#include "exception.h"

using namespace emscripten;

// If NUMBER_TYPE is long double internally, but you want to expose double to JS,
// ensure your function definitions match the correct type (std::vector<double>).
// Also note that if you have genuine function overloads, you can use static_cast<>.

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

EMSCRIPTEN_BINDINGS(solver_module) {
    // Register std::vector<double> so it’s understood as an array in JS.
    register_vector<double>("VectorDouble");
    register_vector<float>("VectorFloat");

    // If you pass vectors of vectors or vectors of strings, also register them:
    register_vector<std::string>("VectorString");
    register_vector<std::vector<double>>("VectorOfVectorDouble");

    class_<Solver>("Solver")
        .constructor<size_t>()
        
        .function("declare_constant", &Solver::declareConstant)
        .function("declare_variable", &Solver::declareVariable)
        .function("declare_function", &Solver::declareFunction)

        .function("evaluate", &Solver::evaluate)

        .function("evaluate_range", &Solver::evaluateForRange)
        .function("evaluate_ranges", &Solver::evaluateForRanges)
        
        .function("clear_cache", &Solver::clearCache)
        .function("use_cache", &Solver::setUseCache)
        ;
}
