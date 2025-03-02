#pragma once

#include "helpers/node_factory.h"
#include "memory/arena_allocator.h"
#include "memory/arena.h"
#include "memory/allocation_config.h"

namespace Expression {

// Return a global NodeFactory using an ArenaAllocator.
inline NodeFactory& get_arena_factory() {
    static Arena arena;                      // Global arena instance.
    static ArenaAllocator arenaAlloc(arena); // Arena-based allocator.
    static NodeFactory factory(&arenaAlloc, AllocatorPolicy::Arena);
    return factory;
}

// This macro declares local variables for an arena, an arena allocator, and a NodeFactory.
// Use it at the beginning of a scope to get a fresh arena-based NodeFactory named 'name'.
#define DECLARE_ARENA_FACTORY(name)           \
    Arena name##_arena;                         \
    ArenaAllocator name##_arenaAlloc(name##_arena);  \
    NodeFactory name(&name##_arenaAlloc, AllocatorPolicy::Arena)

} // namespace Expression
