#pragma once

#include "expression/node.h"
#include "expression/number_node.h"
#include "expression/variable_node.h"
#include "expression/addition_node.h"
#include "expression/subtraction_node.h"
#include "expression/multiplication_node.h"
#include "expression/exponentiation_node.h"

namespace Expression {

// A simple polynomial is represented as a mapping from exponent to coefficient.
using Poly = std::map<int, double>;

// Helpers to add, subtract, and multiply polynomials.
inline Poly add_poly(const Poly &p1, const Poly &p2) {
    Poly result = p1;
    for (const auto &entry : p2) {
        result[entry.first] += entry.second;
    }
    return result;
}

inline Poly sub_poly(const Poly &p1, const Poly &p2) {
    Poly result = p1;
    for (const auto &entry : p2) {
        result[entry.first] -= entry.second;
    }
    return result;
}

inline Poly mul_poly(const Poly &p1, const Poly &p2) {
    Poly result;
    for (const auto &e1 : p1) {
        for (const auto &e2 : p2) {
            result[e1.first + e2.first] += e1.second * e2.second;
        }
    }
    return result;
}

// Recursively extract polynomial coefficients from a Node.
// This function assumes that the expression is expanded and simplified.
inline Poly extract_poly(const Node* node, const std::string &var) {
    Poly poly;
    if (auto num = dynamic_cast<const NumberNode*>(node)) {
        poly[0] = num->getValue();
        return poly;
    }
    if (auto varNode = dynamic_cast<const VariableNode*>(node)) {
        if (varNode->toString() == var) {
            poly[1] = 1.0;
            return poly;
        }
        poly[0] = 0;
        return poly;
    }
    if (auto add = dynamic_cast<const AdditionNode*>(node)) {
        Poly p1 = extract_poly(add->left, var);
        Poly p2 = extract_poly(add->right, var);
        return add_poly(p1, p2);
    }
    if (auto sub = dynamic_cast<const SubtractionNode*>(node)) {
        Poly p1 = extract_poly(sub->left, var);
        Poly p2 = extract_poly(sub->right, var);
        return sub_poly(p1, p2);
    }
    if (auto mul = dynamic_cast<const MultiplicationNode*>(node)) {
        Poly p1 = extract_poly(mul->left, var);
        Poly p2 = extract_poly(mul->right, var);
        return mul_poly(p1, p2);
    }
    if (auto exp = dynamic_cast<const ExponentiationNode*>(node)) {
        if (auto expNum = dynamic_cast<const NumberNode*>(exp->right)) {
            int power = static_cast<int>(std::round(expNum->getValue()));
            Poly basePoly = extract_poly(exp->left, var);
            Poly result;
            result[0] = 1; // Identity for multiplication.
            for (int i = 0; i < power; ++i) {
                result = mul_poly(result, basePoly);
            }
            return result;
        }
    }
    // Fallback: treat node as a constant.
    poly[0] = node->evaluate(Env());
    return poly;
}

// Convert polynomial (a map) to a vector (coefficients in ascending order).
inline std::vector<double> poly_to_vector(const Poly &poly) {
    int degree = poly.rbegin()->first;
    std::vector<double> coeffs(degree + 1, 0.0);
    for (const auto &entry : poly) {
        coeffs[entry.first] = entry.second;
    }
    return coeffs;
}

// Solve a polynomial using the Durand–Kerner method.
// Returns all complex roots.
inline std::vector<std::complex<double>> solve_polynomial_complex(const std::vector<double>& coeffs) {
    int n = coeffs.size() - 1;
    if (n < 1) {
        throw std::runtime_error("Polynomial degree must be at least 1.");
    }
    // Normalize: leading coefficient must be 1.
    double lead = coeffs[n];
    if (std::fabs(lead) < 1e-9) {
        throw std::runtime_error("Leading coefficient is zero.");
    }
    std::vector<double> norm_coeffs(coeffs.size());
    for (size_t i = 0; i < coeffs.size(); i++) {
        norm_coeffs[i] = coeffs[i] / lead;
    }
    // The polynomial is: norm_coeffs[0] + norm_coeffs[1]*x + ... + x^n = 0.
    std::vector<std::complex<double>> roots(n);
    const double pi = 3.14159265358979323846;
    // Initial guesses: roots of unity scaled by a factor (e.g., 0.4 + random noise).
    for (int i = 0; i < n; i++) {
        double angle = 2 * pi * i / n;
        roots[i] = std::polar(1.0, angle);
    }

    const int max_iter = 1000;
    const double tol = 1e-9;
    for (int iter = 0; iter < max_iter; iter++) {
        bool converged = true;
        for (int i = 0; i < n; i++) {
            std::complex<double> f_val(0, 0);
            // Evaluate polynomial at roots[i]
            for (int k = 0; k <= n; k++) {
                f_val += norm_coeffs[k] * std::pow(roots[i], k);
            }
            std::complex<double> prod(1, 0);
            for (int j = 0; j < n; j++) {
                if (j != i) {
                    prod *= (roots[i] - roots[j]);
                }
            }
            std::complex<double> delta = f_val / prod;
            roots[i] -= delta;
            if (std::abs(delta) > tol) {
                converged = false;
            }
        }
        if (converged) break;
    }
    return roots;
}

// Wrapper: return only real roots (filtering those with very small imaginary part).
inline std::vector<double> solve_polynomial(const std::vector<double>& coeffs) {
    auto complex_roots = solve_polynomial_complex(coeffs);
    std::vector<double> real_roots;
    for (const auto &r : complex_roots) {
        if (std::fabs(r.imag()) < 1e-6) {
            real_roots.push_back(r.real());
        }
    }
    return real_roots;
}

} // namespace Expression
