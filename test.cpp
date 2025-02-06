#include <iostream>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>

// ------------------------- Expression Node -------------------------
struct Expression {
    using EvalFunc = std::function<double(const std::unordered_map<std::string, double>&)>;
    using DiffFunc = std::function<std::shared_ptr<Expression>(const std::string&)>;

    EvalFunc eval;
    DiffFunc diff;
    std::string repr;

    Expression(EvalFunc e, DiffFunc d, std::string r) : eval(std::move(e)), diff(std::move(d)), repr(std::move(r)) {}

    double evaluate(const std::unordered_map<std::string, double>& env) const {
        return eval(env);
    }

    std::shared_ptr<Expression> differentiate(const std::string& variable) const {
        return diff(variable);
    }

    std::string toString() const {
        return repr;
    }
};

// ------------------------- Helper Functions -------------------------

// Constant Expression
std::shared_ptr<Expression> constant(double value) {
    return std::make_shared<Expression>(
        [value](const std::unordered_map<std::string, double>&) { return value; },
        [](const std::string&) { return constant(0); },
        std::to_string(value)
    );
}

// Variable Expression
std::shared_ptr<Expression> variable(const std::string& name) {
    return std::make_shared<Expression>(
        [name](const std::unordered_map<std::string, double>& env) {
            return env.at(name);
        },
        [name](const std::string& var) {
            return constant(name == var ? 1 : 0);
        },
        name
    );
}

// Addition
std::shared_ptr<Expression> add(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right) {
    return std::make_shared<Expression>(
        [left, right](const std::unordered_map<std::string, double>& env) {
            return left->evaluate(env) + right->evaluate(env);
        },
        [left, right](const std::string& var) {
            return add(left->differentiate(var), right->differentiate(var));
        },
        "(" + left->toString() + " + " + right->toString() + ")"
    );
}

// Multiplication (Product Rule)
std::shared_ptr<Expression> multiply(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right) {
    return std::make_shared<Expression>(
        [left, right](const std::unordered_map<std::string, double>& env) {
            return left->evaluate(env) * right->evaluate(env);
        },
        [left, right](const std::string& var) {
            return add(
                multiply(left->differentiate(var), right),
                multiply(left, right->differentiate(var))
            );
        },
        "(" + left->toString() + " * " + right->toString() + ")"
    );
}

// Power (Power Rule)
std::shared_ptr<Expression> power(std::shared_ptr<Expression> base, double exponent) {
    return std::make_shared<Expression>(
        [base, exponent](const std::unordered_map<std::string, double>& env) {
            return std::pow(base->evaluate(env), exponent);
        },
        [base, exponent](const std::string& var) {
            return multiply(
                multiply(constant(exponent), power(base, exponent - 1)),
                base->differentiate(var)
            );
        },
        "(" + base->toString() + "^" + std::to_string(exponent) + ")"
    );
}

// ------------------------- Main Test -------------------------
int main() {
    auto expr = add(power(variable("x"), 2), multiply(constant(3), variable("x"))); // x^2 + 3x
    auto derivative = expr->differentiate("x");

    std::unordered_map<std::string, double> env = {{"x", 2}};

    std::cout << "Expression: " << expr->toString() << std::endl;
    std::cout << "Derivative: " << derivative->toString() << std::endl;
    std::cout << "Expression evaluated at x=2: " << expr->evaluate(env) << std::endl;
    std::cout << "Derivative evaluated at x=2: " << derivative->evaluate(env) << std::endl;

    return 0;
}
