#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <string>

class SolverException : public std::runtime_error {
public:
    explicit SolverException(const std::string &message) : std::runtime_error(message) {}
};

#endif // EXCEPTION_H
