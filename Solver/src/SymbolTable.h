// SymbolTable.h
#pragma once

#include "pch.h"
#include "Exception.h"

enum class SymbolType {
    CONSTANT,
    VARIABLE
};

struct SymbolEntry {
    double value;
    SymbolType type;

    SymbolEntry() : value(0.0), type(SymbolType::VARIABLE) {}  // Default constructor with initial values
    SymbolEntry(double val, SymbolType t) : value(val), type(t) {}
};


class SymbolTable {
public:
    // Methods to declare constants and variables
    void declareConstant(const std::string& name, double value);
    void declareVariable(const std::string& name, double value);
    // Lookup for a symbol (constant or variable)
    double lookupSymbol(const std::string& name) const;

    void clearVariables();
    void restoreVariables(const std::unordered_map<std::string, double>& savedVariables);

    std::unordered_map<std::string,double> getConstants() const;

    std::unordered_map<std::string,double> getVariables() const;

    std::unordered_map<std::string, SymbolEntry> getEntries() const;

    std::shared_ptr<SymbolTable> clone() const {
        return std::make_shared<SymbolTable>(*this); // Shallow copy for Copy-on-Write
    }

    // Utility methods for checks
    bool isConstant(const std::string& name) const;
    bool isVariable(const std::string& name) const;

private:
    std::unordered_map<std::string, SymbolEntry> entries; // Unified storage for both constants and variables

    // Small cache for frequently accessed symbols
    mutable std::string cachedSymbolName;
    mutable SymbolEntry cachedSymbolEntry;
};
