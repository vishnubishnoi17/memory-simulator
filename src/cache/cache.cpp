#include "cache.h"
#include <algorithm>

bool MemorySimulator::CacheLevel::access(uintptr_t addr,
                                         bool &hit,
                                         uintptr_t &latency)
{
    uintptr_t set_idx = (addr / block_size) % sets.size();
    uintptr_t tag = addr / (block_size * sets.size());

    auto &set = sets[set_idx];
    auto it = set.find(tag);

    // HIT
    if (it != set.end())
    {
        hits++;
        hit = true;

        // use addr as timestamp surrogate
        it->second.second = addr;

        latency = 1;   // arbitrary or future real value
        return true;
    }

    // MISS
    misses++;
    hit = false;

    // Eviction if full
    if (set.size() >= associativity)
    {
        auto evict_it = set.begin();
        for (auto itr = set.begin(); itr != set.end(); ++itr)
            if (itr->second.second < evict_it->second.second)
                evict_it = itr;

        set.erase(evict_it);
    }

    // Insert new cache line
    set[tag] = {0, addr};
    latency = 10;   // fake miss latency for now

    return false;
}
