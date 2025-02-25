#include "pch.h"
#include "solver.h"

// Helper: trim leading and trailing whitespace
static inline std::string trim(const std::string &s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) {
        ++start;
    }
    auto end = s.end();
    do {
        --end;
    } while (std::distance(start, end) > 0 && std::isspace(*end));
    return std::string(start, end + 1);
}

NUMBER_TYPE Solver::solveEquation(const std::string& equation,
                                  const std::string& variable,
                                  NUMBER_TYPE initialGuess,
                                  NUMBER_TYPE tolerance,
                                  size_t maxIterations) {
    PROFILE_FUNCTION();

    // Find the '=' sign in the equation
    size_t eqPos = equation.find('=');
    if (eqPos == std::string::npos) {
        throw SolverException("Equation must contain an '=' sign.");
    }

    // Split into left and right parts and trim whitespace
    std::string leftExpr = trim(equation.substr(0, eqPos));
    std::string rightExpr = trim(equation.substr(eqPos + 1));

    // Construct f(x) = leftExpr - (rightExpr) so that f(x)=0 when leftExpr = rightExpr
    std::string f_expr = leftExpr + "-(" + rightExpr + ")";

    // Set current expression for evaluation
    setCurrentExpression(f_expr, false);

    // Set the initial guess in the symbol table for the given variable
    declareVariable(variable, initialGuess);
    NUMBER_TYPE currentGuess = initialGuess;

    for (size_t iter = 0; iter < maxIterations; ++iter) {
        // Evaluate the function f(x) at current guess
        NUMBER_TYPE f_val = evaluate(f_expr);
        if (std::abs(f_val) < tolerance) {
            return currentGuess;
        }
        
        // Evaluate the derivative f'(x) at current guess using the central difference method
        NUMBER_TYPE f_deriv = evaluateDerivative(f_expr, variable, currentGuess, "central", tolerance);
        if (std::abs(f_deriv) < 1e-12) {
            throw SolverException("Derivative near zero; cannot solve equation.");
        }
        
        // Newton–Raphson update: x_new = x_current - f(x_current)/f'(x_current)
        NUMBER_TYPE newGuess = currentGuess - f_val / f_deriv;
        
        // Update the variable in the symbol table and the current guess
        declareVariable(variable, newGuess);
        currentGuess = newGuess;
    }
    
    throw SolverException("Equation did not converge within the maximum number of iterations.");
}
