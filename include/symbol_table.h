#pragma once

#include "pch.h"
#include "exception.h"
#include "validator.h"
#include <unordered_map>
#include <vector>
#include <string>

enum class SymbolType {
    CONSTANT,
    VARIABLE
};

struct SymbolEntry {
    NUMBER_TYPE value;
    SymbolType type;

    SymbolEntry() : value(0.0), type(SymbolType::VARIABLE) {}  // Default constructor
    SymbolEntry(NUMBER_TYPE val, SymbolType t) : value(val), type(t) {}
};

class SymbolTable {
public:
    // Declare a constant (stored separately for fast lookups)
    void declareConstant(const std::string& name, NUMBER_TYPE value);

    // Declare a variable (stored in a vector for pointer stability)
    void declareVariable(const std::string& name, NUMBER_TYPE value, bool skipCheck = false);

    // Lookup a symbol (checks both variables and constants)
    NUMBER_TYPE lookupSymbol(const std::string& name) const;

    // Fast direct access to a variable's value (unsafe but fast)
    NUMBER_TYPE* getVariablePtr(const std::string& name);

    // Clears all variables but keeps constants
    void clearVariables();

    // Restore variables from a saved copy
    void restoreVariables(const std::unordered_map<std::string, NUMBER_TYPE>& savedVariables);

    // Get a copy of current constants
    std::unordered_map<std::string, NUMBER_TYPE> getConstants() const;

    // Get a copy of current variables
    std::unordered_map<std::string, NUMBER_TYPE> getVariables() const;

    // Check if a name is a constant
    bool isConstant(const std::string& name) const;

    // Check if a name is a variable
    bool isVariable(const std::string& name) const;

private:
    // Constants stored in a hash table (since they are read-only after declaration)
    std::unordered_map<std::string, NUMBER_TYPE> constants;

    // Variables stored in a vector (ensures pointer stability)
    std::vector<SymbolEntry> variables;
    
    // Maps variable names to indices in the `variables` vector
    std::unordered_map<std::string, size_t> variableIndex;

    // Small cache for frequently accessed symbols
    mutable std::string cachedSymbolName;
    mutable NUMBER_TYPE cachedSymbolValue;
};
