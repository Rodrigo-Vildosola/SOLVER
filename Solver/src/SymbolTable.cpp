#include "SymbolTable.h"
#include "Validator.h"

// Declare a constant, ensuring no variable with the same name exists and the name is valid
void SymbolTable::declareConstant(const std::string& name, double value) {
    if (!Validator::isValidName(name)) {
        throw SolverException("Invalid constant name '" + name + "'.");
    }

    auto it = entries.find(name);
    if (it != entries.end() && it->second.type == SymbolType::CONSTANT) {
        throw SolverException("Constant '" + name + "' already declared.");
    }
    entries[name] = SymbolEntry(value, SymbolType::CONSTANT);
}


// Declare a variable, ensuring no constant with the same name exists and the name is valid
void SymbolTable::declareVariable(const std::string& name, double value) {
    if (!Validator::isValidName(name)) {
        throw SolverException("Invalid variable name '" + name + "'.");
    }

    // Invalidate the cache if the variable being declared is cached
    if (cachedSymbolName == name) {
        cachedSymbolName.clear();  
    }

    auto it = entries.find(name);
    if (it != entries.end() && it->second.type == SymbolType::CONSTANT) {
        throw SolverException("Cannot declare variable '" + name + "', constant with the same name exists.");
    }
    entries[name] = SymbolEntry(value, SymbolType::VARIABLE);
}


// Lookup a symbol in the table (checks both variables and constants)
double SymbolTable::lookupSymbol(const std::string& name) const {
    if (cachedSymbolName == name) {
        return cachedSymbolEntry.value; 
    }

    auto it = entries.find(name);
    if (it != entries.end()) {
        // Cache the result
        cachedSymbolName = name;
        cachedSymbolEntry = it->second;
        return it->second.value;
    }

    throw SolverException("Unknown symbol: '" + name + "'");
}

// Clear all variables (constants remain unaffected)
void SymbolTable::clearVariables() {
    // Invalidate cache when clearing variables
    cachedSymbolName.clear();  // Clear cached name to invalidate the cache
    
    for (auto it = entries.begin(); it != entries.end();) {
        if (it->second.type == SymbolType::VARIABLE) {
            it = entries.erase(it); // Erase variable
        } else {
            ++it; // Keep constants
        }
    }
}



// Restore the variables from a saved copy
void SymbolTable::restoreVariables(const std::unordered_map<std::string, double>& savedVariables) {
    // Invalidate cache when restoring variables
    cachedSymbolName.clear();  // Clear cached name to invalidate the cache

    for (const auto& [name, value] : savedVariables) {
        entries[name] = SymbolEntry(value, SymbolType::VARIABLE);  // Assume every entry is a variable
    }
}



// Check if a symbol is a constant
bool SymbolTable::isConstant(const std::string& name) const {
    auto it = entries.find(name);
    return it != entries.end() && it->second.type == SymbolType::CONSTANT;
}

bool SymbolTable::isVariable(const std::string& name) const {
    auto it = entries.find(name);
    return it != entries.end() && it->second.type == SymbolType::VARIABLE;
}



// Return a copy of the current constants (for listing purposes)
std::unordered_map<std::string, double> SymbolTable::getConstants() const {
    std::unordered_map<std::string, double> constants;
    for (const auto& [name, entry] : entries) {
        if (entry.type == SymbolType::CONSTANT) {
            constants[name] = entry.value;
        }
    }
    return constants;
}

// Return a copy of the current variables (for listing purposes)
std::unordered_map<std::string, double> SymbolTable::getVariables() const {
    std::unordered_map<std::string, double> variables;
    for (const auto& [name, entry] : entries) {
        if (entry.type == SymbolType::VARIABLE) {
            variables[name] = entry.value;
        }
    }
    return variables;
}

// Return a copy of all entries (both constants and variables)
std::unordered_map<std::string, SymbolEntry> SymbolTable::getEntries() const {
    return entries;  // Return the entire entries map
}

