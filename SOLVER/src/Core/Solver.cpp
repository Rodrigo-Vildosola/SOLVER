#include "Solver.h"
#include "Exception.h"
#include <stdexcept>
#include <cmath>
#include <stack>
#include <iostream>
#include <unordered_set>

Solver::Solver() {
    registerBuiltInFunctions();
}

void Solver::registerPredefinedFunction(const std::string& name, const FunctionCallback& callback) {
    predefinedFunctions[name] = callback;
}


void Solver::registerBuiltInFunctions() {
    registerPredefinedFunction("sin", [](const std::vector<double>& args) -> double {
        return std::sin(args[0]);
    });
    registerPredefinedFunction("cos", [](const std::vector<double>& args) -> double {
        return std::cos(args[0]);
    });
    registerPredefinedFunction("tan", [](const std::vector<double>& args) -> double {
        return std::tan(args[0]);
    });
    registerPredefinedFunction("exp", [](const std::vector<double>& args) -> double {
        return std::exp(args[0]);
    });
    registerPredefinedFunction("log", [](const std::vector<double>& args) -> double {
        if (args.size() == 2) {
            return std::log(args[0]) / std::log(args[1]); // log base `args[1]` of `args[0]`
        }
        return std::log(args[0]); // natural log by default
    });
    registerPredefinedFunction("sqrt", [](const std::vector<double>& args) -> double {
        return std::sqrt(args[0]);
    });
    // Add more functions as needed
}


// Declare constants
void Solver::declareConstant(const std::string& name, double value) {
    constants[name] = value;
}

// Declare variables (cleared between evaluations)
void Solver::declareVariable(const std::string& name, double value) {
    variables[name] = value;
}

void Solver::declareFunction(const std::string& name, const std::vector<std::string>& args, const std::string& expression) {
    validateFunctionExpression(expression, args);
    functions[name] = {args, expression};
}

void Solver::validateFunctionExpression(const std::string& expression, const std::vector<std::string>& args) {
    auto tokens = tokenize(expression);
    for (const auto& token : tokens) {
        if (token.type == VARIABLE) {
            if (std::find(args.begin(), args.end(), token.value) == args.end() &&
                constants.find(token.value) == constants.end()) {
                throw SolverException("Variable '" + token.value + "' is not declared in the function scope or as a constant.");
            }
        }
    }
}


std::unique_ptr<ExprNode> Solver::parseExpression(std::vector<Token> tokens) {
    auto postfixQueue = shuntingYard(tokens);
    std::stack<std::unique_ptr<ExprNode>> nodeStack;

    while (!postfixQueue.empty()) {
        Token token = postfixQueue.front();
        postfixQueue.pop();

        if (token.type == NUMBER || token.type == VARIABLE) {
            nodeStack.push(std::make_unique<ExprNode>(token.value));
        } else if (token.type == OPERATOR) {
            if (nodeStack.size() < 2) {
                throw SolverException("Error: Not enough operands for operator '" + token.value + "'");
            }
            auto node = std::make_unique<ExprNode>(token.value);
            node->right = std::move(nodeStack.top());
            nodeStack.pop();
            node->left = std::move(nodeStack.top());
            nodeStack.pop();
            nodeStack.push(std::move(node));
        } else if (token.type == FUNCTION) {
            // Handle the "neg" function (unary minus)
            if (token.value == "neg") {
                if (nodeStack.empty()) {
                    throw SolverException("Error: Not enough operands for unary minus (neg).");
                }
                auto node = std::make_unique<ExprNode>("-");
                node->left = std::make_unique<ExprNode>("0"); // neg(x) is 0 - x
                node->right = std::move(nodeStack.top());
                nodeStack.pop();
                nodeStack.push(std::move(node));
            } else {
                // Handle regular functions (with possibly multiple arguments)
                std::vector<std::unique_ptr<ExprNode>> arguments;
                while (!nodeStack.empty() && nodeStack.top()) {
                    arguments.insert(arguments.begin(), std::move(nodeStack.top()));
                    nodeStack.pop();
                }

                if (arguments.empty()) {
                    throw SolverException("Error: No operands for function: '" + token.value + "'.");
                }

                auto node = std::make_unique<ExprNode>(token.value);
                node->arguments = std::move(arguments);
                nodeStack.push(std::move(node));
            }
        }
    }

    if (nodeStack.empty()) {
        throw SolverException("Error: The expression could not be parsed into an expression tree.");
    }

    return std::move(nodeStack.top());
}


std::unique_ptr<ExprNode> Solver::simplify(std::unique_ptr<ExprNode> node) {
    if (!node) return nullptr;

    // Simplify the left and right subtrees (if applicable)
    node->left = simplify(std::move(node->left));
    node->right = simplify(std::move(node->right));

    // Simplify based on the operator
    if (node->value == "+") {
        // x + 0 = x
        if (isZero(node->right)) return std::move(node->left);
        if (isZero(node->left)) return std::move(node->right);
        // x + x = 2 * x
        if (areEqual(node->left, node->right)) {
            return makeMulNode(2, std::move(node->left));
        }
    } else if (node->value == "*") {
        // x * 1 = x
        if (isOne(node->right)) return std::move(node->left);
        if (isOne(node->left)) return std::move(node->right);
        // x * 0 = 0
        if (isZero(node->right) || isZero(node->left)) {
            return makeConstNode(0);
        }
    } else if (node->value == "-") {
        // x - 0 = x
        if (isZero(node->right)) return std::move(node->left);
    } else if (node->value == "/") {
        // x / 1 = x
        if (isOne(node->right)) return std::move(node->left);
    } else if (node->value == "^") {
        // x^1 = x
        if (isOne(node->right)) return std::move(node->left);
        // x^0 = 1
        if (isZero(node->right)) return makeConstNode(1);
    }

    return node;  // Return the simplified (or original) node
}


double Solver::evaluateFunction(const std::string& func, const std::vector<double>& args) {
    if (predefinedFunctions.find(func) != predefinedFunctions.end()) {
        return predefinedFunctions[func](args); // Call the function
    }

    // Check user-defined functions (already implemented)
    if (functions.find(func) != functions.end()) {
        auto funcDef = functions[func];
        if (funcDef.args.size() != args.size()) {
            throw SolverException("Function argument count mismatch for '" + func + "'. Expected " +
                                  std::to_string(funcDef.args.size()) + " but got " + std::to_string(args.size()) + ".");
        }

        auto savedLocalSymbols = variables;
        variables.clear();
        for (size_t i = 0; i < funcDef.args.size(); ++i) {
            variables[funcDef.args[i]] = args[i];
        }

        double result = evaluate(funcDef.expression);
        variables = savedLocalSymbols;

        return result;
    }
    throw SolverException("Unknown function: '" + func + "'.");
}

// Adjusted evaluateNode to check both constants and variables
double Solver::evaluateNode(const std::unique_ptr<ExprNode>& node) {
    if (!node) {
        throw SolverException("Error: Trying to evaluate a null node.");
    }

    if (!node->left && !node->right && node->arguments.empty()) {
        if (variables.find(node->value) != variables.end()) {
            return variables.at(node->value);
        } else if (constants.find(node->value) != constants.end()) {
            return constants.at(node->value);
        }
        try {
            return std::stod(node->value);
        } catch (const std::invalid_argument&) {
            throw SolverException("Invalid argument: Unable to convert '" + node->value + "' to a number.");
        }
    }

    if (!node->arguments.empty()) {
        std::vector<double> args;
        for (const auto& argNode : node->arguments) {
            args.push_back(evaluateNode(argNode));
        }
        return evaluateFunction(node->value, args);
    }

    double leftValue = evaluateNode(node->left);
    double rightValue = evaluateNode(node->right);

    if (node->value == "+") {
        return leftValue + rightValue;
    } else if (node->value == "-") {
        return leftValue - rightValue;
    } else if (node->value == "*") {
        return leftValue * rightValue;
    } else if (node->value == "/") {
        if (rightValue == 0) {
            throw SolverException("Division by zero error.");
        }
        return leftValue / rightValue;
    } else if (node->value == "^") {
        return std::pow(leftValue, rightValue);
    } else {
        throw SolverException("Unknown operator: '" + node->value + "'.");
    }
}

double Solver::evaluate(const std::string& expression, bool debug) {
    // Tokenize the expression
    auto tokens = tokenize(expression);
    // Parse the expression into an expression tree
    auto exprTree = parseExpression(tokens);
    if (!exprTree) {
        throw SolverException("Error: Failed to parse the expression into a tree.");
    }
    // Simplify the expression tree
    exprTree = simplify(std::move(exprTree));

    if (debug) {
        std::cout << "\nExpression tree:" << std::endl;
        printTree(exprTree.get());
    }

    // Evaluate the simplified expression tree
    double result = evaluateNode(exprTree);
    return result;
}



// Evaluate a range for the expression
std::vector<double> Solver::evaluateForRange(const std::string& variable, const std::vector<double>& values, const std::string& expression, bool debug) {
    std::vector<double> results;
    for (double value : values) {
        declareVariable(variable, value); // Dynamically update the variable
        try {
            double result = evaluate(expression, debug);
            results.push_back(result);
        } catch (const SolverException& e) {
            std::cerr << "Error evaluating expression with '" << variable << "' = " << value << ": " << e.what() << std::endl;
            results.push_back(std::nan("")); // Insert NaN for errors
        }
    }
    return results;
}
