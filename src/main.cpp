#include "solver.h"

int main() {
    // Define an environment for variables
    Solver solver;
    solver.declareFunction("f", {"x"}, "30 + x");
    auto roots = solver.solveEquation("f(3) * x ^ 2 == 10", "x");
    std::cout << "Real Roots:" << std::endl;
    for (double r : roots) {
        std::cout << "x = " << r << std::endl;
    }

    return 0;
}
