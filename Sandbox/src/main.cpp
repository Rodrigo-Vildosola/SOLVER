#include <iostream>
#include "Solver.h"
#include <cmath>
#include <iomanip>
#include <chrono>  
#include <vector>
#include <cassert>

std::vector<double> linspace(double start, double end, int num_points) {
    std::vector<double> points;
    if (num_points <= 0) return points;

    double step = (end - start) / (num_points - 1);
    for (int i = 0; i < num_points; ++i) {
        points.push_back(start + i * step);
    }
    return points;
}

int main() {
    // Initialize constants for the Solver instance
    double e = 2.71828;
    double pi = 3.14159;

    Solver solver;
    solver.setUseCache(true);

    // Declare constants and variables
    solver.declareConstant("pi", pi);
    solver.declareConstant("e", e);  
    solver.declareVariable("x", 5.0);
    solver.declareVariable("y", 10.0);
    solver.declareVariable("z", 15.0);

    // Expressions and expected results
    std::vector<std::pair<std::string, double>> testExpressions = {
        {"x + 0", 5.0},               // Simplifies to x, value should be 5
        {"0 + x", 5.0},               // Simplifies to x, value should be 5
        {"x * 1", 5.0},               // Simplifies to x, value should be 5
        {"1 * x", 5.0},               // Simplifies to x, value should be 5
        {"x * 0", 0.0},               // Simplifies to 0, value should be 0
        {"0 * x", 0.0},               // Simplifies to 0, value should be 0
        {"x - 0", 5.0},               // Simplifies to x, value should be 5
        {"x / 1", 5.0},               // Simplifies to x, value should be 5
        {"x ^ 1", 5.0},               // Simplifies to x, value should be 5
        {"x ^ 0", 1.0},               // Simplifies to 1, value should be 1
        {"2 + 3", 5.0},               // Constant folding, should be 5
        {"4 * 5", 20.0},              // Constant folding, should be 20
        {"(x + 0) * (1 + 2)", 15.0},  // x+0 simplifies to x, 1+2 to 3, then x*3 = 15
        {"x * (y + z)", 125.0},       // Should be x*(y+z) = 5*(10+15) = 125
        {"3 * (x + 4)", 27.0},        // Should distribute to 3x + 12 = 15 + 12 = 27
        {"e ^ 1", e},                 // Simplifies to e
        {"(x^2) * (x^3)", 3125.0},    // Simplifies to x^(2+3) -> x^5 = 3125 for x=5
        {"sin(x)^2 + cos(x)^2", 1.0}     // Simplifies to x^(2+3) -> x^5 = 3125 for x=5

    };

    // Loop through test expressions and simplify them
    bool all_correct = true;
    for (const auto& [expr, expected] : testExpressions) {
        std::cout << "Original expression: " << expr << std::endl;
        solver.setCurrentExpression(expr, true);

        double result = solver.evaluate(expr, true);  // Set debug to true to see simplification steps

        // Check if the result matches the expected value
        if (std::fabs(result - expected) > 1e-9) {  // Using a small tolerance for floating-point comparison
            all_correct = false;
            std::cerr << "Mismatch for expression: " << expr 
                      << "\nExpected: " << expected 
                      << "\nGot: " << result 
                      << std::endl;
        } else {
            std::cout << "Simplified result: " << result << " (Correct)" << std::endl;
        }

        std::cout << "------------------------------------------" << std::endl;
    }

    if (all_correct) {
        std::cout << "All expressions simplified correctly!" << std::endl;
    } else {
        std::cout << "Some expressions did not simplify as expected." << std::endl;
    }

    return 0;
}
