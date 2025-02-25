#include <iostream>
#include "ast/node.h"
#include "function.h"

// Example predefined function: square(x) = x * x
NUMBER_TYPE squareFunction(const std::vector<NUMBER_TYPE>& args) {
    if (args.size() != 1) {
        throw std::runtime_error("Function 'square' requires exactly one argument.");
    }
    return args[0] * args[0];
}

int main() {
    // Define an environment for variables
    Env env;
    env["x"] = 3;

    // Define a function table to store user-defined functions
    std::unordered_map<std::string, Function> functionTable;

    // Register a predefined function: square(x) = x * x
    functionTable["square"] = Function(squareFunction, 1);

    // Construct an AST for the expression: (x + 5) * square(2)
    Node* ast = new PowerNode(
        new AdditionNode(
            new VariableNode("x"),
            new NumberNode(5)
        ),
        new NumberNode(3)
    );

    std::cout << "AST: " << ast->toString() << std::endl;
    std::cout << "Evaluation result: " << ast->evaluate(env) << std::endl;

    Node* d_ast = ast->derivative("x");
    Node* simplified_d_ast = d_ast->simplify();
    std::cout << "Derivative: " << simplified_d_ast->toString() << std::endl;
    std::cout << "Derivative evaluated: " << simplified_d_ast->evaluate(env) << std::endl;

    // Cleanup
    delete ast;
    delete d_ast;
    delete simplified_d_ast;

    return 0;
}
