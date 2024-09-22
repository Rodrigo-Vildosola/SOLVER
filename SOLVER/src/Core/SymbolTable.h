#pragma once
#include <unordered_map>
#include <string>
#include "Exception.h"

class SymbolTable {
public:
    // Declare constants and variables
    void declareConstant(const std::string& name, double value);
    void declareVariable(const std::string& name, double value);
    
    // Lookup symbol value (either constant or variable)
    double lookupSymbol(const std::string& name) const;

    // Clear all variables (for function scoping)
    void clearVariables();

private:
    std::unordered_map<std::string, double> constants;
    std::unordered_map<std::string, double> variables;
};
