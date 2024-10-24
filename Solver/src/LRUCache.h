#include <unordered_map>
#include <list>
#include <memory>

template<typename Key, typename Value>
class LRUCache {
public:
    LRUCache(size_t maxSize) : maxSize(maxSize) {}

    Value* get(const Key& key) {
        auto it = cacheMap.find(key);
        if (it == cacheMap.end()) {
            return nullptr;
        }
        // Move the accessed item to the front of the list
        cacheList.splice(cacheList.begin(), cacheList, it->second);
        return &(it->second->second);
    }

    void put(const Key& key, Value value) {
        auto it = cacheMap.find(key);
        if (it != cacheMap.end()) {
            // Key already exists, update value and move to front
            cacheList.splice(cacheList.begin(), cacheList, it->second);
            it->second->second = std::move(value);
            return;
        }
        if (cacheList.size() == maxSize) {
            // Cache is full, remove the least recently used (back of the list)
            auto last = cacheList.back();
            cacheMap.erase(last.first);
            cacheList.pop_back();
        }
        // Insert new item at the front
        cacheList.emplace_front(key, std::move(value));
        cacheMap[key] = cacheList.begin();
    }

private:
    size_t maxSize;
    std::list<std::pair<Key, Value>> cacheList; // Stores keys and values
    std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> cacheMap; // Maps keys to list iterators
};
