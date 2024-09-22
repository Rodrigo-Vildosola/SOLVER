#pragma once

#include <unordered_map>
#include <string>
#include "Exception.h"

class SymbolTable {
public:
    // Methods to declare constants and variables
    void declareConstant(const std::string& name, double value);
    void declareVariable(const std::string& name, double value);

    // Lookup for a symbol (constant or variable)
    double lookupSymbol(const std::string& name) const;

    // Variable management methods
    void clearVariables();
    std::unordered_map<std::string, double> getVariables() const;
    void restoreVariables(const std::unordered_map<std::string, double>& savedVariables);

    // Utility methods for checks
    bool isConstant(const std::string& name) const;
    bool isVariable(const std::string& name) const;

private:
    std::unordered_map<std::string, double> constants;
    std::unordered_map<std::string, double> variables;
};
