#pragma once

// Define precision level here
// Options: float, double, long double, __float128 (if supported)
#define NUMBER_TYPE long double

// Better string conversion (high precision output)
#include <sstream>
#include <iomanip>

inline std::string numberToString(NUMBER_TYPE value) {
    std::ostringstream oss;
    oss << std::setprecision(30) << value;  // Increase precision dynamically
    return oss.str();
}
