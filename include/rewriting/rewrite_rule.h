#pragma once

#include "expression/node.h"
#include "helpers/node_factory.h"

namespace Expression {

// Abstract base class for transformation rules.
class RewriteRule {
public:
    virtual ~RewriteRule() = default;

    // Return true if this rule matches the given node.
    virtual bool matches(const Node* node) const = 0;

    // Apply the transformation and return a new node.
    // The caller is responsible for using the factory to allocate nodes.
    virtual Node* apply(const Node* node, NodeFactory &factory) const = 0;
};

} // namespace Expression
