#include <unordered_map>
#include <list>

template<typename Key, typename Value>
class LRUCache {
public:
    explicit LRUCache(size_t maxSize) : maxSize(maxSize) {}

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
            // Key exists, move it to the front and update the value
            cacheList.splice(cacheList.begin(), cacheList, it->second);
            it->second->second = std::move(value);
            return;
        }
        if (cacheList.size() == maxSize) {
            // Cache is full, remove the least recently used (LRU) element
            auto last = cacheList.back();
            cacheMap.erase(last.first);
            cacheList.pop_back();
        }
        // Insert new key-value pair at the front
        cacheList.emplace_front(key, std::move(value));
        cacheMap[key] = cacheList.begin();
    }

    void clear() {
        cacheList.clear();
        cacheMap.clear();
    }

private:
    size_t maxSize;
    std::list<std::pair<Key, Value>> cacheList; // Most recently used at the front
    std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> cacheMap;
};
