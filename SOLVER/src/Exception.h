#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "pch.h"

class SolverException : public std::runtime_error {
public:
    explicit SolverException(const std::string &message) : std::runtime_error(message) {}
};

#endif // EXCEPTION_H
