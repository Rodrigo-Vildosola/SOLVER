#include "token.h"
#include "helpers/node_factory.h"

using namespace Expression;

namespace AST {

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
                    throw std::runtime_error("Not enough operands for + operator");
                Node* right = st.top(); st.pop();
                Node* left  = st.top(); st.pop();
                Node* result = factory.add(left, right);
                st.push(result);
                break;
            }
            case OperatorType::SUB:
            {
                if (st.size() < 2) 
                    throw std::runtime_error("Not enough operands for - operator");
                Node* right = st.top(); st.pop();
                Node* left  = st.top(); st.pop();
                Node* result = factory.sub(left, right);
                st.push(result);
                break;
            }
            case OperatorType::MUL:
            {
                if (st.size() < 2) 
                    throw std::runtime_error("Not enough operands for * operator");
                Node* right = st.top(); st.pop();
                Node* left  = st.top(); st.pop();
                Node* result = factory.mul(left, right);
                st.push(result);
                break;
            }
            case OperatorType::DIV:
            {
                if (st.size() < 2) 
                    throw std::runtime_error("Not enough operands for / operator");
                Node* right = st.top(); st.pop();
                Node* left  = st.top(); st.pop();
                Node* result = factory.div(left, right);
                st.push(result);
                break;
            }
            case OperatorType::POW:
            {
                if (st.size() < 2) 
                    throw std::runtime_error("Not enough operands for ^ operator");
                Node* right = st.top(); st.pop();
                Node* left  = st.top(); st.pop();
                Node* result = factory.exp(left, right);
                st.push(result);
                break;
            }
            default:
                // Unknown or unhandled operator
                throw std::runtime_error("Unknown operator token in postfix: " + tk.value);
            }
            break;
        }

        case TokenType::FUNCTION:
        {
            // Suppose a function is represented in postfix with arguments already placed
            // on the stack. For example, "x sin" => sin(x). Or if there's a multi-arg function
            // you might have an integer that tells how many arguments. 
            // For a single-argument function:
            if (st.empty())
                throw std::runtime_error("Not enough operands for function: " + tk.value);

            Node* operand = st.top();
            st.pop();

            // In your code, you might handle "sin", "cos", "log", etc.:
            // e.g.: 
            if (tk.value == "sin") {
                st.push(factory.sin(operand));
            }
            else if (tk.value == "cos") {
                st.push(factory.cos(operand));
            }
            else if (tk.value == "tan") {
                st.push(factory.tan(operand));
            }
            else if (tk.value == "ln") {
                st.push(factory.ln(operand));
            }
            else {
                // For user-defined function or unhandled function:
                // Possibly factory.func(...) if you keep track of argument count/callback?
                throw std::runtime_error("Unhandled function: " + tk.value);
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
        throw std::runtime_error("Invalid postfix expression: stack does not have exactly one element at the end.");
    }
    return st.top();
}

} // namespace AST
