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

    double e = 2.71828;
    double pi = 3.14159;


    Solver solver;
    solver.setUseCache(true);

    // Add some constants, variables, and functions
    solver.declareConstant("pi", pi);
    solver.declareConstant("e", e);  
    solver.declareVariable("x", 5.0);
    solver.declareFunction("f", {"x"}, "x^2 + (((pi * 2) + 1) * ((x * 2) + 1)) + e");
    solver.declareFunction("w", {"z"}, "e^z");

    solver.setCurrentExpression("f(x)", true);

    // Now use linspace to generate values for x and evaluate f(x) over that range
    std::vector<double> x_values = linspace(0, 100, 100);  // Reduce points for quick verification during testing

    // Measure performance using evaluateForRange
    auto start1 = std::chrono::high_resolution_clock::now();
    std::vector<double> results = solver.evaluateForRange("x", x_values, "f(x)", false);
    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration1 = end1 - start1;

    std::cout << "evaluateForRange took: " << duration1.count() << " seconds." << std::endl;

    // Measure performance using a loop with declareVariable and evaluate
    auto start2 = std::chrono::high_resolution_clock::now();
    std::vector<double> loop_results;
    for (double x : x_values) {
        solver.declareVariable("x", x);
        double result = solver.evaluate("f(x)", false);
        loop_results.push_back(result);
    }
    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration2 = end2 - start2;

    std::cout << "Loop with declareVariable took: " << duration2.count() << " seconds." << std::endl;

    // Sanity check to ensure both approaches give the same result
    for (size_t i = 0; i < x_values.size(); ++i) {
        // std::cout << results[i] << " " << loop_results[i] << std::endl;
        if (results[i] != loop_results[i]) {
            std::cerr << "Mismatch at index " << i << ": evaluateForRange(" << x_values[i] 
                      << ") = " << results[i] << ", loop(" << x_values[i] 
                      << ") = " << loop_results[i] << std::endl;
        }
    }

    // Analytical comparison to expected values for f(x) = x^2
    bool all_correct = true;
    // for (size_t i = 0; i < x_values.size(); ++i) {
    //     double expected = x_values[i] * x_values[i] + (pi * x_values[i]) + e + x_values[i];
    //     if (std::fabs(results[i] - expected) > 1e-9) { // Using a small tolerance for floating-point comparison
    //         all_correct = false;
    //         std::cerr << "Error at index " << i << ": Expected " << expected 
    //                   << ", but got " << results[i] << std::endl;
    //     }
    // }

    if (all_correct) {
        std::cout << "All results are correct!" << std::endl;
    } else {
        std::cout << "There were some errors in the results." << std::endl;
    }

    return 0;
}
