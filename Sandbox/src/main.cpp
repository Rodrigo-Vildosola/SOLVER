#include <iostream>
#include "SOLVER.h"
#include <cmath> // For NaN
#include <iomanip> // For setting precision


// void printTokens(const std::vector<Token>& tokens) {
//     for (const auto& token : tokens) {
//         std::cout << "Token: " << token.value << " Type: " << token.type << std::endl;
//     }
// }

void printTree(const ExprNode* node, int depth = 0, const std::string& prefix = "", bool isLeft = true) {
    if (node) {
        // Print the current node
        std::cout << prefix;

        // Add an appropriate connector (either a corner or a line)
        std::cout << (depth == 0 ? "" : (isLeft ? "├── " : "└── "));

        std::cout << node->value << std::endl;

        // Prepare the prefix for the children
        std::string childPrefix = prefix + (depth == 0 ? "" : (isLeft ? "│   " : "    "));

        // Print the left subtree first
        if (node->left) {
            printTree(node->left.get(), depth + 1, childPrefix, true);
        }

        // Print the right subtree
        if (node->right) {
            printTree(node->right.get(), depth + 1, childPrefix, false);
        }
    }
}

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

    // Declare the variable "x" to be dynamically evaluated
    solver.declareVariable("z", 0);  // This can be overwritten by evaluateForRange

    // Expression to evaluate for the range of x values
    std::string expression = "x^2 + 3*x + 2";

    // Generate 400 evenly spaced numbers between -10 and 10
    std::vector<double> x_values = linspace(-10, 10, 400);

    // Get the results by evaluating the expression over the range of x values
    std::vector<double> results = solver.evaluateForRange("x", x_values, expression);

    // Print the results
    std::cout << "Results for the expression: " << expression << std::endl;
    std::cout << std::fixed << std::setprecision(5); // Set precision for printing
    for (size_t i = 0; i < x_values.size(); ++i) {
        std::cout << "x = " << x_values[i] << " -> f(x) = " << results[i] << std::endl;
    }

    return 0;

}
