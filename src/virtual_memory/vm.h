#ifndef VM_H
#define VM_H

#include "../../include/memsim.h"

class VirtualMemory {
public:
    uintptr_t va_size, page_size, num_pages, phys_frames;
    std::vector<PageTableEntry> page_table;
    enum class Replacement { FIFO, LRU };
    Replacement policy;
    uintptr_t faults, hits;
    uintptr_t clock;  // For FIFO queue

    VirtualMemory(uintptr_t vas, uintptr_t ps, uintptr_t phys);
    uintptr_t translate(uintptr_t va, bool& fault);
    void handle_fault(uintptr_t va);
};

#endif
