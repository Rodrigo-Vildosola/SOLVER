#pragma once

#include "pch.h"
#include "ExprNode.h"
#include "Simplification.h"
#include "SymbolTable.h"

class MultiPassSimplifier {
public:
    explicit MultiPassSimplifier(const SymbolTable& symbolTable) : symbolTable(symbolTable) {
        initializePasses();
    }

    ExprNode* simplify(ExprNode* root);

private:
    const SymbolTable& symbolTable;
    std::vector<std::vector<Simplification::RewriteRule>> passes;

    void initializePasses();
};
