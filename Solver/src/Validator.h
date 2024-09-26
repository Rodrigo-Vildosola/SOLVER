// Validator.h
#pragma once

#include "pch.h"

namespace Validator {
    /**
     * @brief Validates whether a given name is a valid identifier.
     * 
     * The rules are similar to Python variable naming conventions:
     * - Must start with a letter (A-Z, a-z) or underscore (_)
     * - Subsequent characters can be letters, digits (0-9), or underscores
     * 
     * @param name The name to validate.
     * @return true If the name is valid.
     * @return false If the name is invalid.
     */
    bool isValidName(const std::string& name);

    /**
     * @brief Validate the syntax of the given expression.
     * 
     * @param expression The expression to validate.
     */
    void isValidSyntax(const std::string& expression);
}
