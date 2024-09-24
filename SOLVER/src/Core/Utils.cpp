#include "Solver.h"
#include <iostream>
#include <sstream>

void Solver::printTree(const ExprNode* node, int depth, const std::string& prefix, bool isLeft) const {
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

// Register standard math functions
void Solver::registerBuiltInFunctions() {
    registerPredefinedFunction("sin", [](const std::vector<double>& args) -> double {
        return std::sin(args[0]);
    }, 1);

    registerPredefinedFunction("cos", [](const std::vector<double>& args) -> double {
        return std::cos(args[0]);
    }, 1);

    registerPredefinedFunction("tan", [](const std::vector<double>& args) -> double {
        return std::tan(args[0]);
    }, 1);

    registerPredefinedFunction("exp", [](const std::vector<double>& args) -> double {
        return std::exp(args[0]);
    }, 1);

    // Natural logarithm function (ln)
    registerPredefinedFunction("ln", [](const std::vector<double>& args) -> double {
        return std::log(args[0]);
    }, 1);

    // Logarithm with specified base
    registerPredefinedFunction("log", [](const std::vector<double>& args) -> double {
        return std::log(args[0]) / std::log(args[1]); // log base args[1] of args[0]
    }, 2);

    registerPredefinedFunction("sqrt", [](const std::vector<double>& args) -> double {
        return std::sqrt(args[0]);
    }, 1);

    registerPredefinedFunction("abs", [](const std::vector<double>& args) -> double {
        return std::abs(args[0]);
    }, 1);

    registerPredefinedFunction("max", [](const std::vector<double>& args) -> double {
        return std::max(args[0], args[1]);
    }, 2);

    registerPredefinedFunction("min", [](const std::vector<double>& args) -> double {
        return std::min(args[0], args[1]);
    }, 2);

    // Add more predefined functions as needed
}



// Syntactic validation: Check for basic syntax correctness and provide detailed error messages
void Solver::isValidSyntax(const std::string& expression) {
    int balance = 0;
    bool lastWasOperator = true; // To handle cases like "-3"
    size_t position = 0;

    for (size_t i = 0; i < expression.length(); ++i) {
        char c = expression[i];
        position = i; // Track current position for error reporting

        if (std::isspace(c)) {
            continue;
        }

        if (c == '(') {
            balance++;
            lastWasOperator = true;
        }
        else if (c == ')') {
            balance--;
            if (balance < 0) {
                throw SolverException("Syntax Error: Unmatched closing parenthesis ')' at position " + std::to_string(i));
            }
            lastWasOperator = false;
        }
        else if (c == ',') {
            if (lastWasOperator) {
                throw SolverException("Syntax Error: Unexpected comma ',' at position " + std::to_string(i));
            }
            lastWasOperator = true; // Comma acts similarly to an operator in terms of preceding elements
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
            if (lastWasOperator) {
                throw SolverException("Syntax Error: Unexpected operator '" + std::string(1, c) + "' at position " + std::to_string(i));
            }
            lastWasOperator = true;
        }
        else if (std::isdigit(c) || std::isalpha(c) || c == '.') {
            lastWasOperator = false;
        }
        else {
            // Unknown character
            throw SolverException("Syntax Error: Unknown character '" + std::string(1, c) + "' at position " + std::to_string(i));
        }
    }

    if (balance != 0) {
        std::stringstream ss;
        ss << "Syntax Error: Mismatched parentheses. ";
        ss << (balance > 0 ? "Missing closing parenthesis ')'." 
                            : "Extra closing parenthesis ')' found.");
        throw SolverException(ss.str());
    }

    if (lastWasOperator) {
        throw SolverException("Syntax Error: Expression cannot end with an operator.");
    }
}

// Semantic validation: Ensure all dependencies are defined
void Solver::validateFunctionDependencies(const std::string& expression, const std::vector<std::string>& args) {
    auto tokens = tokenize(expression);
    for (const auto& token : tokens) {
        if (token.type == VARIABLE) {
            // Check if the variable is a function argument or a declared constant
            if (std::find(args.begin(), args.end(), token.value) == args.end() && !symbolTable.isConstant(token.value)) {
                throw SolverException("Variable '" + token.value + "' is not declared in the function scope or as a constant.");
            }
        }
        else if (token.type == FUNCTION) {
            // Ensure that the function being called is defined
            if (functions.find(token.value) == functions.end()) {
                throw SolverException("Function '" + token.value + "' is not defined.");
            }
        }
    }
}


// Validate a function expression (syntactic validation)
void Solver::validateFunctionExpression(const std::string& expression, const std::vector<std::string>& args) {
    // Since validation is moved to evaluation time, this method can be empty or removed
}
