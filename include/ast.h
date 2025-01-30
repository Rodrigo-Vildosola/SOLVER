// ast.h (example header for the AST node)

#pragma once

#include <vector>
#include "token.h"
#include "function.h"

/**
 * @struct ASTNode
 * @brief A node in the abstract syntax tree (AST) built from a flattened postfix expression.
 *
 * Each node holds:
 * - A Token (NUMBER, VARIABLE, OPERATOR, or FUNCTION).
 * - A list of children (for operators, typically 2 children; for a function, 'argCount' children).
 * 
 * Note: This uses raw pointers for children; you will need to manage memory (delete) manually.
 */
struct ASTNode
{
    Token token;                ///< The token for this node (e.g., {OPERATOR, "+"} or {VARIABLE, "x"}).
    std::vector<ASTNode*> children;  ///< Child pointers. For binary operators, typically size=2. For a function, size=argCount.

    /**
     * @brief Constructor that initializes the node with a Token.
     * @param t The Token to store in this ASTNode.
     */
    explicit ASTNode(const Token &t) : token(t) {}

    /**
     * @brief Destructor that deletes all children (recursively).
     *
     * Because we store raw pointers, we must ensure we clean them up
     * to avoid memory leaks.
     */
    ~ASTNode()
    {
        for (ASTNode* child : children) {
            delete child;
        }
    }
};

/**
 * @brief Builds an AST from a flattened postfix expression.
 *
 * @param postfix   The flattened postfix tokens (no user-defined FUNCTION tokens remain).
 * @param functions Map of function name to Function struct (for predefined functions).
 * @return A pointer to the root ASTNode of the constructed tree. Caller is responsible for deleting it.
 * @throws SolverException if there's a mismatch in the stack usage, unknown function, etc.
 */
ASTNode* buildASTFromPostfix(const std::vector<Token> &postfix, const std::unordered_map<std::string, Function> &functions);


/**
 * @brief Recursively prints the AST in a tree-like format to stdout.
 * 
 * @param node    Pointer to the current AST node.
 * @param prefix  A string used to indent child nodes (managed automatically in recursion).
 * @param isLast  Indicates whether this node is the last child of its parent (for drawing └ or ├).
 */
static void printASTRecursive(const ASTNode* node, const std::string& prefix, bool isLast);


/**
 * @brief Public-facing function to pretty-print the AST from its root.
 * 
 * @param root Pointer to the root ASTNode of the tree.
 */
void printAST(const ASTNode* root);
