#pragma once

#include "node.h"

namespace Expression {

// Base class for unary operations (e.g., sin, cos, negation).
class UnaryOpNode : public Node {
public:
    explicit UnaryOpNode(Node* operand);
    virtual ~UnaryOpNode();

    Node* operand;
protected:
};

} // namespace Expression
