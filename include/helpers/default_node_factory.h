// File: helpers/default_node_factory.h
#pragma once

#include "helpers/node_factory.h"
#include "memory/default_allocator.h"
#include "memory/allocation_config.h"

namespace Expression {

inline NodeFactory& get_default_factory() {
    static DefaultAllocator defaultAlloc;
    static NodeFactory defaultFactory(&defaultAlloc, AllocatorPolicy::Default);
    return defaultFactory;
}

#define DECLARE_DEFAULT_FACTORY(name)                \
    DefaultAllocator name##_defaultAlloc;            \
    NodeFactory name(&name##_defaultAlloc, AllocatorPolicy::Default)

} // namespace Expression
