/* LRUCache.h 
LRU Cache = Least Recently Used Cache
- When a cache is full and a new item needs to be put into cache,
the last recently used item gets removed and the new one is put in.
*/

#ifndef LRUCACHE_H
#define LRUCACHE_H
#include <iostream>
#include <unordered_map>
#include <utility>
#include <optional>
#include <stdexcept>

template <class K, class V>
class LRUCache {
public:
    // constructor
    LRUCache(int capacity);

    // destructor
    ~LRUCache();

    // copy constructor
    LRUCache(LRUCache& other);

    // overloaded assignment operator
    LRUCache& operator=(LRUCache& other);

    // move constructor
    LRUCache(LRUCache&& other) noexcept;

    // move assignment operator
    LRUCache& operator=(LRUCache&& other) noexcept;

    // clears nodes
    void Clear();

    // push back node
    void PushFront(const K& key, const V& value);

    // remove node - returns a value if found
    std::optional<V> Remove(const K& key);

    // get value from a key if ofund
    std::optional<V> GetValue(const K& key);

    // get capacity
    int GetCapacity();

    // is empty
    bool IsEmpty();

    // get size
    int GetSize();

private:
    static constexpr int DEFAULT_CAPACITY = 1;
    struct Node {
        K m_key;
        V m_value;
        Node* m_next;
        Node* m_prev;
        Node(const K& key, const V& value) : m_key(key), m_value(value), m_next(nullptr), m_prev(nullptr) {}
    };

    // movetofront - moves nodes to the front
    void MoveToFront(Node* node);

    Node* m_head; // most recently used - MRU
    Node* m_tail; // least recently used - LRU
    int m_size; // the current size of the doubly linked list
    int m_capacity; // part of the LRUCache - the max size of the cache
    std::unordered_map<K, Node*> m_map; // for O(1) access for key-value store


};

// constructor
template <class K, class V>
LRUCache<K, V>::LRUCache(int capacity) : m_head(nullptr), m_tail(nullptr), m_size(0) {
    try {
        m_capacity = capacity;
        if(m_capacity < 1) {
            throw std::invalid_argument("Error: 0 or negative capacity size, setting to default value of 1");
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
        m_capacity = DEFAULT_CAPACITY;
    }
}

// destructor
template <class K, class V>
LRUCache<K, V>::~LRUCache() { Clear(); }

// copy constructor
template <class K, class V>
LRUCache<K, V>::LRUCache(LRUCache& other) {
    // null out before copying
    m_head = nullptr;
    m_tail = nullptr;
    m_size = 0;
    m_capacity = other.m_capacity;
    Node* curr = other.m_tail;
    while(curr != nullptr) {
        PushFront(curr->m_key, curr->m_value);
        curr = curr->m_prev;
    }
}

// copy assignment operator
template <class K, class V>
LRUCache<K, V>& LRUCache<K, V>::operator=(LRUCache& other) {
    if(this != &other) {
        Clear();
        // copy and swap
        LRUCache<K, V> temp = other;
        m_head = std::exchange(temp.m_head, nullptr);
        m_tail = std::exchange(temp.m_tail, nullptr);
        m_size = std::exchange(temp.m_size, 0);
        m_capacity = std::exchange(temp.m_capacity, 0);
        m_map = std::move(temp.m_map); // m_map = temp.m_map would work, but a move is faster since temp is being destructed anyway
    }
    return *this;
}

// move constructor
template <class K, class V>
LRUCache<K, V>::LRUCache(LRUCache&& other) noexcept {
    m_head = std::exchange(other.m_head, nullptr);
    m_tail = std::exchange(other.m_tail, nullptr);
    m_size = std::exchange(other.m_size, 0);
    m_capacity = std::exchange(other.m_capacity, 0);
    m_map = std::move(other.m_map);
}

// move assignment operator
template <class K, class V>
LRUCache<K, V>& LRUCache<K, V>::operator=(LRUCache&& other) noexcept {
    if(this != &other) {
        Clear();

        m_head = std::exchange(other.m_head, nullptr);
        m_tail = std::exchange(other.m_tail, nullptr);
        m_size = std::exchange(other.m_size, 0);
        m_capacity = std::exchange(other.m_capacity, 0);
        m_map = std::move(other.m_map);
    }
    return *this;
}

// clear
// NOTE: clear does not change m_capacity
template <class K, class V>
void LRUCache<K, V>::Clear() {
    while(m_head != nullptr) {
        Remove(m_head->m_key);
    }
    m_head = nullptr;
    m_tail = nullptr;
    m_size = 0;
    m_map.clear();
}

template <class K, class V>
void LRUCache<K, V>::PushFront(const K& key, const V& value) {
    auto it = m_map.find(key);
    if(it != m_map.end()) {
        it->second->m_value = value;
        MoveToFront(it->second); // key touched - move to front
        return;
    }

    // capacity has been reached, remove LRU
    if(m_size >= m_capacity) {
        Remove(m_tail->m_key);
    }

    Node* newNode = new Node(key, value);
    newNode->m_next = m_head;
    newNode->m_prev = nullptr;
    if(IsEmpty()) {
        m_head = newNode;
        m_tail = newNode;
    } else {
        m_head->m_prev = newNode;
        m_head = newNode;
    }
    m_map.insert({key, newNode});
    m_size++;
}

template <class K, class V>
std::optional<V> LRUCache<K, V>::Remove(const K& key) {
    auto it = m_map.find(key);
    if(it == m_map.end()) { // empty case
        return std::nullopt;
    }

    Node* node = it->second;
    V rmVal = node->m_value;
    
    if(node == m_head) {
        m_head = m_head->m_next;
    } else {
        node->m_prev->m_next = node->m_next;
    }

    if(node == m_tail) {
        m_tail = m_tail->m_prev;
    } else {
        node->m_next->m_prev = node->m_prev;
    }

    m_map.erase(it);
    delete node;
    m_size--;
    return rmVal;
}

template <class K, class V>
std::optional<V> LRUCache<K, V>::GetValue(const K& key) {
    auto it = m_map.find(key);
    if(it == m_map.end()) {
        return std::nullopt;
    }
    MoveToFront(it->second);
    return it->second->m_value; 
}

template <class K, class V>
int LRUCache<K, V>::GetCapacity() { return m_capacity; }


template <class K, class V>
bool LRUCache<K, V>::IsEmpty() {
    return m_head == nullptr;
}

template <class K, class V>
int LRUCache<K, V>::GetSize() {
    return m_size;
}

// PRIVATE HELPERS
template <class K, class V>
void LRUCache<K, V>::MoveToFront(Node* node) {
    if(node == m_head) return;

    node->m_prev->m_next = node->m_next;
    if(node == m_tail) {
        m_tail = m_tail->m_prev;
    } else {
        node->m_next->m_prev = node->m_prev;
    }
    node->m_next = m_head;
    node->m_prev = nullptr;
    m_head->m_prev = node;
    m_head = node;
}


#endif