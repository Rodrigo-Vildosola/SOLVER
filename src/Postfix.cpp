#include "pch.h"
#include "postfix.h"
#include "debug.h"

namespace Postfix {

#pragma region Helpers

size_t getFunctionArgCount(const std::string& functionName, const std::unordered_map<std::string, Function>& functions) {
    auto it = functions.find(functionName);
    if (it != functions.end()) {
        return it->second.argCount;
    }
    throw SolverException("Unknown function '" + functionName + "'");
}

#pragma endregion

#pragma region Parsing and Shunting Yard

int getPrecedence(const std::string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    if (op == "^") return 3;
    return 0;
}

bool isLeftAssociative(const std::string& op) {
    return op != "^";  // "^" is right-associative, all others are left-associative
}

void processOperatorStack(const Token& token, std::stack<Token>& operatorStack, std::vector<Token>& outputVector) {
    while (!operatorStack.empty() &&
           ((isLeftAssociative(token.value) && getPrecedence(token.value) <= getPrecedence(operatorStack.top().value)) ||
            (!isLeftAssociative(token.value) && getPrecedence(token.value) < getPrecedence(operatorStack.top().value)))) {
        outputVector.push_back(operatorStack.top());
        operatorStack.pop();
    }
    operatorStack.push(token);
}

void handleParentheses(std::stack<Token>& operatorStack, std::vector<Token>& outputVector) {
    while (!operatorStack.empty() && operatorStack.top().value != "(") {
        outputVector.push_back(operatorStack.top());
        operatorStack.pop();
    }
    if (operatorStack.empty()) {
        throw SolverException("Mismatched parentheses.");
    }
    operatorStack.pop();  // Pop the left parenthesis
}

void handleFunctionArgumentSeparator(std::stack<Token>& operatorStack, std::vector<Token>& outputVector, std::stack<int>& argumentCounts) {
    while (!operatorStack.empty() && operatorStack.top().value != "(") {
        outputVector.push_back(operatorStack.top());
        operatorStack.pop();
    }
    if (operatorStack.empty()) {
        throw SolverException("Mismatched parentheses or misplaced comma.");
    }
    // Increase the argument count for the current function
    if (!argumentCounts.empty()) {
        argumentCounts.top()++;
    }
}

std::vector<Token> shuntingYard(const std::vector<Token>& tokens) {
    PROFILE_FUNCTION()
    std::vector<Token> outputVector;       // Use a vector instead of queue
    std::stack<Token> operatorStack;
    std::stack<int> argumentCounts;

    for (const auto& token : tokens) {
        if (token.type == NUMBER || token.type == VARIABLE) {
            outputVector.push_back(token); // Push directly into the output vector
        } else if (token.type == FUNCTION) {
            operatorStack.push(token);
            argumentCounts.push(1);
        } else if (token.type == OPERATOR) {
            processOperatorStack(token, operatorStack, outputVector);
        } else if (token.value == "(") {
            operatorStack.push(token);
        } else if (token.value == ")") {
            handleParentheses(operatorStack, outputVector);
            if (!operatorStack.empty() && operatorStack.top().type == FUNCTION) {
                outputVector.push_back(operatorStack.top());
                operatorStack.pop();
            }
        } else if (token.value == ",") {
            handleFunctionArgumentSeparator(operatorStack, outputVector, argumentCounts);
        }
    }

    // Pop all remaining operators into the output vector
    while (!operatorStack.empty()) {
        if (operatorStack.top().value == "(" || operatorStack.top().value == ")") {
            throw SolverException("Mismatched parentheses.");
        }
        outputVector.push_back(operatorStack.top());
        operatorStack.pop();
    }

    return outputVector; // Return the postfix expression as a vector
}

#pragma endregion

#pragma region Postfix Evaluation

double evaluatePostfix(const std::vector<Token>& postfixQueue, const SymbolTable& symbolTable, const std::unordered_map<std::string, Function>& functions) {
    PROFILE_FUNCTION()
    std::stack<double> stack;

    for (const auto& token : postfixQueue) {
        if (token.type == NUMBER) {
            stack.push(std::stod(token.value));
        } else if (token.type == VARIABLE) {
            stack.push(symbolTable.lookupSymbol(token.value));
        } else if (token.type == OPERATOR) {
            if (stack.size() < 2) {
                throw SolverException("Not enough operands for operator '" + token.value + "'");
            }

            double right = stack.top(); stack.pop();
            double left = stack.top(); stack.pop();

            if (token.value == "+") stack.push(left + right);
            else if (token.value == "-") stack.push(left - right);
            else if (token.value == "*") stack.push(left * right);
            else if (token.value == "/") {
                if (right == 0) throw SolverException("Division by zero error.");
                stack.push(left / right);
            } else if (token.value == "^") stack.push(std::pow(left, right));
            else throw SolverException("Unknown operator: '" + token.value + "'");
        } else if (token.type == FUNCTION) {
            auto it = functions.find(token.value);
            if (it == functions.end()) {
                throw SolverException("Unknown function: '" + token.value + "'");
            }

            const Function& function = it->second;
            size_t argCount = function.argCount;

            if (stack.size() < argCount) {
                throw SolverException("Not enough arguments for function '" + token.value + "'");
            }

            std::vector<double> args(argCount);
            for (size_t i = 0; i < argCount; ++i) {
                args[argCount - i - 1] = stack.top();
                stack.pop();
            }

            if (function.isPredefined) {
                stack.push(function.callback(args));
            } else {
                throw SolverException("User defined functions should be flattened before eval stage");
            }
        } else {
            throw SolverException("Unsupported token type during evaluation.");
        }
    }

    if (stack.size() != 1) {
        throw SolverException("The postfix expression could not be evaluated.");
    }

    return stack.top();
}


std::vector<Token> flattenPostfix(const std::vector<Token>& postfixQueue, const std::unordered_map<std::string, Function>& functions) {
    PROFILE_FUNCTION()

    // We use a stack of "partial" postfix expressions. Each element on the stack
    // is a fully formed (sub)postfix sequence. For instance, if we see a NUMBER token,
    // we push a vector containing just that one token. If we see an OPERATOR that needs
    // two operands, we pop the top two vectors from the stack, concatenate them, then
    // append the operator, creating a single new vector we push back.

    std::stack<std::vector<Token>> argumentStack;

    for (const auto& token : postfixQueue)
    {
        switch (token.type)
        {
        case NUMBER:
        case VARIABLE:
        {
            // For a simple number or variable, just push it as a single-element vector.
            argumentStack.push({ token });
            break;
        }

        case OPERATOR:
        {
            // An operator needs two operands in postfix:
            // leftExpr + rightExpr + operator
            if (argumentStack.size() < 2)
            {
                throw SolverException(
                    "Not enough operands for operator '" + token.value + "'");
            }
            // Pop the right-hand side
            auto right = argumentStack.top();
            argumentStack.pop();
            // Pop the left-hand side
            auto left = argumentStack.top();
            argumentStack.pop();

            // Merge them into one postfix expression
            std::vector<Token> combined = left;
            combined.insert(combined.end(), right.begin(), right.end());
            combined.push_back(token);

            argumentStack.push(std::move(combined));
            break;
        }

        case FUNCTION:
        {
            // A function can be either a predefined function (like sin, cos) or
            // a user-defined function. We retrieve it from the function map:
            auto it = functions.find(token.value);
            if (it == functions.end())
            {
                throw SolverException(
                    "Unknown function: '" + token.value + "'");
            }

            const Function& function = it->second;
            const size_t argCount = function.argCount;

            // Check if we have enough arguments on the stack
            if (argumentStack.size() < argCount)
            {
                throw SolverException(
                    "Insufficient arguments for function: '" + token.value + "'");
            }

            // Collect the top `argCount` vectors from the stack
            // in reverse order, because the top is the last argument.
            std::vector<std::vector<Token>> args(argCount);
            for (size_t j = 0; j < argCount; ++j)
            {
                args[argCount - j - 1] = argumentStack.top();
                argumentStack.pop();
            }

            if (function.isPredefined)
            {
                // For predefined functions, we do NOT inline anything.
                // Instead, we simply combine the arguments into one
                // postfix sequence and then append the function token.
                std::vector<Token> combined;
                for (const auto& argPostfix : args)
                {
                    combined.insert(combined.end(),
                                    argPostfix.begin(),
                                    argPostfix.end());
                }
                combined.push_back(token);

                // Push the resulting single postfix expression for
                // this function call back onto the stack.
                argumentStack.push(std::move(combined));
            }
            else
            {
                // For user-defined functions, we inline the function definition.
                // The function.inlinedPostfix is the user-defined function's
                // body in postfix form (where variables are placeholders).
                // We substitute each occurrence of the variable with its
                // corresponding argument postfix vector.

                std::vector<Token> inlined;
                inlined.reserve(function.inlinedPostfix.size() * 2); 
                // Rough reservation to reduce reallocs

                for (const auto& fToken : function.inlinedPostfix)
                {
                    // If it's a variable, see if it matches one of the argument names.
                    // If so, replace it with that argument's flattened postfix tokens.
                    if (fToken.type == VARIABLE)
                    {
                        auto argIt = std::find(function.argumentNames.begin(),
                                               function.argumentNames.end(),
                                               fToken.value);
                        if (argIt != function.argumentNames.end())
                        {
                            // This variable is one of the function's formal parameters
                            size_t index = std::distance(function.argumentNames.begin(),
                                                         argIt);
                            // Insert the entire argument postfix
                            const auto& argPostfix = args[index];
                            inlined.insert(inlined.end(),
                                           argPostfix.begin(),
                                           argPostfix.end());
                        }
                        else
                        {
                            // It's some other variable that isn't a function param
                            inlined.push_back(fToken);
                        }
                    }
                    else
                    {
                        // Keep numbers, operators, etc. as is
                        inlined.push_back(fToken);
                    }
                }

                // Push the fully inlined postfix expression for this function call
                argumentStack.push(std::move(inlined));
            }
            break;
        }

        default:
            throw SolverException(
                "Unsupported token type during flattening: " + token.value);
        }
    }

    // At the end, there should be exactly ONE expression in the stack
    // if the postfix expression was well-formed.
    if (argumentStack.size() != 1)
    {
        throw SolverException(
            "Flattening error: leftover expressions in the stack. Stack size = "
            + std::to_string(argumentStack.size()));
    }

    // This single element is our fully flattened postfix expression.
    return argumentStack.top();
}


#pragma endregion

#pragma region Postfix simplification

std::vector<Token> fullySimplifyPostfix(const std::vector<Token> &postfix, const std::unordered_map<std::string, Function> &functions)
{
    // We'll do a loop that calls singlePassSimplify repeatedly
    // until we detect no changes or we reach an iteration limit.
    std::vector<Token> current = postfix;
    bool changed = true;
    const int MAX_ITERATIONS = 50; // arbitrary safe-guard

    for (int i = 0; i < MAX_ITERATIONS && changed; ++i) {
        std::vector<Token> next = singlePassSimplify(current, functions, changed);
        current = std::move(next);
    }

    return current;
}

std::vector<Token> singlePassSimplify(const std::vector<Token> &postfix, const std::unordered_map<std::string, Function> &functions, bool &changed) {
    changed = false;
    std::stack<std::vector<Token>> stack;

    for (const Token &token : postfix)
    {
        if (token.type == NUMBER || token.type == VARIABLE) {
            // Push as a single-token vector
            stack.push({ token });
        } else if (token.type == OPERATOR) {
            if (stack.size() < 2) {
                throw SolverException(
                    "Not enough operands for operator '" + token.value + "' during simplification.");
            }
            auto rightExpr = stack.top(); 
            stack.pop();
            auto leftExpr  = stack.top(); 
            stack.pop();

            // Attempt local simplification
            std::vector<Token> simplified = trySimplifyBinary(leftExpr, rightExpr, token, changed);
            stack.push(std::move(simplified));
        } else if (token.type == FUNCTION) {
            // Lookup function info
            auto it = functions.find(token.value);
            if (it == functions.end()) {
                throw SolverException("Unknown function '" + token.value + "' during simplification.");
            }
            const Function &func = it->second;
            size_t argCount = func.argCount;

            if (stack.size() < argCount) {
                throw SolverException(
                    "Not enough arguments for function '" + token.value + "' during simplification.");
            }

            // Gather arguments
            std::vector<std::vector<Token>> argExprs(argCount);
            for (size_t i = 0; i < argCount; i++) {
                argExprs[argCount - i - 1] = stack.top();
                stack.pop();
            }

            // Attempt function-level folding
            std::vector<Token> simplified = trySimplifyFunction(argExprs, token, functions, changed);
            stack.push(std::move(simplified));
        } else {
            // If your flattened postfix truly has no FUNCTION, PAREN, etc.,
            // any other token type is unexpected at this stage.
            throw SolverException("Unexpected token type in flattened postfix: " + token.value);
        }
    }

    // There should be exactly one sub-expression on the stack if postfix is valid
    if (stack.size() != 1)
    {
        throw SolverException("Postfix simplification error: leftover expressions in stack.");
    }

    return stack.top();
}


std::vector<Token> trySimplifyBinary(const std::vector<Token> &leftExpr, const std::vector<Token> &rightExpr, const Token &opToken, bool &changed) {
    // If both are single-number expressions, constant fold
    if (isNumber(leftExpr) && isNumber(rightExpr)) {
        changed = true;  // We are folding something
        double lhs = asNumber(leftExpr);
        double rhs = asNumber(rightExpr);

        if (opToken.value == "+") {
            return { {NUMBER, std::to_string(lhs + rhs)} };
        }
        if (opToken.value == "-") {
            return { {NUMBER, std::to_string(lhs - rhs)} };
        }
        if (opToken.value == "*") {
            return { {NUMBER, std::to_string(lhs * rhs)} };
        }
        if (opToken.value == "/") {
            if (std::fabs(rhs) < 1e-14) {
                throw SolverException("Division by zero in constant folding.");
            }
            return { {NUMBER, std::to_string(lhs / rhs)} };
        }
        if (opToken.value == "^") {
            return { {NUMBER, std::to_string(std::pow(lhs, rhs))} };
        }
        throw SolverException("Unknown operator in constant folding: " + opToken.value);
    }

    // If none of the local simplifications matched, just combine them
    // into a single postfix expression: leftExpr + rightExpr + opToken
    std::vector<Token> combined = leftExpr;
    combined.insert(combined.end(), rightExpr.begin(), rightExpr.end());
    combined.push_back(opToken);
    return combined;
}


std::vector<Token> trySimplifyFunction(const std::vector<std::vector<Token>> &argExprs, const Token &funcToken, const std::unordered_map<std::string, Function> &functions, bool &changed) {
    auto it = functions.find(funcToken.value);
    if (it == functions.end()) {
        throw SolverException("Unknown function: " + funcToken.value);
    }
    const Function &func = it->second;

    if (!func.isPredefined) {
        // Shouldn't happen if everything is flattened, but just in case
        throw SolverException("User-defined function encountered in trySimplifyFunction().");
    }

    // Check if all arguments are single-number tokens => we can constant fold
    bool allNumeric = true;
    for (auto &arg : argExprs) {
        if (!isNumber(arg)) {
            allNumeric = false;
            break;
        }
    }

    if (allNumeric) {
        changed = true;
        std::vector<double> numericArgs;
        numericArgs.reserve(argExprs.size());
        for (auto &arg : argExprs) {
            numericArgs.push_back(asNumber(arg));
        }

        // Evaluate the callback
        double foldedVal;
        try {
            foldedVal = func.callback(numericArgs);  
        } catch (const std::exception &e) {
            throw SolverException("Error constant-folding function '"
                                  + funcToken.value + "': " + e.what());
        }

        return { { NUMBER, std::to_string(foldedVal) } };
    }

    // If not all numeric => reassemble
    std::vector<Token> combined;
    for (auto &arg : argExprs) {
        combined.insert(combined.end(), arg.begin(), arg.end());
    }
    combined.push_back(funcToken);
    return combined;
}


//------------------------------------------------------------------------------
// Helper Functions
//------------------------------------------------------------------------------

static bool isNumber(const std::vector<Token> &tokens)
{
    return (tokens.size() == 1 && tokens[0].type == NUMBER);
}

static double asNumber(const std::vector<Token> &tokens)
{
    return std::stod(tokens[0].value);
}

#pragma endregion

}
