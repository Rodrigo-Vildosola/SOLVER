#include "token.h"
#include "helpers/node_factory.h"

using namespace Expression;

namespace AST {

inline int getFunctionArgCount(const std::string &funcName) {
    static const std::unordered_map<std::string, int> funcArgCounts = {
        {"sin", 1},
        {"cos", 1},
        {"tan", 1},
        {"ln", 1},
        {"neg", 1},
        {"log", 2}
        // Add more functions as needed.
    };

    auto it = funcArgCounts.find(funcName);
    if (it != funcArgCounts.end()) {
        return it->second;
    }
    // Default: assume 1 argument if unknown.
    return 1;
}

Node* buildASTFromPostfix(const std::vector<Token> &tokens, NodeFactory &factory)
{
    std::stack<Node*> st;

    for (const auto &tk : tokens) {
        switch (tk.type) {
        case TokenType::NUMBER:
        {
            // Create a Number node
            Node* numNode = factory.num(tk.numericValue);
            st.push(numNode);
            break;
        }

        case TokenType::VARIABLE:
        {
            // Create a Variable node
            Node* varNode = factory.var(tk.value);
            st.push(varNode);
            break;
        }

        case TokenType::OPERATOR:
        {
            // Typically an operator is binary (+, -, *, /, ^, ==, etc.)
            // If unary operators exist (like unary minus) you'd handle them separately.
            switch (tk.op)
            {
            case OperatorType::ADD:
            {
                if (st.size() < 2) 
                    throw SolverException("Not enough operands for + operator");
                Node* right = st.top(); st.pop();
                Node* left  = st.top(); st.pop();
                Node* result = factory.add(left, right);
                st.push(result);
                break;
            }
            case OperatorType::SUB:
            {
                if (st.size() < 2) 
                    throw SolverException("Not enough operands for - operator");
                Node* right = st.top(); st.pop();
                Node* left  = st.top(); st.pop();
                Node* result = factory.sub(left, right);
                st.push(result);
                break;
            }
            case OperatorType::MUL:
            {
                if (st.size() < 2) 
                    throw SolverException("Not enough operands for * operator");
                Node* right = st.top(); st.pop();
                Node* left  = st.top(); st.pop();
                Node* result = factory.mul(left, right);
                st.push(result);
                break;
            }
            case OperatorType::DIV:
            {
                if (st.size() < 2) 
                    throw SolverException("Not enough operands for / operator");
                Node* right = st.top(); st.pop();
                Node* left  = st.top(); st.pop();
                Node* result = factory.div(left, right);
                st.push(result);
                break;
            }
            case OperatorType::POW:
            {
                if (st.size() < 2) 
                    throw SolverException("Not enough operands for ^ operator");
                Node* right = st.top(); st.pop();
                Node* left  = st.top(); st.pop();
                Node* result = factory.exp(left, right);
                st.push(result);
                break;
            }
            case OperatorType::EQU:
            {
                if (st.size() < 2) throw SolverException("Not enough operands for ==");
                Node* right = st.top(); st.pop();
                Node* left  = st.top(); st.pop();
                st.push(factory.eq(left, right));
                break;
            }
            default:
                // Unknown or unhandled operator
                throw SolverException("Unknown operator token in postfix: " + tk.value);
            }
            break;
        }

        case TokenType::FUNCTION:
        {
            // Suppose a function is represented in postfix with arguments already placed
            // on the stack. For example, "x sin" => sin(x). Or if there's a multi-arg function
            // you might have an integer that tells how many arguments. 
            // For a single-argument function:
            int argCount = getFunctionArgCount(tk.value);
            if (st.size() < static_cast<size_t>(argCount))
                throw SolverException("Not enough operands for function: " + tk.value);
            
            // Pop arguments (in reverse order since they are pushed in order).
            std::vector<Node*> args(argCount);
            for (int i = argCount - 1; i >= 0; --i) {
                args[i] = st.top();
                st.pop();
            }

            // Create the function node. For built-in functions you may want to
            // dispatch to specific factory methods; for example:
            if (tk.value == "sin" && argCount == 1) {
                st.push(factory.sin(args[0]));
            } else if (tk.value == "cos" && argCount == 1) {
                st.push(factory.cos(args[0]));
            } else if (tk.value == "tan" && argCount == 1) {
                st.push(factory.tan(args[0]));
            } else if (tk.value == "ln" && argCount == 1) {
                st.push(factory.ln(args[0]));
            } else if (tk.value == "neg" && argCount == 1) {
                st.push(factory.neg(args[0]));
            } else if (tk.value == "log" && argCount == 2) {
                // Convention: log(base, operand)
                st.push(factory.log(args[0], args[1]));
            } else {
                // For user-defined or generic functions, use a generic factory call.
                st.push(factory.func(tk.value, argCount, args, /* you may provide a callback if needed */ nullptr));
            }
            break;
        }

        default:
            // e.g. parentheses or separators might not appear in final postfix,
            // or you've stripped them out in the shunting yard step. 
            // Or handle them if your pipeline leaves them in the postfix.
            break;
        } // end switch token.type
    } // end for

    if (st.size() != 1) {
        throw SolverException("Invalid postfix expression: stack does not have exactly one element at the end.");
    }
    return st.top();
}

} // namespace AST
