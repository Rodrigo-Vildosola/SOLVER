// MultiPassSimplifier.h
#pragma once

#include "pch.h"
#include "ExprNode.h"
#include "Simplification.h"


class MultiPassSimplifier {
public:
    ExprNode* simplify(ExprNode* root);

private:
    std::vector<std::vector<Simplification::RewriteRule>> passes;

    void initializePasses();
};

