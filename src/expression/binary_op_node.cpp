#include "expression/binary_op_node.h"
#include "memory/allocation_config.h"

namespace Expression {

BinaryOpNode::BinaryOpNode(Node* left, Node* right)
    : left(left), right(right) {}

BinaryOpNode::~BinaryOpNode() {
    if (AllocationConfig::getPolicy() == AllocatorPolicy::Default) {
        std::cout << "Deleting" << std::endl;
        delete left;
        delete right;
    }
}

} // namespace Expression
