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

    // add a kv pair
    void Set(const K& key, const V& value);

    // remove a key
    std::optional<V> Remove(const K& key);

    // get capacity
    int GetCapacity();

    // get size
    int GetSize();

    // isempty
    bool IsEmpty();

private:
    LRUCache<K, V> m_cache;
    std::mutex m_mutex;
};

// constructor
template <class K, class V>
ThreadSafeCache<K, V>::ThreadSafeCache(int capacity) : m_cache(capacity) {}

// get key
template <class K, class V>
std::optional<V> ThreadSafeCache<K, V>::Get(const K& key) { 
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cache.GetValue(key)
}

// set key
template <class K, class V>
void ThreadSafeCache<K, V>::Set(const K& key, const V& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cache.PushFront(key, value);
}

// remove key
template <class K, class V>
std::optional<V> ThreadSafeCache<K, V>::Remove(const K& key) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cache.Remove(key);
}

// get capacity
template <class K, class V>
int ThreadSafeCache<K, V>::GetCapacity() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cache.GetCapacity();
}

// get size
template <class K, class V>
int ThreadSafeCache<K, V>::GetSize() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cache.GetSize();
}

// isempty
template <class K, class V>
bool ThreadSafeCache<K, V>::IsEmpty() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cache.IsEmpty();
}

#endif