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
    solver.declareVariable("x", 5.0);
    solver.declareFunction("f", {"x"}, "x^2 + 2*x + pi");

    std::cout << solver.evaluate("x + 0", true) << std::endl;
    std::cout << "Result: " << solver.evaluate("f(2) + pi", true) << std::endl;  // Should replace 'pi' with 3.14159


    // List constants
    // auto constants = solver.listConstants();
    // for (const auto& [name, value] : constants) {
    //     std::cout << "Constant: " << name << " = " << value << std::endl;
    // }

    // std::cout << std::endl;

    // // List variables
    // auto variables = solver.listVariables();
    // for (const auto& [name, value] : variables) {
    //     std::cout << "Variable: " << name << " = " << value << std::endl;
    // }

    // std::cout << std::endl;

    // // List functions
    // auto functions = solver.listFunctions();
    // for (const auto& [name, func] : functions) {
    //     const auto& [args, isPredefined] = func;
    //     std::cout << "Function: " << name << " (Predefined: " << (isPredefined ? "Yes" : "No") << ")";
    //     std::cout << " Args: ";
    //     for (const auto& arg : args) {
    //         std::cout << arg << " ";
    //     }
    //     std::cout << std::endl;
    // }


}
