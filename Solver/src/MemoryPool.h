// MemoryPool.h
#pragma once
#include <vector>
#include <memory>

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
        obj->~T();  // Explicitly call destructor
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
    std::vector<std::unique_ptr<T[]>> memoryBlocks;  // Store memory blocks to prevent memory leaks

    // Allocate a new block of memory
    void allocatePool() {
        std::unique_ptr<T[]> newBlock = std::make_unique<T[]>(poolSize);
        for (size_t i = 0; i < poolSize; ++i) {
            freeList.push_back(&newBlock[i]);
        }
        memoryBlocks.push_back(std::move(newBlock));
    }
};
