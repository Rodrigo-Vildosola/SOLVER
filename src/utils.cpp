#include <nlohmann/json.hpp>

#include "solver.h"
#include "utils.h"
#include "tokenizer.h"
#include "debug.h"
#include "postfix.h"
#include "helpers/arena_node_factory.h"
#include "helpers/builder.h"

using namespace nlohmann;
using namespace Expression;


std::string Solver::dumpState() const {
    json j;

    j["constants"] = symbolTable.getConstants();
    j["variables"] = symbolTable.getVariables();

    json jsonFunctions;
    for (const auto& pair : functions) {
        const std::string& fname = pair.first;
        const Function &func = pair.second;

        if (!func.isPredefined) {
            json funcJson;
            funcJson["arguments"] = func.argumentNames;
            funcJson["expression"] = func.expression;

            jsonFunctions[fname] = funcJson;
        }
    }

    j["functions"] = jsonFunctions;
    
    return j.dump(2);
}

void Solver::loadState(const std::string& dump) {
    json j = json::parse(dump);

    // Clear existing state
    symbolTable.clear();
    functions.clear();

    // Load constants
    if (j.contains("constants")) {
        auto constants = j["constants"].get<Env>();
        for (const auto &pair : constants) {
            declareConstant(pair.first, pair.second);
        }
    }

    // Load variables
    if (j.contains("variables")) {
        auto variables = j["variables"].get<Env>();
        for (const auto &pair : variables) {
            declareVariable(pair.first, pair.second);
        }
    }

    // Load user-defined functions
    if (j.contains("functions")) {
        for (auto it = j["functions"].begin(); it != j["functions"].end(); ++it) {
            std::string fname = it.key();
            json funcJson = it.value();

            // Get argument names
            std::vector<std::string> argNames = funcJson["arguments"].get<std::vector<std::string>>();
            std::string expression = funcJson["expression"].get<std::string>();


            declareFunction(fname, argNames, expression);
        }
    }
}


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


std::vector<std::string> Solver::listFunctions() const {
    std::vector<std::string> functionList;

    for (const auto& pair : functions) {
        const std::string& functionName = pair.first;
        const Function& func = pair.second;

        // Skip predefined functions
        if (func.isPredefined) {
            continue;
        }

        // Create function signature: f(x, y, z) =
        std::string functionStr = functionName + "(";
        for (size_t i = 0; i < func.argumentNames.size(); ++i) {
            functionStr += func.argumentNames[i];
            if (i < func.argumentNames.size() - 1) {
                functionStr += ", ";
            }
        }
        functionStr += ") = ";

        // Append the original expression
        functionStr += func.expression;

        // Store the formatted function string
        functionList.push_back(functionStr);
    }

    return functionList;
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




NUMBER_TYPE Solver::evaluateAST(const std::string &expression, bool debug)
{
    PROFILE_FUNCTION();
    setCurrentExpressionAST(expression, debug);

    std::size_t cacheKey = generateCacheKey(expression, {});
    if (cacheEnabled) {
        if (NUMBER_TYPE* cachedResult = expressionCache.get(cacheKey)) {
            // std::cout << "AST cache hit!" << std::endl;
            return *cachedResult;  // Return cached result if found
        }
    }

    if (!currentAST) {
        throw SolverException("Cannot evaluate AST pipeline: currentAST is null.");
    }

    Env env = symbolTable.getVariables();

    // Evaluate the final AST
    NUMBER_TYPE result = 0.0;
    try {
        result = currentAST->evaluate(env);
    }
    catch (const SolverException &e) {
        throw; // or handle differently
    }

    return result;
}


Node* Solver::parseAST(const std::string& expression, NodeFactory& f, bool debug) {    
    auto tokens   = Tokenizer::tokenize(expression);
    auto postfix  = Postfix::shuntingYard(tokens);
    auto flattened = Postfix::flattenPostfix(postfix, functions);
    auto inlined = replaceConstantSymbols(flattened, symbolTable);

    Node * root = AST::buildASTFromPostfix(inlined, f);

    Node * simplified = simplifyAST(root, f);

    if (debug) {
        std::cout << "Simplified AST: " << simplified->toString() << "\n";
    }

    return simplified; 
}


void Solver::setCurrentExpressionAST(const std::string &expression, bool debug) {
    DECLARE_ARENA_FACTORY(f);

    if (expression == currentExpressionAST && currentAST != nullptr) {
        return; // no need to rebuild
    }

    // Store the expression
    currentExpressionAST = expression;

    // If we had an old AST, free it
    if (currentAST) {
        currentAST = nullptr;
    }

    try {
        // store it
        currentAST = parseAST(expression, f, debug); // 'root' is no longer valid after the simplification returns the new root

    }
    catch (const SolverException &e) {
        // If there's an error, ensure we don't leave a partial AST
        if (currentAST) {
            currentAST = nullptr;
        }
        // rethrow 
        throw;
    }
}

