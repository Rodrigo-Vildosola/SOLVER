#include "simplification/simplification_engine.h"

void SimplificationEngine::add_rule(std::unique_ptr<SimplificationRule> rule) {
    rules.push_back(std::move(rule));
}

std::vector<Token> SimplificationEngine::simplify(const std::vector<Token>& input, const std::unordered_map<std::string, Function>& functions) {
    std::vector<Token> current = input;
    bool changed = false;
    const int MAX_ITERATIONS = 50; // safeguard against infinite loops

    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        std::vector<Token> next = simplifyPass(current, functions, changed);
        if (!changed)
            break;
        current = std::move(next);
    }
    return current;
}

std::vector<Token> SimplificationEngine::simplifyPass(const std::vector<Token>& input, const std::unordered_map<std::string, Function>& functions, bool &changed) {
    changed = false;
    std::stack<std::vector<Token>> stack;

    for (const Token &token : input) {
        if (token.type == NUMBER || token.type == VARIABLE) {
            // These are leaves; push as single-token expressions.
            stack.push({ token });
        }
        else if (token.type == OPERATOR) {
            // For a binary operator, pop the last two sub-expressions.
            if (stack.size() < 2) {
                throw SolverException("Not enough operands for operator '" + token.value + "' during simplification.");
            }
            auto rightExpr = stack.top(); stack.pop();
            auto leftExpr  = stack.top(); stack.pop();

            // Reassemble the current sub-expression.
            std::vector<Token> subExpr = leftExpr;
            subExpr.insert(subExpr.end(), rightExpr.begin(), rightExpr.end());
            subExpr.push_back(token);

            // Try all rules (in order) on this sub-expression.
            for (const auto &rule : rules) {
                std::vector<Token> candidate;
                if (rule->apply(subExpr, candidate)) {
                    subExpr = std::move(candidate);
                    changed = true;
                    break; // if one rule applies, do not try further rules for this sub-expression.
                }
            }
            stack.push(std::move(subExpr));
        }
        else if (token.type == FUNCTION) {
            // Look up the function to know how many arguments to pop.
            auto it = functions.find(token.value);
            if (it == functions.end()) {
                throw SolverException("Unknown function '" + token.value + "' during simplification.");
            }
            const Function &func = it->second;
            size_t argCount = func.argCount;

            if (stack.size() < argCount) {
                throw SolverException("Not enough arguments for function '" + token.value + "' during simplification.");
            }

            // Pop the function's arguments (in reverse order).
            std::vector<std::vector<Token>> argExprs(argCount);
            for (size_t i = 0; i < argCount; i++) {
                argExprs[argCount - i - 1] = stack.top();
                stack.pop();
            }
            
            // Reassemble the full sub-expression representing the function call.
            std::vector<Token> subExpr;
            for (const auto &arg : argExprs) {
                subExpr.insert(subExpr.end(), arg.begin(), arg.end());
            }
            subExpr.push_back(token);

            // Try all rules on this function sub-expression.
            for (const auto &rule : rules) {
                std::vector<Token> candidate;
                if (rule->apply(subExpr, candidate)) {
                    subExpr = std::move(candidate);
                    changed = true;
                    break;
                }
            }
            stack.push(std::move(subExpr));
        }
        else {
            throw SolverException("Unexpected token type in postfix: " + token.value);
        }
    }

    if (stack.size() != 1) {
        throw SolverException("Postfix simplification error: leftover expressions in stack.");
    }

    return stack.top();
}
