#include "compiler.h"
#include <cmath>     // For std::pow
#include <vector>
#include <stack>
#include <stdexcept> // For std::runtime_error if needed


EvalFunc compilePostfix(const std::vector<Token>& tokens, const std::unordered_map<std::string, Function>& functions) {
    std::stack<EvalFunc> funcStack;
    
    for (const auto &token : tokens) {
        if (token.type == NUMBER) {
            // Create a lambda that returns the constant.
            NUMBER_TYPE val = token.numericValue;
            funcStack.push([val](const Env&) -> NUMBER_TYPE {
                return val;
            });
        }
        else if (token.type == VARIABLE) {
            // Create a lambda that looks up the variable in the environment.
            std::string varName = token.value;
            funcStack.push([varName](const Env &env) -> NUMBER_TYPE {
                auto it = env.find(varName);
                if (it == env.end()) {
                    throw SolverException("Variable '" + varName + "' not found in environment.");
                }
                return it->second;
            });
        }
        else if (token.type == OPERATOR) {
            // Binary operator: pop two functions.
            if (funcStack.size() < 2) {
                throw SolverException("Not enough operands during compilation for operator " + token.value);
            }
            auto rightFunc = funcStack.top();
            funcStack.pop();
            auto leftFunc  = funcStack.top();
            funcStack.pop();
            switch (token.op) {
                case OperatorType::ADD:
                    funcStack.push([leftFunc, rightFunc](const Env &env) -> NUMBER_TYPE {
                        return leftFunc(env) + rightFunc(env);
                    });
                    break;
                case OperatorType::SUB:
                    funcStack.push([leftFunc, rightFunc](const Env &env) -> NUMBER_TYPE {
                        return leftFunc(env) - rightFunc(env);
                    });
                    break;
                case OperatorType::MUL:
                    funcStack.push([leftFunc, rightFunc](const Env &env) -> NUMBER_TYPE {
                        return leftFunc(env) * rightFunc(env);
                    });
                    break;
                case OperatorType::DIV:
                    funcStack.push([leftFunc, rightFunc](const Env &env) -> NUMBER_TYPE {
                        NUMBER_TYPE r = rightFunc(env);
                        if (r == 0) throw SolverException("Division by zero");
                        return leftFunc(env) / r;
                    });
                    break;
                case OperatorType::POW:
                    funcStack.push([leftFunc, rightFunc](const Env &env) -> NUMBER_TYPE {
                        return std::pow(leftFunc(env), rightFunc(env));
                    });
                    break;
                default:
                    throw SolverException("Unknown operator during compilation: " + token.value);
            }
        }
        else if (token.type == FUNCTION) {
            // For a function, we need to pop as many operands as the function requires.
            auto funcIt = functions.find(token.value);
            if (funcIt == functions.end()) {
                throw SolverException("Unknown function during compilation: " + token.value);
            }
            size_t argCount = funcIt->second.argCount;
            if (funcStack.size() < argCount) {
                throw SolverException("Not enough operands for function " + token.value);
            }
            std::vector<EvalFunc> args(argCount);
            for (size_t i = 0; i < argCount; ++i) {
                args[argCount - i - 1] = funcStack.top();
                funcStack.pop();
            }
            // Capture the function callback directly.
            auto callback = funcIt->second.callback;
            funcStack.push([token, args, callback](const Env &env) -> NUMBER_TYPE {
                std::vector<NUMBER_TYPE> evaluatedArgs;
                evaluatedArgs.reserve(args.size());
                for (auto &f : args) {
                    evaluatedArgs.push_back(f(env));
                }
                return callback(evaluatedArgs);
            });
        }
        // (Optionally handle additional token types.)
    }
    if (funcStack.size() != 1) {
        throw SolverException("Compilation error: stack size is not 1 after processing.");
    }
    return funcStack.top();
}
