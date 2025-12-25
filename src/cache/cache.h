#ifndef CACHE_H
#define CACHE_H

#include "../../include/memsim.h"
#include <vector>
#include <unordered_map>

namespace MemorySimulator {

struct CacheLevel {
    uintptr_t size, block_size, associativity;
    std::vector<std::unordered_map<uintptr_t, std::pair<uintptr_t, uintptr_t>>> cache_lines;
    uintptr_t hits, misses;

    enum class Policy { FIFO, LRU };
    Policy policy;

    CacheLevel(uintptr_t s = 1024, uintptr_t b = 64, uintptr_t a = 1,
               Policy p = Policy::FIFO);

    // This MUST match main.cpp expectations
    unsigned long access(unsigned long addr, bool &hit, unsigned long &evicted);
};

} // namespace MemorySimulator

#endif