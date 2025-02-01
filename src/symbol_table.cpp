#include "symbol_table.h"
#include "validator.h"

// Declare a constant (error if already declared or if a variable exists with the same name)
void SymbolTable::declareConstant(const std::string& name, NUMBER_TYPE value) {
    if (!Validator::isValidName(name)) {
        throw SolverException("Invalid constant name '" + name + "'.");
    }
    if (constants.find(name) != constants.end()) {
        throw SolverException("Constant '" + name + "' already declared.");
    }
    if (variableIndex.find(name) != variableIndex.end()) {  // NEW CHECK: Prevent variable-constant name conflict
        throw SolverException("Cannot declare constant '" + name + "', variable with the same name exists.");
    }

    constants[name] = value;
}


// Declare a variable (stored in a vector for pointer stability)
void SymbolTable::declareVariable(const std::string& name, NUMBER_TYPE value, bool skipCheck) {
    if (!skipCheck && !Validator::isValidName(name)) {
        throw SolverException("Invalid variable name '" + name + "'.");
    }
    if (constants.find(name) != constants.end()) {
        throw SolverException("Cannot declare variable '" + name + "', constant with the same name exists.");
    }

    auto it = variableIndex.find(name);
    if (it == variableIndex.end()) {
        // New variable → Add to vector and map
        variableIndex[name] = variables.size();
        variables.emplace_back(value, SymbolType::VARIABLE);
    } else {
        // Existing variable → Update value
        variables[it->second].value = value;
    }

    // Invalidate cache
    cachedSymbolName.clear();
}

// Fast direct access to a variable's value (unsafe but fast)
NUMBER_TYPE* SymbolTable::getVariablePtr(const std::string& name) {
    auto it = variableIndex.find(name);
    if (it == variableIndex.end()) {
        throw SolverException("Variable not declared: " + name);
    }
    return &variables[it->second].value;
}

// Lookup a symbol (checks both variables and constants)
NUMBER_TYPE SymbolTable::lookupSymbol(const std::string& name) const {
    if (cachedSymbolName == name) {
        return cachedSymbolValue;
    }

    auto constIt = constants.find(name);
    if (constIt != constants.end()) {
        cachedSymbolName = name;
        cachedSymbolValue = constIt->second;
        return constIt->second;
    }

    auto varIt = variableIndex.find(name);
    if (varIt != variableIndex.end()) {
        cachedSymbolName = name;
        cachedSymbolValue = variables[varIt->second].value;
        return cachedSymbolValue;
    }

    throw SolverException("Unknown symbol: '" + name + "'");
}

// Clear all variables but keep constants
void SymbolTable::clearVariables() {
    variables.clear();
    variableIndex.clear();
    cachedSymbolName.clear();
}

// Restore variables from a saved copy
void SymbolTable::restoreVariables(const std::unordered_map<std::string, NUMBER_TYPE>& savedVariables) {
    clearVariables();
    for (const auto& [name, value] : savedVariables) {
        declareVariable(name, value, true);
    }
}


// Check if a name is a constant
bool SymbolTable::isConstant(const std::string& name) const {
    return constants.find(name) != constants.end();
}

// Check if a name is a variable
bool SymbolTable::isVariable(const std::string& name) const {
    return variableIndex.find(name) != variableIndex.end();
}


// Get a copy of current constants
std::unordered_map<std::string, NUMBER_TYPE> SymbolTable::getConstants() const {
    return constants;
}

// Get a copy of current variables
std::unordered_map<std::string, NUMBER_TYPE> SymbolTable::getVariables() const {
    std::unordered_map<std::string, NUMBER_TYPE> vars;
    for (const auto& [name, index] : variableIndex) {
        vars[name] = variables[index].value;
    }
    return vars;
}

