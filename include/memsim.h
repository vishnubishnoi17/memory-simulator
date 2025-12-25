#ifndef MEMSIM_H
#define MEMSIM_H

#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <map>
#include <unordered_map>
#include <memory>

enum class AllocatorType { FIRST_FIT, BEST_FIT, WORST_FIT, BUDDY };

struct Block {
    uintptr_t size;     // Total size including header
    bool allocated;     // Allocated flag
    Block* prev;        // Doubly-linked for coalescing
    Block* next;
    int id;             // Block ID for CLI
    // Payload follows header
};

struct PageTableEntry {
    bool valid;         // Valid bit
    uintptr_t frame;    // Physical frame number
    int ref_count;      // For LFU (optional)
    uintptr_t timestamp; // For LRU
};

class AllocatorStrategy {  // Strategy pattern for allocators
public:
    virtual uintptr_t allocate(size_t size, Block** head, int& next_id, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) = 0;
    virtual bool deallocate(int id, Block** head, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) = 0;
    virtual ~AllocatorStrategy() = default;
};

class MemorySimulator {
private:
    Block* head;                        // Linked list head
    uintptr_t total_size;
    uintptr_t base_addr;
    AllocatorType current_allocator;
    std::unique_ptr<AllocatorStrategy> strategy;         // Current strategy
    int next_id;
    uintptr_t used_bytes;
    uintptr_t alloc_attempts;
    uintptr_t alloc_successes;
    std::map<int, Block*> id_to_block;  // Track by ID for free

    // Cache
    struct CacheLevel {
        uintptr_t size, block_size, associativity;
        std::vector<std::unordered_map<uintptr_t, std::pair<uintptr_t, uintptr_t>>> sets;  // Tag -> (data, timestamp)
        uintptr_t hits, misses;
        CacheLevel(uintptr_t s, uintptr_t b, uintptr_t a) : size(s), block_size(b), associativity(a), hits(0), misses(0) {
            sets.resize(size / (b * a));
        }
        bool access(uintptr_t addr, bool& hit, uintptr_t& latency);  // Simplified
    };
    std::vector<CacheLevel> caches;  // L1, L2

    // VM
    uintptr_t va_size, page_size, phys_size;
    std::vector<PageTableEntry> page_table;  // Single process
    uintptr_t page_faults, page_hits;
    enum class Replacement { FIFO, LRU };

public:
    MemorySimulator();
    ~MemorySimulator();
    bool init(uintptr_t size, bool enable_cache = false, bool enable_vm = false);
    uintptr_t malloc(size_t size);
    bool free(int block_id);
    void dump();
    void stats();
    void set_allocator(AllocatorType type);
    void access_virtual(uintptr_t va);  // Triggers VM + Cache + Memory
    // Cache/VM config (optional)
    void config_cache(uintptr_t l1_size, uintptr_t block_size, uintptr_t assoc);
    void config_vm(uintptr_t vas, uintptr_t ps);
    int get_last_id() const { return next_id - 1; }  // For CLI
};

#endif
