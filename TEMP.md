To implement **Phase 1** effectively and keep it extensible, we’ll follow a step-by-step approach to refactor your existing code and create a rule-based multi-pass simplification system. This will involve creating several new modules, adding specialized classes, and reworking some existing methods. Here’s a structured guide with code examples:

---

### Step 1: **Separate Simplification Modules**

1. **Create a `Simplification` Namespace and Rewrite Rules System**:
   - First, create a `Simplification` namespace with a `RewriteRule` class to manage rule-based simplifications.
   - Move all simplification functions (e.g., `applyBasicSimplifications`) to the new namespace and adapt the existing simplification logic.

2. **Define the `RewriteRule` and `RewriteEngine`**:
   - Implement the `RewriteRule` struct with a pattern-matching and transformation mechanism.
   - The `RewriteEngine` class will manage multiple rewrite rules, applying them iteratively to an expression tree node.

```cpp
// Simplification.h
#pragma once
#include "ExprNode.h"
#include <functional>
#include <vector>

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
    
    ExprNode* applyBasicSimplifications(const std::string& op, ExprNode* leftNode, ExprNode* rightNode);
}
```

3. **Rewrite the `applyBasicSimplifications` Function**:
   - Update `applyBasicSimplifications` to use `RewriteRule` for each rule. This will allow the addition of new rules later.

```cpp
// Simplification.cpp
#include "Simplification.h"

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

    ExprNode* applyBasicSimplifications(const std::string& op, ExprNode* leftNode, ExprNode* rightNode) {
        RewriteEngine engine;

        engine.addRule({ [](const ExprNode& node) { return node.value == "+" && node.right->value == "0"; },
                        [](ExprNode* node) { return new ExprNode(*node->left); } });

        engine.addRule({ [](const ExprNode& node) { return node.value == "*" && node.right->value == "1"; },
                        [](ExprNode* node) { return new ExprNode(*node->left); } });

        return engine.applyRules(new ExprNode(op, leftNode, rightNode));
    }
}
```

---

### Step 2: **Implement a Multi-Pass Simplifier**

1. **Design the `MultiPassSimplifier` Class**:
   - Organize simplification rules into separate passes, with each pass addressing specific simplification cases. Each pass can be made up of a set of rewrite rules or direct function calls.
   - Allow the multi-pass simplifier to recursively apply each pass until no further changes are detected, achieving a “fixed-point” simplification.

```cpp
// MultiPassSimplifier.h
#pragma once
#include "ExprNode.h"
#include "Simplification.h"
#include <vector>

class MultiPassSimplifier {
public:
    ExprNode* simplify(ExprNode* root);

private:
    std::vector<std::vector<Simplification::RewriteRule>> passes;

    void initializePasses();
};

// MultiPassSimplifier.cpp
#include "MultiPassSimplifier.h"

void MultiPassSimplifier::initializePasses() {
    // Define rules for Pass 1
    passes.push_back({
        // Rule: x + 0 -> x
        { [](const ExprNode& node) { return node.value == "+" && node.right->value == "0"; },
          [](ExprNode* node) { return new ExprNode(*node->left); } },

        // Rule: x * 1 -> x
        { [](const ExprNode& node) { return node.value == "*" && node.right->value == "1"; },
          [](ExprNode* node) { return new ExprNode(*node->left); } }
    });

    // Add more passes as necessary
}

ExprNode* MultiPassSimplifier::simplify(ExprNode* root) {
    initializePasses();

    for (const auto& pass : passes) {
        Simplification::RewriteEngine engine;
        for (const auto& rule : pass) {
            engine.addRule(rule);
        }
        root = engine.applyRules(root);
    }
    return root;
}
```

2. **Integrate MultiPassSimplifier into `ExpressionTree::simplify`**:
   - Modify `ExpressionTree::simplify` to use `MultiPassSimplifier`, allowing simplification to occur in iterative passes.

```cpp
// ExpressionTree.cpp
#include "MultiPassSimplifier.h"

namespace ExpressionTree {
    ExprNode* simplify(ExprNode* node, const SymbolTable& symbolTable) {
        MultiPassSimplifier simplifier;
        return simplifier.simplify(node);
    }
}
```

---

### Step 3: **Refactor `ExpressionTree` with the New System**

With this new structure, replace old simplification functions and integrate the rewrite-based simplifications:

1. **Update `applyBasicSimplifications`** to Use `RewriteEngine`:
   - Any future simplification can now be added as a new `RewriteRule` in `Simplification`. This keeps the core `ExpressionTree` code clean and focused.

2. **Modify ExpressionTree Code for Simplification**:
   - Update `applyBasicSimplifications` within `ExpressionTree::simplify` to rely on `RewriteEngine` and `MultiPassSimplifier`, making it extensible.

```cpp
// ExpressionTree.cpp
#include "Simplification.h"
#include "MultiPassSimplifier.h"

namespace ExpressionTree {

    ExprNode* simplify(ExprNode* node, const SymbolTable& symbolTable) {
        MultiPassSimplifier simplifier;
        return simplifier.simplify(node);
    }

    // Additional functions can also use `Simplification::RewriteEngine` or `MultiPassSimplifier` as needed.
}
```

---

### Summary of Files and Code Changes

- **`Simplification.h` and `Simplification.cpp`**: Handles all individual simplification rules, RewriteEngine, and simple rewrite functions.
- **`RewriteRule` and `RewriteEngine`**: Defines pattern-matching and rule-application logic, making it easier to add or modify simplification rules.
- **`MultiPassSimplifier.h` and `MultiPassSimplifier.cpp`**: Applies rules in multi-pass stages, with initialization of passes to apply progressively more complex rules.
- **Updates in `ExpressionTree::simplify`**: Use `MultiPassSimplifier` for simplification, making the `ExpressionTree` namespace cleaner and more scalable.

By structuring the code this way, future additions, such as complex algebraic factorization or trigonometric simplifications, can be added as new rewrite rules or as dedicated passes within `MultiPassSimplifier`.
