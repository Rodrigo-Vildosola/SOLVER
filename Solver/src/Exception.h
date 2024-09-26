#pragma once

#include "pch.h"

/**
 * @class SolverException
 * @brief Custom exception class for handling errors in the Solver class.
 * 
 * The SolverException class is derived from std::runtime_error and is thrown when
 * the Solver encounters an error during expression evaluation or function declaration.
 */
class SolverException : public std::runtime_error {
public:
    /**
     * @brief Constructor for the SolverException class.
     * 
     * @param message The error message to display when the exception is thrown.
     */
    explicit SolverException(const std::string &message) : std::runtime_error(message) {}
};

