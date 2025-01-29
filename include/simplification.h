// Simplification.h
#pragma once

#include "pch.h"
#include "expr_node.h"


namespace Simplification {
    
    struct RewriteRule {
        std::function<bool(const ExprNode&)> pattern;
        std::function<ExprNode*(ExprNode*)> replacement;
        
        bool matches(const ExprNode& node) const { return pattern(node); }
        ExprNode* apply(ExprNode* node) const { return replacement(node); }
    };

    class RewriteEngine {
    public:
        void addRule(const RewriteRule& rule) { rules.push_back(rule); }
        ExprNode* applyRules(ExprNode* node);

    private:
        std::vector<RewriteRule> rules;
    };
    
}
