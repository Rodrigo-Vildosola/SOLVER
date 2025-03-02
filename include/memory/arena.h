#pragma once

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstddef>
#include <new>
#include <iostream>
#include <functional>
#include <cassert>
#include "expression/node.h"

namespace Expression {

class Arena {
private:
    // Internal structure for a memory block.
    struct Block {
        char* memory;
        size_t size;
        size_t offset;
        size_t alignment;  // Alignment with which the block was allocated

        Block(size_t sz, size_t align) : size(sz), offset(0), alignment(align) {
            // Allocate the block with the requested alignment.
            memory = static_cast<char*>(::operator new(sz, std::align_val_t(align)));
        }
        ~Block() {
            ::operator delete(memory, std::align_val_t(alignment));
        }
    };

    std::vector<Block*> blocks;        ///< Vector of allocated blocks
    size_t defaultBlockSize;             ///< Default size for new blocks
    size_t blockAlignment;               ///< Base alignment for blocks
    size_t nodeCount = 0;                ///< Count of allocated nodes

    // Vector of cleanup callbacks to invoke destructors on allocated nodes.
    std::vector<std::function<void()>> destructors;

public:
    /**
     * @brief Construct a new Arena object.
     * @param blockSize The default size for new blocks.
     * @param alignment The base alignment for blocks (default is alignof(std::max_align_t)).
     */
    explicit Arena(size_t blockSize = 4096, size_t alignment = alignof(std::max_align_t))
        : defaultBlockSize(blockSize), blockAlignment(alignment) {
        blocks.push_back(new Block(defaultBlockSize, blockAlignment));
    }

    // Disable copying.
    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;

    ~Arena() {
        // Call destructors in reverse order of allocation.
        for (auto it = destructors.rbegin(); it != destructors.rend(); ++it) {
            (*it)();
        }
        std::cerr << "[Arena] Destroying: " << nodeCount << " nodes allocated.\n";
        for (Block* block : blocks) {
            delete block;
        }
    }

    /**
     * @brief Allocate a new Node of type T using placement new.
     * 
     * This version adjusts for proper alignment, registers a destructor callback
     * if T has a non-trivial destructor, and allocates new blocks as needed.
     * 
     * @tparam T Must be derived from Node.
     * @tparam Args Constructor arguments for T.
     * @return T* Pointer to the new Node.
     */
    template<typename T, typename... Args>
    T* make(Args&&... args) {
        static_assert(std::is_base_of<Node, T>::value, "T must inherit from Node");

        size_t spaceNeeded = sizeof(T);

        Block* currentBlock = blocks.back();

        // Calculate current allocation position.
        char* currentStart = currentBlock->memory + currentBlock->offset;
        size_t remaining = currentBlock->size - currentBlock->offset;
        void* alignedPtr = currentStart;

        // Attempt to align the pointer for T.
        if (!std::align(alignof(T), spaceNeeded, alignedPtr, remaining)) {
            // Not enough space in the current block after alignment.
            size_t newAlign = std::max(blockAlignment, static_cast<size_t>(alignof(T)));
            size_t newBlockSize = std::max(defaultBlockSize, spaceNeeded);
            currentBlock = new Block(newBlockSize, newAlign);
            blocks.push_back(currentBlock);
            currentStart = currentBlock->memory;
            remaining = currentBlock->size;
            alignedPtr = currentStart;
            bool ok = std::align(alignof(T), spaceNeeded, alignedPtr, remaining);
            assert(ok && "Alignment failed in new block");
        } else {
            // Check if after alignment there's enough space.
            size_t adjustment = reinterpret_cast<char*>(alignedPtr) - currentStart;
            if (adjustment + spaceNeeded > remaining) {
                size_t newAlign = std::max(blockAlignment, static_cast<size_t>(alignof(T)));
                size_t newBlockSize = std::max(defaultBlockSize, spaceNeeded);
                currentBlock = new Block(newBlockSize, newAlign);
                blocks.push_back(currentBlock);
                currentStart = currentBlock->memory;
                remaining = currentBlock->size;
                alignedPtr = currentStart;
                bool ok = std::align(alignof(T), spaceNeeded, alignedPtr, remaining);
                assert(ok && "Alignment failed in new block");
            } else {
                // Advance the offset by the adjustment needed for alignment.
                size_t adjustment = reinterpret_cast<char*>(alignedPtr) - currentStart;
                currentBlock->offset += adjustment;
            }
        }

        // Ensure there is enough space in the block.
        if (currentBlock->offset + spaceNeeded > currentBlock->size) {
            size_t newAlign = std::max(blockAlignment, static_cast<size_t>(alignof(T)));
            size_t newBlockSize = std::max(defaultBlockSize, spaceNeeded);
            currentBlock = new Block(newBlockSize, newAlign);
            blocks.push_back(currentBlock);
            currentBlock->offset = 0;
            currentStart = currentBlock->memory;
            remaining = currentBlock->size;
            alignedPtr = currentStart;
            bool ok = std::align(alignof(T), spaceNeeded, alignedPtr, remaining);
            assert(ok && "Alignment failed in new block");
        }

        void* mem = currentBlock->memory + currentBlock->offset;
        currentBlock->offset += spaceNeeded;

        T* node = new (mem) T(std::forward<Args>(args)...);
        ++nodeCount;

        if (!std::is_trivially_destructible<T>::value) {
            destructors.push_back([node]() {
                node->~T();
            });
        }

        return node;
    }

    // Optional debug method for printing usage stats.
    void printStats() const {
        size_t totalBytes = 0;
        for (const Block* block : blocks)
            totalBytes += block->size;
        std::cout << "[Arena] Blocks: " << blocks.size() 
                  << ", Total Memory: ~" << totalBytes 
                  << " bytes, Nodes: " << nodeCount << std::endl;
    }

    size_t getBlockCount() const { return blocks.size(); }

};

}  // namespace Expression
