#include "cache.h"
#include <algorithm>

namespace MemorySimulator {

CacheLevel::CacheLevel(uintptr_t s, uintptr_t b, uintptr_t a, Policy p)
    : size(s), block_size(b), associativity(a),
      hits(0), misses(0), policy(p) 
{
    uintptr_t num_sets = size / (block_size * a);
    cache_lines.resize(num_sets);
}

unsigned long CacheLevel::access(unsigned long addr,
                                 bool &hit,
                                 unsigned long &evicted)
{
    uintptr_t set_idx = (addr / block_size) % cache_lines.size();
    uintptr_t tag = addr / (block_size * cache_lines.size());
    auto &set = cache_lines[set_idx];

    auto it = set.find(tag);

    // Cache HIT
    if (it != set.end()) {
        ++hits;
        hit = true;
        it->second.second = addr;   // timestamp substitute
        evicted = 0;
        return it->second.first;
    }

    // Cache MISS
    ++misses;
    hit = false;

    // Eviction if set full
    if (set.size() >= associativity) {
        auto evict_it = set.begin();
        for (auto itr = set.begin(); itr != set.end(); ++itr) {
            if (itr->second.second < evict_it->second.second)
                evict_it = itr;
        }

        evicted = evict_it->first;
        set.erase(evict_it);
    } else {
        evicted = 0;
    }

    // Insert new block (dummy data value = 0)
    set[tag] = {0, addr};
    return 0;
}

} // namespace MemorySimulator