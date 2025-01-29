// Simplification.cpp
#include "simplification.h"

namespace Simplification {

    ExprNode* RewriteEngine::applyRules(ExprNode* node) {
        bool modified;
        do {
            modified = false;
            for (const auto& rule : rules) {
                if (rule.matches(*node)) {
                    node = rule.apply(node);
                    modified = true;
                }
            }
        } while (modified);
        return node;
    }

}
