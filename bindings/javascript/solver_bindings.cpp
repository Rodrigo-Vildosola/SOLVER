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
    class_<Solver>("Solver")
        .constructor<size_t>()
        
        // Basic solver usage
        .function("declare_constant", &Solver::declareConstant)
        .function("declare_variable", &Solver::declareVariable)
        .function("evaluate", &Solver::evaluate)
        
        // Additional methods
        .function(
            "evaluate_range",
            &Solver::evaluateForRange
        )
        .function(
            "evaluate_ranges",
            &Solver::evaluateForRanges
        )
        
        .function("clear_cache", &Solver::clearCache)
        .function("use_cache", &Solver::setUseCache)
        ;
}
