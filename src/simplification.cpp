#include "simplification.h"

namespace Simplification {

#pragma region Postfix simplification

std::vector<Token> fullySimplifyPostfix(const std::vector<Token> &postfix, const std::unordered_map<std::string, Function> &functions) {
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
