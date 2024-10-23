#include "Solver.h"
#include "Tokenizer.h"



// Register standard math functions
void Solver::registerBuiltInFunctions() {
    registerPredefinedFunction("neg", [](const std::vector<double>& args) -> double {
        return -args[0];
    }, 1);

    registerPredefinedFunction("sin", [](const std::vector<double>& args) -> double {
        return std::sin(args[0]);
    }, 1);

    registerPredefinedFunction("cos", [](const std::vector<double>& args) -> double {
        return std::cos(args[0]);
    }, 1);

    registerPredefinedFunction("tan", [](const std::vector<double>& args) -> double {
        return std::tan(args[0]);
    }, 1);

    registerPredefinedFunction("exp", [](const std::vector<double>& args) -> double {
        return std::exp(args[0]);
    }, 1);

    // Natural logarithm function (ln)
    registerPredefinedFunction("ln", [](const std::vector<double>& args) -> double {
        return std::log(args[0]);
    }, 1);

    // Logarithm with specified base
    registerPredefinedFunction("log", [](const std::vector<double>& args) -> double {
        return std::log(args[0]) / std::log(args[1]); // log base args[1] of args[0]
    }, 2);

    registerPredefinedFunction("sqrt", [](const std::vector<double>& args) -> double {
        return std::sqrt(args[0]);
    }, 1);

    registerPredefinedFunction("abs", [](const std::vector<double>& args) -> double {
        return std::abs(args[0]);
    }, 1);

    registerPredefinedFunction("max", [](const std::vector<double>& args) -> double {
        return std::max(args[0], args[1]);
    }, 2);

    registerPredefinedFunction("min", [](const std::vector<double>& args) -> double {
        return std::min(args[0], args[1]);
    }, 2);

    // Add more predefined functions as needed
}


// Semantic validation: Ensure all dependencies are defined
void Solver::validateFunctionDependencies(const std::string& expression, const std::vector<std::string>& args) {
    auto tokens = Tokenizer::tokenize(expression);
    for (const auto& token : tokens) {
        if (token.type == VARIABLE) {
            // Check if the variable is a function argument or a declared constant
            if (std::find(args.begin(), args.end(), token.value) == args.end() && !symbolTable.isConstant(token.value)) {
                throw SolverException("Variable '" + token.value + "' is not declared in the function scope or as a constant.");
            }
        }
        else if (token.type == FUNCTION) {
            // Ensure that the function being called is defined
            if (functions.find(token.value) == functions.end()) {
                throw SolverException("Function '" + token.value + "' is not defined.");
            }
        }
    }
}



std::string Solver::generateCacheKey(const std::string& base, const std::vector<double>& args) {
    std::ostringstream cacheKey;
    cacheKey << base;

    // Append function arguments (or expression arguments), only if args is not empty
    if (!args.empty()) {
        cacheKey << "(";
        for (size_t i = 0; i < args.size(); ++i) {
            cacheKey << std::fixed << std::setprecision(6) << args[i];  // Consistent precision for floating-point numbers
            if (i < args.size() - 1) {
                cacheKey << ", ";
            }
        }
        cacheKey << ")";
    }

    // Append variable state
    const auto& variables = symbolTable.getVariables();
    for (const auto& [varName, varValue] : variables) {
        cacheKey << ";" << varName << "=" << std::fixed << std::setprecision(6) << varValue;  // Ensure consistent precision
    }

    return cacheKey.str();  // Convert the ostringstream to a string
}
