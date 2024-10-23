#include <iostream>
#include "Solver.h"
#include <cmath>
#include <iomanip>


void testEquation(Solver& solver, const std::string& equation) {
    std::cout << "Testing equation: " << equation << std::endl;

    double evaluatedValue = solver.evaluate(equation);
    std::cout << "Evaluated expression value: " << evaluatedValue << std::endl;
    std::cout << "--------------------------" << std::endl << std::endl;
}

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
    Solver solver;

    // Add some constants, variables, and functions
    solver.declareConstant("pi", 3.14159);
    solver.declareConstant("e", 2.71828);  
    solver.declareVariable("x", 5.0);
    solver.declareFunction("f", {"x"}, "x^2");
    solver.declareFunction("w", {"z"}, "e^z");

    // Test a single evaluation for debugging purposes
    std::cout << solver.evaluate("x + 0", true) << std::endl;
    std::cout << "Result: " << solver.evaluate("f(pi) + pi", true) << std::endl;
    std::cout << "Result: " << solver.evaluate("w(x)", true) << std::endl;

    // Now use linspace to generate values for x and evaluate f(x) over that range
    std::vector<double> x_values = linspace(0, 12, 5);
    std::vector<double> results = solver.evaluateForRange("x", x_values, "f(x)", true);

    // Print the results for each value of x
    std::cout << "Evaluating f(x) over the range [0, 10]:" << std::endl;
    for (size_t i = 0; i < x_values.size(); ++i) {
        std::cout << "f(" << x_values[i] << ") = " << results[i] << std::endl;
    }

    return 0;
}
