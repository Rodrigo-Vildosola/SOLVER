#pragma once
#include <vector>
#include <memory>
#include <cstdlib>  // for std::aligned_alloc
#include <cassert>  // for assert

template<typename T>
class MemoryPool {
public:
    MemoryPool(size_t poolSize = 1024) : poolSize(poolSize) {
        allocatePool();
    }

    // Allocate a new object from the pool
    T* allocate() {
        if (freeList.empty()) {
            allocatePool();  // Allocate a new pool if we run out of memory
        }
        T* obj = freeList.back();
        freeList.pop_back();
        return obj;
    }

    // Deallocate and return the object back to the pool
    void deallocate(T* obj) {
        freeList.push_back(obj);  // Add the object back to the free list
    }

    // Allocate a new object in place using constructor arguments
    template<typename... Args>
    T* construct(Args&&... args) {
        T* obj = allocate();
        new (obj) T(std::forward<Args>(args)...);  // Placement new to construct the object
        return obj;
    }

    // Destroy the object and return it to the pool
    void destroy(T* obj) {
        obj->~T();  // Explicitly call the destructor
        deallocate(obj);  // Return memory to the pool
    }

    // Optionally clear the entire pool, releasing all memory
    void clearPool() {
        freeList.clear();  // Clear the free list
        memoryBlocks.clear();  // Deallocate all blocks
    }

private:
    size_t poolSize;  // Number of objects per pool allocation
    std::vector<T*> freeList;  // List of free memory slots
    std::vector<std::unique_ptr<void, void(*)(void*)>> memoryBlocks;  // Store raw memory blocks

    // Allocate a new block of memory
    void allocatePool() {
        // Allocate raw memory aligned for type T
        void* rawMemory = std::aligned_alloc(alignof(T), poolSize * sizeof(T));
        assert(rawMemory != nullptr && "Memory allocation failed!");

        // Convert raw memory to T pointers and add them to the free list
        T* block = static_cast<T*>(rawMemory);
        for (size_t i = 0; i < poolSize; ++i) {
            freeList.push_back(&block[i]);
        }

        // Use a custom deleter to free the aligned memory
        memoryBlocks.emplace_back(rawMemory, [](void* ptr) { std::free(ptr); });
    }
};
