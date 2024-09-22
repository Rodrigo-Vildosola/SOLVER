#include "SymbolTable.h"

void SymbolTable::declareConstant(const std::string& name, double value) {
    constants[name] = value;
}

void SymbolTable::declareVariable(const std::string& name, double value) {
    variables[name] = value;
}

double SymbolTable::lookupSymbol(const std::string& name) const {
    if (variables.find(name) != variables.end()) {
        return variables.at(name);
    } else if (constants.find(name) != constants.end()) {
        return constants.at(name);
    } else {
        throw SolverException("Unknown symbol: '" + name + "'");
    }
}

void SymbolTable::clearVariables() {
    variables.clear();
}
