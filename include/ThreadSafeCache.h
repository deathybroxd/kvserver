/* ThreadSafeCache.h
templated mutex wrapper around LRUCache.h
- ThreadSafeCache owns the LRUCache in this project
*/


#ifndef THREADSAFECACHE_H
#define THREADSAFECACHE_H

#include "LRUCache.h"
#include <mutex>
#include <optional>

template <class K, class V>
class ThreadSafeCache {
public:

    // constructor
    ThreadSafeCache(int capacity);

    // destructor
    ~ThreadSafeCache() = default;

    // NO copy / move operators - would break the mutex
    ThreadSafeCache(ThreadSafeCache&) = delete;
    ThreadSafeCache& operator=(ThreadSafeCache&) = delete;
    ThreadSafeCache(ThreadSafeCache&&) = delete;
    ThreadSafeCache& operator=(ThreadSafeCache&&) = delete;

    // get key
    std::optional<V> Get(const K& key);

    void Set(const K& key, const V& value);
    
    std::optional<V> Remove(const K& key);

    std::optional


private:
    LRUCache<K, V> m_cache;
    std::mutex m_mutex;

};

#endif