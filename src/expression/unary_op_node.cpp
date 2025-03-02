#include "expression/unary_op_node.h"
#include "memory/allocation_config.h"

namespace Expression {

UnaryOpNode::UnaryOpNode(Node* operand)
    : operand(operand) {}

UnaryOpNode::~UnaryOpNode() {
    if (AllocationConfig::getPolicy() == AllocatorPolicy::Default) {
        delete operand;
    }
}

} // namespace Expression
