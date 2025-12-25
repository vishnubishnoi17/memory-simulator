#include "cache.h"
#include <algorithm>

CacheLevel::CacheLevel(uintptr_t s, uintptr_t b, uintptr_t a, Policy p) 
    : size(s), block_size(b), associativity(a), hits(0), misses(0), policy(p) {
    uintptr_t num_sets = size / (block_size * a);
    cache_lines.resize(num_sets);
}

bool CacheLevel::access(uintptr_t addr, bool& hit, uintptr_t& time) {
    uintptr_t set_idx = (addr / block_size) % cache_lines.size();
    uintptr_t tag = addr / (block_size * cache_lines.size());
    auto& set = cache_lines[set_idx];

    auto it = set.find(tag);
    if (it != set.end()) {
        ++hits;
        it->second.second = time;  // Update timestamp for LRU
        hit = true;
        return true;  // Hit
    }
    ++misses;
    hit = false;
    // Evict (simplified: LRU overwrites least recent; FIFO could use queue)
    if (set.size() >= associativity) {
        auto evict_it = set.begin();
        for (auto itr = set.begin(); itr != set.end(); ++itr) {
            if (itr->second.second < evict_it->second.second) evict_it = itr;
        }
        set.erase(evict_it->first);
    }
    set[tag] = {0, time};  // Load from lower, data is dummy (0)
    return false;  // Miss
}