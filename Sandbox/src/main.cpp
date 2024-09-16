#include <iostream>
#include <sstream>
#include <string>
#include <cctype>

double parseTerm(const std::string& term) {
    std::istringstream iss(term);
    double value;
    iss >> value;
    return value;
}

void solveEquation(const std::string& equation) {
    std::string lhs, rhs;
    size_t equalsPos = equation.find('=');
    
    // Split the equation into LHS and RHS
    if (equalsPos != std::string::npos) {
        lhs = equation.substr(0, equalsPos);
        rhs = equation.substr(equalsPos + 1);
    } else {
        std::cerr << "Invalid equation format." << std::endl;
        return;
    }

    double lhsConstant = 0, lhsCoeff = 0;
    double rhsConstant = 0;

    // Parse the left side (e.g., 4 + x)
    std::istringstream lhsStream(lhs);
    std::string token;
    char op = '+'; // default operator

    while (lhsStream >> token) {
        if (token == "+" || token == "-") {
            op = token[0];
        } else if (token.find('x') != std::string::npos) {
            // Handle coefficients of x (e.g., 2x, -x)
            size_t pos = token.find('x');
            std::string coeffStr = token.substr(0, pos);
            double coeff = coeffStr.empty() ? 1 : parseTerm(coeffStr);
            coeff = (op == '-') ? -coeff : coeff;
            lhsCoeff += coeff;
        } else {
            // Parse a constant term
            double constant = parseTerm(token);
            constant = (op == '-') ? -constant : constant;
            lhsConstant += constant;
        }
    }

    // Parse the right side (e.g., 55)
    rhsConstant = parseTerm(rhs);

    // Move lhsConstant to the right side
    rhsConstant -= lhsConstant;

    // Now the equation is of the form: lhsCoeff * x = rhsConstant
    if (lhsCoeff == 0) {
        if (rhsConstant == 0) {
            std::cout << "Infinite solutions (identity equation)." << std::endl;
        } else {
            std::cout << "No solution." << std::endl;
        }
    } else {
        double solution = rhsConstant / lhsCoeff;
        std::cout << "x = " << solution << std::endl;
    }
}

int main() {
    std::string equation = "-3 - x = 55";
    std::cout << "Equation: " << equation << std::endl;
    solveEquation(equation);
    return 0;
}
