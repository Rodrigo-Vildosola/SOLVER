#include "solver.h"
#include "tokenizer.h"
#include "debug.h"


// Register standard math functions
void Solver::registerBuiltInFunctions() {
    PROFILE_FUNCTION()
    registerPredefinedFunction("neg", [](const std::vector<NUMBER_TYPE>& args) -> NUMBER_TYPE {
        return -args[0];
    }, 1);

    registerPredefinedFunction("sin", [](const std::vector<NUMBER_TYPE>& args) -> NUMBER_TYPE {
        return std::sinl(args[0]);
    }, 1);

    registerPredefinedFunction("cos", [](const std::vector<NUMBER_TYPE>& args) -> NUMBER_TYPE {
        return std::cosl(args[0]);
    }, 1);

    registerPredefinedFunction("tan", [](const std::vector<NUMBER_TYPE>& args) -> NUMBER_TYPE {
        return std::tanl(args[0]);
    }, 1);

    registerPredefinedFunction("exp", [](const std::vector<NUMBER_TYPE>& args) -> NUMBER_TYPE {
        return std::expl(args[0]);
    }, 1);

    // Natural logarithm function (ln)
    registerPredefinedFunction("ln", [](const std::vector<NUMBER_TYPE>& args) -> NUMBER_TYPE {
        return std::logl(args[0]);
    }, 1);

    // Logarithm with specified base
    registerPredefinedFunction("log", [](const std::vector<NUMBER_TYPE>& args) -> NUMBER_TYPE {
        return std::logl(args[0]) / std::logl(args[1]); // log base args[1] of args[0]
    }, 2);

    registerPredefinedFunction("sqrt", [](const std::vector<NUMBER_TYPE>& args) -> NUMBER_TYPE {
        return std::sqrtl(args[0]);
    }, 1);

    registerPredefinedFunction("abs", [](const std::vector<NUMBER_TYPE>& args) -> NUMBER_TYPE {
        return std::abs(args[0]);
    }, 1);

    registerPredefinedFunction("max", [](const std::vector<NUMBER_TYPE>& args) -> NUMBER_TYPE {
        return std::max(args[0], args[1]);
    }, 2);

    registerPredefinedFunction("min", [](const std::vector<NUMBER_TYPE>& args) -> NUMBER_TYPE {
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



std::size_t Solver::generateCacheKey(const std::string& base, const std::vector<NUMBER_TYPE>& args) {
    std::size_t hash = std::hash<std::string>{}(base);

    // Hash the function or expression arguments
    for (const auto& arg : args) {
        hash ^= std::hash<NUMBER_TYPE>{}(arg) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }

    return hash;  // Return the hashed key as an integer instead of converting to string
}

void Solver::printFunctionExpressions() {
    PROFILE_FUNCTION()

    std::cout << "=== Solver Functions Report ===" << std::endl;
    for (const auto& [name, function] : functions) {
        if (function.isPredefined) continue;
        std::cout << "Function Name: " << name << std::endl;
        std::cout << "  Arguments: ";
        if (function.argumentNames.empty()) {
            std::cout << "None";
        } else {
            for (const auto& arg : function.argumentNames) {
                std::cout << arg << " ";
            }
        }
        std::cout << std::endl;

        std::cout << "  Type: " << (function.isPredefined ? "Predefined" : "User-defined") << std::endl;

        if (!function.isPredefined) {
            std::cout << "  Postfix Expression: ";
            printPostfix(function.inlinedPostfix);
        } else {
            std::cout << "  Predefined Callback: Yes" << std::endl;
        }

        std::cout << std::endl;
    }
    std::cout << "=== End of Report ===" << std::endl;
}



