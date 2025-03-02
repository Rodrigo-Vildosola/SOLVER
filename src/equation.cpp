#include "pch.h"
#include "solver.h"
#include "helpers/arena_node_factory.h"
#include "helpers/polynomial.h"

using namespace Expression;

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

std::vector<NUMBER_TYPE> Solver::solveEquation(const std::string& equation, const std::string& variable) {
    PROFILE_FUNCTION();
    DECLARE_ARENA_FACTORY(f);

    Node* ast = parseAST(equation, f);

    const EqualityNode* eq = dynamic_cast<const EqualityNode*>(ast);
    if (!eq) {
        throw SolverException("Expression is not an equation.");
    }

    Node* diff = f.sub(eq->left, eq->right);

    auto poly = extract_poly(diff, variable);
    auto coeffs = poly_to_vector(poly);

    std::ostringstream oss;
    for (size_t i = 0; i < coeffs.size(); i++) {
        oss << "x^" << i << ": " << coeffs[i] << " ";
    }

    std::vector<double> roots = solve_polynomial(coeffs);

    return roots;
}
