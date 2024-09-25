#include "SymbolTable.h"
#include "Validator.h"

// Declare a constant, ensuring no variable with the same name exists and the name is valid
void SymbolTable::declareConstant(const std::string& name, double value) {
    // Validate the name
    if (!Validator::isValidName(name)) {
        throw SolverException("Invalid constant name '" + name + "'. Names must start with a letter or underscore and contain only letters, digits, or underscores, and must not be reserved keywords.");
    }

    if (constants.find(name) != constants.end()) {
        throw SolverException("Cannot redeclare constant '" + name + "'. Constants cannot be redeclared.");
    }

    if (variables.find(name) != variables.end()) {
        throw SolverException("Cannot declare constant '" + name + "' because a variable with the same name already exists.");
    }

    constants[name] = value;
}


// Declare a variable, ensuring no constant with the same name exists and the name is valid
void SymbolTable::declareVariable(const std::string& name, double value) {
    // Validate the name
    if (!Validator::isValidName(name)) {
        throw SolverException("Invalid variable name '" + name + "'. Names must start with a letter or underscore and contain only letters, digits, or underscores, and must not be reserved keywords.");
    }

    // Check for name collision with constants
    if (constants.find(name) != constants.end()) {
        throw SolverException("Cannot declare variable '" + name + "' because a constant with the same name already exists.");
    }

    variables[name] = value;
}

// Lookup a symbol in the table (checks both variables and constants)
double SymbolTable::lookupSymbol(const std::string& name) const {
    if (variables.find(name) != variables.end()) {
        return variables.at(name);
    } else if (constants.find(name) != constants.end()) {
        return constants.at(name);
    } else {
        throw SolverException("Unknown symbol: '" + name + "'");
    }
}

// Clear all variables (constants remain unaffected)
void SymbolTable::clearVariables() {
    variables.clear();
}

// Return a copy of the current variables (for backup/restore purposes)
std::unordered_map<std::string, double> SymbolTable::getVariables() const {
    return variables;
}

// Restore the variables from a saved copy
void SymbolTable::restoreVariables(const std::unordered_map<std::string, double>& savedVariables) {
    variables = savedVariables;
}

// Check if a symbol is a constant
bool SymbolTable::isConstant(const std::string& name) const {
    return constants.find(name) != constants.end();
}

// Check if a symbol is a variable
bool SymbolTable::isVariable(const std::string& name) const {
    return variables.find(name) != variables.end();
}
