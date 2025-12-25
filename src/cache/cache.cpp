#include "cache.h"
#include <algorithm>

CacheLevel::CacheLevel(uintptr_t s, uintptr_t b, uintptr_t a, Policy p) 
    : size(s), block_size(b), associativity(a), policy(p), hits(0), misses(0) {
    uintptr_t num_sets = size / (block_size * a);
    cache_lines.resize(num_sets);
}

bool CacheLevel::access(uintptr_t phys_addr, uintptr_t& data, uintptr_t current_time) {
    uintptr_t set_idx = (phys_addr / block_size) % cache_lines.size();
    uintptr_t tag = phys_addr / (block_size * cache_lines.size());
    auto& set = cache_lines[set_idx];

    auto it = set.find(tag);
    if (it != set.end()) {
        ++hits;
        it->second.second = current_time;  // Update timestamp for LRU
        data = it->second.first;
        return true;  // Hit
    }
    ++misses;
    // Evict (simplified: LRU overwrites least recent; FIFO could use queue)
    if (set.size() >= associativity) {
        auto evict_it = set.begin();
        for (auto& entry : set) {
            if (entry.second.second < evict_it->second.second) evict_it = &entry;
        }
        set.erase(evict_it->first);
    }
    set[tag] = {0 /*dummy data*/, current_time};  // Load from lower
    data = 0;
    return false;  // Miss
}
