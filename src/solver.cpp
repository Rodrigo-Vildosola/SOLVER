#include "solver.h"
#include "exception.h"
#include "postfix.h"
#include "validator.h"
#include "debug.h"
#include "tokenizer.h"
#include "ast.h"
#include "compile.h"

Solver::Solver(size_t exprCacheSize)
    : expressionCache(exprCacheSize) {
    PROFILE_BEGIN_SESSION("SOLVER", "solver.json");
    registerBuiltInFunctions();
}

void Solver::setUseCache(bool useCache) {
    PROFILE_FUNCTION()
    cacheEnabled = useCache;
}

void Solver::invalidateCaches() {
    PROFILE_FUNCTION()
    if (cacheEnabled) {
        expressionCache.clear();
    }
}

void Solver::clearCache() {
    PROFILE_FUNCTION()
    expressionCache.clear();
}

void Solver::declareConstant(const std::string& name, NUMBER_TYPE value) {
    PROFILE_FUNCTION()
    symbolTable.declareConstant(name, value);
    invalidateCaches();
}

void Solver::declareVariable(const std::string& name, NUMBER_TYPE value) {
    PROFILE_FUNCTION()
    symbolTable.declareVariable(name, value);
    invalidateCaches();
}

#pragma region Parsing

std::vector<Token> Solver::parse(const std::string& expression, bool debug) {
    auto tokens   = Tokenizer::tokenize(expression);
    auto postfix  = Postfix::shuntingYard(tokens);
    auto flattened = Postfix::flattenPostfix(postfix, functions);
    auto inlined = Simplification::replaceConstantSymbols(flattened, symbolTable);

    // Now do a simplification pass
    auto simplified = Simplification::fullySimplifyPostfix(inlined, functions);

    if (debug) {
        std::cout << "Flattened postfix: ";
        printPostfix(inlined);
        std::cout << "Simplified postfix: ";
        printPostfix(simplified);
    }

    return simplified; 
}

ASTNode* Solver::parseAST(const std::string& expression, bool debug) {
    auto tokens   = Tokenizer::tokenize(expression);
    auto postfix  = Postfix::shuntingYard(tokens);
    auto flattened = Postfix::flattenPostfix(postfix, functions);
    auto inlined = Simplification::replaceConstantSymbols(flattened, symbolTable);

    ASTNode * root = AST::buildASTFromPostfix(inlined, functions);

    ASTNode * simplified = Simplification::simplifyAST(root, functions);

    if (debug) {
        std::cout << "Flattened AST: ";
        AST::printAST(root);
        std::cout << "Simplified AST: ";
        AST::printAST(simplified);
    }


    return simplified; 
}

#pragma endregion

#pragma region Evaluation

NUMBER_TYPE Solver::evaluate(const std::string& expression, bool debug) {
    PROFILE_FUNCTION()
    setCurrentExpression(expression, debug);

    std::size_t cacheKey = generateCacheKey(expression, {});
    if (cacheEnabled) {
        if (NUMBER_TYPE* cachedResult = expressionCache.get(cacheKey)) {
            return *cachedResult;  // Return cached result if found
        }
    }

    NUMBER_TYPE result = Postfix::evaluatePostfix(currentPostfix, symbolTable, functions);

    if (cacheEnabled) {
        expressionCache.put(cacheKey, result);  // Cache the result
    }

    return result;
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

    // Evaluate the final AST
    NUMBER_TYPE result = 0.0;
    try {
        result = AST::evaluateAST(currentAST, symbolTable, functions);
    }
    catch (const SolverException &e) {
        throw; // or handle differently
    }

    return result;
}


std::vector<NUMBER_TYPE> Solver::evaluateForRange(const std::string& variable, const std::vector<NUMBER_TYPE>& values, const std::string& expression, bool debug) {
    PROFILE_FUNCTION()
    setCurrentExpression(expression, debug);

    std::vector<NUMBER_TYPE> results;
    results.reserve(values.size());

    if (!Validator::isValidName(variable)) {
        throw SolverException("Invalid variable name '" + variable + "'.");
    }

    NUMBER_TYPE* varPtr = symbolTable.getVariablePtr(variable);

    for (NUMBER_TYPE value : values) {
        PROFILE_SCOPE("EvaluateRangeLoop");
        *varPtr = value;

        try {
            NUMBER_TYPE result = Postfix::evaluatePostfix(currentPostfix, symbolTable, functions);
            results.push_back(result);
        } catch (const SolverException& e) {
            std::cerr << "Error evaluating expression for " << variable << " = " << value << ": " << e.what() << std::endl;
            results.push_back(std::nan(""));
        }
    }

    return results;
}

std::vector<NUMBER_TYPE> Solver::evaluateForRanges(const std::vector<std::string>& variables, const std::vector<std::vector<NUMBER_TYPE>>& valuesSets, const std::string& expression, bool debug) {
    PROFILE_FUNCTION()

    // Basic validation:
    if (variables.size() != valuesSets.size()) {
        throw SolverException("Mismatch in number of variables vs. value ranges.");
    }
    for (auto& var : variables) {
        if (!Validator::isValidName(var)) {
            throw SolverException("Invalid variable name '" + var + "'.");
        }
    }

    // Parse expression only once for efficiency
    setCurrentExpression(expression, debug);

    // Compute the total number of combinations in the cartesian product
    // E.g., if x has 3 values and y has 2, totalCombinations = 3 * 2 = 6.
    size_t totalCombinations = 1;
    for (const auto& vals : valuesSets) {
        totalCombinations *= vals.size();
    }

    // Prepare output vector
    std::vector<NUMBER_TYPE> results;
    results.reserve(totalCombinations);

    // Get direct pointers to all variables (auto-create if missing)
    const size_t nVars = variables.size();
    std::vector<NUMBER_TYPE*> varPtrs(nVars);
    for (size_t i = 0; i < nVars; ++i) {
        varPtrs[i] = symbolTable.getVariablePtr(variables[i]);
    }

    // Indices for tracking cartesian product iteration
    std::vector<size_t> indices(nVars, 0);

    // We'll iterate from combination #0 to combination #(totalCombinations-1).
    // For each iteration, assign symbolTable values and evaluate.
    for (size_t count = 0; count < totalCombinations; ++count) {
        // Assign each variable to its current index's value
        for (size_t i = 0; i < nVars; ++i) {
            *varPtrs[i] = valuesSets[i][indices[i]]; // Direct memory write
        }

        // Evaluate and capture the result
        try {
            NUMBER_TYPE val = Postfix::evaluatePostfix(currentPostfix, symbolTable, functions);
            results.push_back(val);
        } catch (const SolverException& e) {
            // On error, store NaN to keep indices consistent
            std::cerr << "Error evaluating expression for combination "
                      << (count+1) << " of " << totalCombinations << ": "
                      << e.what() << std::endl;
            results.push_back(std::nan(""));
        }

        // Increment the 'indices' in a multi-digit manner (the last variable changes fastest)
        for (int varIndex = static_cast<int>(nVars) - 1; varIndex >= 0; --varIndex) {
            indices[varIndex]++;
            if (indices[varIndex] < valuesSets[varIndex].size()) {
                // We successfully incremented without overflow; break
                break;
            } else {
                // Reset this index to 0 and carry over to the next variable
                indices[varIndex] = 0;
            }
        }
    }

    return results;
}

#pragma endregion

#pragma region Functions

void Solver::registerPredefinedFunction(const std::string& name, const FunctionCallback& callback, size_t argCount) {
    PROFILE_FUNCTION()
    auto result = functions.emplace(name, Function(callback, argCount));
    if (!result.second) {
        throw SolverException("Function '" + name + "' already exists.");
    }
}

void Solver::declareFunction(const std::string& name, const std::vector<std::string>& args, const std::string& expression) {
    PROFILE_FUNCTION()
    if (!Validator::isValidName(name)) {
        throw SolverException("Invalid function name: '" + name + "'.");
    }

    Validator::isValidSyntax(expression);

    try {
        // Tokenize and convert the function body to postfix
        auto tokens = Tokenizer::tokenize(expression);
        auto postfix = Postfix::shuntingYard(tokens);
        auto flattened = Postfix::flattenPostfix(postfix, functions);
        
        // std::cout << "This is the postfix of the expression: " << expression << std::endl;
        // printPostfix(flattened);

        // Store the function with its inlined postfix and argument names
        functions[name] = Function(flattened, args);
    } catch (const std::exception& e) {
        throw SolverException("Error defining function '" + name + "': " + e.what());
    }
}

#pragma endregion

#pragma region Helpers

// Return the list of constants
std::unordered_map<std::string, NUMBER_TYPE> Solver::listConstants() const {
    return symbolTable.getConstants();
}

// Return the list of variables
std::unordered_map<std::string, NUMBER_TYPE> Solver::listVariables() const {
    return symbolTable.getVariables();
}

void Solver::setCurrentExpression(const std::string& expression, bool debug) {
    // Check if the expression is the same and the currentPostfix is not empty
    if (expression == currentExpressionPostfix && !currentPostfix.empty()) {
        return;
    }

    // Otherwise, parse the new expression into postfix
    currentExpressionPostfix = expression;
    currentPostfix = parse(expression, debug); 

    if (debug) {
        std::cout << "Current expression set to: " << expression << std::endl;
    }
}

void Solver::setCurrentExpressionAST(const std::string &expression, bool debug) {
    if (expression == currentExpressionAST && currentAST != nullptr) {
        return; // no need to rebuild
    }

    // Store the expression
    currentExpressionAST = expression;

    // If we had an old AST, free it
    if (currentAST) {
        delete currentAST;
        currentAST = nullptr;
    }

    try {
        // store it
        currentAST = parseAST(expression, debug); // 'root' is no longer valid after the simplification returns the new root

    }
    catch (const SolverException &e) {
        // If there's an error, ensure we don't leave a partial AST
        if (currentAST) {
            delete currentAST;
            currentAST = nullptr;
        }
        // rethrow 
        throw;
    }
}


#pragma endregion
