#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <iostream>

// ==================
// 🔹 Define Precision Level
// ==================
// Change NUMBER_TYPE to float, double, or long double

#if defined(PREFER_FLOAT)
    #define NUMBER_TYPE float
    #define NUMBER_TYPE_STRING "float"
#elif defined(PREFER_DOUBLE)
    #define NUMBER_TYPE double
    #define NUMBER_TYPE_STRING "double"
#else
    #define NUMBER_TYPE long double
    #define NUMBER_TYPE_STRING "long double"
#endif

// ==================
// 🔹 Precision Configuration
// ==================
constexpr int PRECISION_DIGITS = std::numeric_limits<NUMBER_TYPE>::digits10 + 10;  // Max digits + buffer

// ==================
// 🔹 String Conversions
// ==================

// Converts NUMBER_TYPE to a high-precision string
inline std::string numberToString(NUMBER_TYPE value) {
    std::ostringstream oss;
    oss << std::setprecision(PRECISION_DIGITS) << std::scientific << value;
    return oss.str();
}

// Converts string to NUMBER_TYPE safely
inline NUMBER_TYPE stringToNumber(const std::string& str) {
    std::istringstream iss(str);
    NUMBER_TYPE value;
    iss >> value;
    return value;
}

// ==================
// 🔹 Debug Info
// ==================
inline void printPrecisionInfo() {
    std::cout << "Precision Type: " << NUMBER_TYPE_STRING << "\n";
    std::cout << "Digits of Precision: " << PRECISION_DIGITS << "\n";
    std::cout << "Machine Epsilon: " << std::numeric_limits<NUMBER_TYPE>::epsilon() << "\n";
}

