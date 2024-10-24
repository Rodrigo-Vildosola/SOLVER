// SymbolTable.h
#pragma once

#include "pch.h"
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
    std::unordered_map<std::string, double> getConstants() const;


    void restoreVariables(const std::unordered_map<std::string, double>& savedVariables);

    // Utility methods for checks
    bool isConstant(const std::string& name) const;
    bool isVariable(const std::string& name) const;

    std::shared_ptr<SymbolTable> clone() const {
        auto newTable = std::make_shared<SymbolTable>();
        newTable->constants = this->constants;  // Constants are immutable, so shallow copy is fine
        newTable->variables = this->variables;  // Variables will be independently manipulated
        return newTable;
    }

private:
    std::unordered_map<std::string, double> constants;
    std::unordered_map<std::string, double> variables;
};
