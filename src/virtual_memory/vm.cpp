#include "vm.h"
#include <cmath>
#include <algorithm>

VirtualMemory::VirtualMemory(uintptr_t vas, uintptr_t ps, uintptr_t phys) 
    : va_size(vas), page_size(ps), phys_frames(phys / ps), faults(0), hits(0), clock(0), policy(Replacement::FIFO) {
    num_pages = 1ULL << (va_size - static_cast<uintptr_t>(std::log2(ps)));
    page_table.resize(num_pages, {false, 0, 0, 0});
}

uintptr_t VirtualMemory::translate(uintptr_t va, bool& fault) {
    uintptr_t vpn = va / page_size;
    if (vpn >= num_pages) { fault = true; return 0; }
    PageTableEntry& pte = page_table[vpn];
    if (!pte.valid) {
        fault = true;
        return 0;
    }
    ++hits;
    fault = false;
    return pte.frame * page_size + (va % page_size);
}

void VirtualMemory::handle_fault(uintptr_t va) {
    ++faults;
    uintptr_t vpn = va / page_size;
    // Find victim
    if (policy == Replacement::FIFO) {
        uintptr_t victim = clock % phys_frames;
        page_table[victim].valid = false;  // Evict (simplified indexing)
    } else {  // LRU
        uintptr_t min_time = UINT64_MAX, victim_vpn = 0;
        for (uintptr_t i = 0; i < num_pages; ++i) {
            if (page_table[i].valid && page_table[i].timestamp < min_time) {
                min_time = page_table[i].timestamp;
                victim_vpn = i;
            }
        }
        page_table[victim_vpn].valid = false;
    }
    // Load page
    page_table[vpn].valid = true;
    page_table[vpn].frame = clock++ % phys_frames;  // Assign frame
    page_table[vpn].timestamp = clock;
    // Symbolic disk latency: 100 cycles (track in stats)
}
