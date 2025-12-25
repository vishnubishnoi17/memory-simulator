#ifndef CACHE_H
#define CACHE_H

#include "../../include/memsim.h"

struct CacheLevel {
    uintptr_t size, block_size, associativity;
    std::vector<std::unordered_map<uintptr_t, std::pair<uintptr_t, uintptr_t>>> cache_lines;  // Set index -> tag -> (data, timestamp)
    uintptr_t hits, misses;
    enum class Policy { FIFO, LRU };
    Policy policy;

    CacheLevel(uintptr_t s = 1024, uintptr_t b = 64, uintptr_t a = 1, Policy p = Policy::FIFO);
    bool access(uintptr_t phys_addr, uintptr_t& data, uintptr_t current_time);  // Returns hit
};

#endif
