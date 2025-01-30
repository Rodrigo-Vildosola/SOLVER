#include "simplification.h"

namespace Simplification {

#pragma region Postfix simplification

inline std::string toStringLongDouble(long double value, int precision = 30) {
    std::ostringstream oss;
    // Choose the format and precision you want:
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}

static long double asNumber(const std::vector<Token> &tokens);
static bool isNumber(const std::vector<Token> &tokens);

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
        long double lhs = asNumber(leftExpr);
        long double rhs = asNumber(rightExpr);

        if (opToken.value == "+") {
            return { {NUMBER, toStringLongDouble(lhs + rhs)} };
        }
        if (opToken.value == "-") {
            return { {NUMBER, toStringLongDouble(lhs - rhs)} };
        }
        if (opToken.value == "*") {
            return { {NUMBER, toStringLongDouble(lhs * rhs)} };
        }
        if (opToken.value == "/") {
            if (std::fabs(rhs) < 1e-14) {
                throw SolverException("Division by zero in constant folding.");
            }
            return { {NUMBER, toStringLongDouble(lhs / rhs)} };
        }
        if (opToken.value == "^") {
            return { {NUMBER, toStringLongDouble(std::pow(lhs, rhs))} };
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
        std::vector<long double> numericArgs;
        numericArgs.reserve(argExprs.size());
        for (auto &arg : argExprs) {
            numericArgs.push_back(asNumber(arg));
        }

        // Evaluate the callback
        long double foldedVal;
        try {
            foldedVal = func.callback(numericArgs);  
        } catch (const std::exception &e) {
            throw SolverException("Error constant-folding function '"
                                  + funcToken.value + "': " + e.what());
        }

        return { { NUMBER, toStringLongDouble(foldedVal) } };
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

static long double asNumber(const std::vector<Token> &tokens)
{
    return std::stold(tokens[0].value);
}


std::vector<Token> replaceConstantSymbols(const std::vector<Token> &postfix, const SymbolTable &symbolTable) {
    std::vector<Token> replaced;
    replaced.reserve(postfix.size()); // minor optimization

    for (const auto &tk : postfix)
    {
        // If it's a would-be "VARIABLE" but is actually a constant in the table,
        // convert it to a NUMBER token
        if (tk.type == VARIABLE && symbolTable.isConstant(tk.value))
        {
            long double constVal = symbolTable.lookupSymbol(tk.value);
            Token newToken;
            newToken.type  = NUMBER;
            newToken.value = toStringLongDouble(constVal);
            replaced.push_back(std::move(newToken));
        }
        else
        {
            // Otherwise, just keep the original token
            replaced.push_back(tk);
        }
    }

    return replaced;
}

#pragma endregion


#pragma region AST simplification

// Forward declarations for local helper routines
static bool isNumberNode(const ASTNode* node);
static long double getNumberValue(const ASTNode* node);
static ASTNode* makeNumberNode(long double value);
static ASTNode* simplifyOperatorNode(ASTNode* node);
static ASTNode* simplifyFunctionNode(ASTNode* node, const std::unordered_map<std::string, Function> &functions);


ASTNode* simplifyAST(ASTNode* node, const std::unordered_map<std::string, Function> &functions)
{
    if (!node) return nullptr;

    // First, simplify each child
    for (ASTNode*& child : node->children) {
        child = simplifyAST(child, functions);
    }

    // Now, apply local simplifications based on node->token.type
    switch (node->token.type) {
    case OPERATOR:
        return simplifyOperatorNode(node);

    case FUNCTION:
        return simplifyFunctionNode(node, functions);

    default:
        // NUMBER or VARIABLE have no further local folds here
        return node;
    }
}


static ASTNode* simplifyOperatorNode(ASTNode* node)
{
    if (!node) return nullptr;

    // Expect exactly 2 children for a binary operator
    if (node->children.size() != 2) {
        // Might be an error or a unary operator if you support that
        return node;
    }

    ASTNode* left  = node->children[0];
    ASTNode* right = node->children[1];
    const std::string &op = node->token.value;

    // 1) Check if both children are NUMBER => constant fold
    if (isNumberNode(left) && isNumberNode(right))
    {
        long double lhs = getNumberValue(left);
        long double rhs = getNumberValue(right);

        long double result = 0.0;
        if (op == "+")      result = lhs + rhs;
        else if (op == "-") result = lhs - rhs;
        else if (op == "*") result = lhs * rhs;
        else if (op == "/") {
            if (std::fabs(rhs) < 1e-14) {
                // We can throw or skip
                throw SolverException("Division by zero in AST folding.");
            }
            result = lhs / rhs;
        }
        else if (op == "^") {
            result = std::pow(lhs, rhs);
        }
        else {
            return node; // unknown operator, skip
        }

        // We built a new leaf with the folded result
        ASTNode* folded = makeNumberNode(result);
        delete node; // deletes leftChild and rightChild too
        return folded;
    }

    // 2) If not both numbers, optional algebraic identities:
    //    (some examples)
    // x + 0 => x
    // x - 0 => x
    // x * 1 => x
    // x * 0 => 0
    // 0 / x => 0   (if x != 0)
    // ...
    // For instance:

    if (op == "+") {
        // check if right is a number 0 => remove it
        if (isNumberNode(right) && std::fabs(getNumberValue(right)) < 1e-14) {
            // x + 0 => x
            ASTNode* leftover = left;
            node->children.clear(); // detach children
            delete node;            // also deletes right
            return leftover;
        }
        // check if left is a number 0 => remove it
        if (isNumberNode(left) && std::fabs(getNumberValue(left)) < 1e-14) {
            // 0 + x => x
            ASTNode* leftover = right;
            node->children.clear();
            delete node; // also deletes left
            return leftover;
        }
    }
    else if (op == "*") {
        // x * 0 => 0
        if (isNumberNode(right) && std::fabs(getNumberValue(right)) < 1e-14) {
            // 0
            ASTNode* zeroNode = makeNumberNode(0.0);
            delete node; // deletes left & right
            return zeroNode;
        }
        // 0 * x => 0
        if (isNumberNode(left) && std::fabs(getNumberValue(left)) < 1e-14) {
            ASTNode* zeroNode = makeNumberNode(0.0);
            delete node;
            return zeroNode;
        }
        // x * 1 => x
        if (isNumberNode(right) && std::fabs(getNumberValue(right) - 1.0) < 1e-14) {
            ASTNode* leftover = left;
            node->children.clear();
            delete node; // also deletes right
            return leftover;
        }
        // 1 * x => x
        if (isNumberNode(left) && std::fabs(getNumberValue(left) - 1.0) < 1e-14) {
            ASTNode* leftover = right;
            node->children.clear();
            delete node;
            return leftover;
        }
    }
    // ... you could add more rules as needed

    // If none apply, return original node
    return node;
}


static ASTNode* simplifyFunctionNode(ASTNode* node, const std::unordered_map<std::string, Function> &functions)
{
    if (!node) return nullptr;

    // Look up the function
    auto it = functions.find(node->token.value);
    if (it == functions.end()) {
        // Should not happen if properly flattened
        return node;
    }
    const Function &func = it->second;
    // The node->children.size() should match func.argCount, presumably

    // Check if all children are numeric
    bool allNumbers = true;
    for (auto* child : node->children) {
        if (!isNumberNode(child)) {
            allNumbers = false;
            break;
        }
    }
    if (!allNumbers) {
        // Can't fold further
        return node;
    }

    // If all numeric, gather them
    std::vector<long double> args;
    args.reserve(node->children.size());
    for (auto* child : node->children) {
        args.push_back(getNumberValue(child));
    }

    // Evaluate the function callback
    long double resultVal = 0.0;
    try {
        resultVal = func.callback(args);
    } catch (const std::exception &e) {
        throw SolverException("Error folding function '" + node->token.value + "': " + e.what());
    }

    // Replace node with a single numeric leaf
    ASTNode* folded = makeNumberNode(resultVal);
    delete node; // cleans up old children
    return folded;
}


static bool isNumberNode(const ASTNode* node)
{
    return node && node->token.type == NUMBER;
}

static long double getNumberValue(const ASTNode* node)
{
    // Assume isNumberNode(node) == true
    return std::stold(node->token.value);
}

static ASTNode* makeNumberNode(long double value)
{
    // Create a node with type NUMBER and the string representation of 'value'
    Token t;
    t.type  = NUMBER;
    t.value = toStringLongDouble(value);

    return new ASTNode(t);
}

#pragma endregion

}
