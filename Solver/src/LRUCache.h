#include "pch.h"
#include "ExprNode.h"

// LRU Cache class to manage expression trees
class LRUCache {
public:
    LRUCache(size_t capacity) : maxCapacity(capacity) {}

    // Stores the expression tree in the cache
    void storeTree(const std::string& key, std::unique_ptr<ExprNode> tree) {
        auto it = cacheMap.find(key);
        if (it != cacheMap.end()) {
            // Move the existing key to the front (most recently used)
            cacheList.splice(cacheList.begin(), cacheList, it->second);
            return;
        }

        if (cacheList.size() == maxCapacity) {
            // Remove the least recently used item
            auto last = cacheList.back();
            cacheMap.erase(last);
            cacheList.pop_back();
            treeMap.erase(last);
        }

        // Add the new tree to the front
        cacheList.push_front(key);
        cacheMap[key] = cacheList.begin();
        treeMap[key] = std::move(tree);  // Store the tree
    }

    // Retrieves the expression tree from the cache if it exists
    std::unique_ptr<ExprNode> getTree(const std::string& key) {
        auto it = cacheMap.find(key);
        if (it == cacheMap.end()) return nullptr;

        // Move the accessed key to the front (most recently used)
        cacheList.splice(cacheList.begin(), cacheList, it->second);
        return std::move(treeMap[key]);
    }

    // Clears the entire cache
    void clear() {
        cacheList.clear();
        cacheMap.clear();
        treeMap.clear();
    }

private:
    size_t maxCapacity;  // Maximum number of entries in the cache
    std::list<std::string> cacheList;  // LRU order (front is most recent)
    std::unordered_map<std::string, std::list<std::string>::iterator> cacheMap;  // Maps expression to its position in cacheList
    std::unordered_map<std::string, std::unique_ptr<ExprNode>> treeMap;  // Stores the expression trees
};
