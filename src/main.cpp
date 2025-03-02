#include <iostream>
#include "solver.h"

// Example predefined function: square(x) = x * x
NUMBER_TYPE squareFunction(const std::vector<NUMBER_TYPE>& args) {
    if (args.size() != 1) {
        throw std::runtime_error("Function 'square' requires exactly one argument.");
    }
    return args[0] * args[0];
}

int main() {
    // Define an environment for variables
    Solver solver;

    auto roots = solver.solveEquation("2 * x ^ 2 == 10", "x");
    std::cout << "Real Roots:" << std::endl;
    for (double r : roots) {
        std::cout << "x = " << r << std::endl;
    }

    return 0;
}
