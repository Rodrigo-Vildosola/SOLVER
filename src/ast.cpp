#include "pch.h"
#include "ast.h"
#include "exception.h" // SolverException, if needed

namespace AST {

ASTNode* buildASTFromPostfix(const std::vector<Token> &postfix, const std::unordered_map<std::string, Function> &functions)
{
    std::stack<ASTNode*> nodeStack;

    for (const auto &token : postfix)
    {
        switch (token.type)
        {
        case NUMBER:
        case VARIABLE:
        {
            // Create a leaf node
            ASTNode* node = new ASTNode(token);
            nodeStack.push(node);
            break;
        }

        case OPERATOR:
        {
            // For a binary operator, we pop 2 children (right, then left) from the stack
            if (nodeStack.size() < 2) {
                throw SolverException("Not enough operands for operator '" + token.value + "' when building AST.");
            }

            ASTNode* rightChild = nodeStack.top(); 
            nodeStack.pop();
            ASTNode* leftChild  = nodeStack.top(); 
            nodeStack.pop();

            ASTNode* opNode = new ASTNode(token);
            opNode->children.push_back(leftChild);
            opNode->children.push_back(rightChild);

            nodeStack.push(opNode);
            break;
        }

        case FUNCTION:
        {
            // Predefined function: find how many arguments
            auto it = functions.find(token.value);
            if (it == functions.end()) {
                throw SolverException("Unknown function '" + token.value + "' in AST construction.");
            }
            const Function &func = it->second;
            const size_t argCount = func.argCount;

            if (nodeStack.size() < argCount) {
                throw SolverException("Not enough arguments for function '" + token.value + "' in AST construction.");
            }

            // Pop 'argCount' children (right to left in stack order)
            std::vector<ASTNode*> argNodes(argCount);
            for (size_t i = 0; i < argCount; ++i) {
                argNodes[argCount - 1 - i] = nodeStack.top();
                nodeStack.pop();
            }

            // Create a function node, attach the children
            ASTNode* funcNode = new ASTNode(token);
            for (ASTNode* child : argNodes) {
                funcNode->children.push_back(child);
            }

            nodeStack.push(funcNode);
            break;
        }

        default:
            // Normally, the flattened postfix should not contain parentheses or user-defined FUNCTION tokens.
            throw SolverException("Unsupported token type in flattened postfix for AST building: " + token.value);
        }
    }

    // At the end, we should have exactly 1 node in the stack
    if (nodeStack.size() != 1) {
        throw SolverException("Error building AST: leftover nodes in the stack.");
    }

    // This is the root of our AST
    ASTNode* root = nodeStack.top();
    nodeStack.pop();

    return root;
}



static void printASTRecursive(const ASTNode* node, const std::string& prefix, bool isLast)
{
    if (!node) return;

    // Print the prefix and the branch symbol
    std::cout << prefix << (isLast ? "\\-- " : "|-- ");

    // Describe the node based on its token type
    switch (node->token.type) {
        case NUMBER:
            std::cout << "NUMBER(" << node->token.value << ")";
            break;
        case VARIABLE:
            std::cout << "VARIABLE(" << node->token.value << ")";
            break;
        case OPERATOR:
            std::cout << "OPERATOR(" << node->token.value << ")";
            break;
        case FUNCTION:
            std::cout << "FUNCTION(" << node->token.value << ")";
            break;
        default:
            std::cout << "UNKNOWN_TOKEN(" << node->token.value << ")";
            break;
    }
    std::cout << std::endl;

    // Prepare the prefix for child nodes.
    // If this node is the last child, we add "    " (4 spaces);
    // otherwise we add "|   " to continue the tree lines.
    std::string childPrefix = prefix + (isLast ? "    " : "|   ");

    // Recurse for each child
    for (size_t i = 0; i < node->children.size(); ++i) {
        bool childIsLast = (i == node->children.size() - 1);
        printASTRecursive(node->children[i], childPrefix, childIsLast);
    }
}

void printAST(const ASTNode* root)
{
    printASTRecursive(root, "", true);
}

}
