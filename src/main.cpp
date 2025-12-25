#include "../include/memsim.h"
#include "../src/allocator/allocator_strategy.h"
#include "../src/allocator/block.h"
#include "../src/buddy/buddy_allocator.h"
#include <sstream>
#include <memory>
#include <iomanip>
#include "virtual_memory/vm.h" 

std::unique_ptr<AllocatorStrategy> create_strategy(AllocatorType type, uintptr_t size = 0) {
    switch (type) {
        case AllocatorType::FIRST_FIT: return std::make_unique<FirstFitStrategy>();
        case AllocatorType::BEST_FIT: return std::make_unique<BestFitStrategy>();
        case AllocatorType::WORST_FIT: return std::make_unique<WorstFitStrategy>();
        case AllocatorType::BUDDY: return std::make_unique<BuddyStrategy>(size);
        default: return nullptr;
    }
}

// MemorySimulator implementations (inline for simplicity)
MemorySimulator::MemorySimulator() : head(nullptr), total_size(0), base_addr(0), strategy(nullptr), next_id(1), used_bytes(0), alloc_attempts(0), alloc_successes(0) {}

MemorySimulator::~MemorySimulator() {
    Block* curr = head;
    while (curr) {
        Block* next = curr->next;
        operator delete(curr);
        curr = next;
    }
}

bool MemorySimulator::init(uintptr_t size, bool enable_cache, bool enable_vm) {
    if (total_size > 0) return false;
    total_size = size;
    base_addr = 0x0000;
    head = create_initial_block(size, base_addr);
    strategy = create_strategy(AllocatorType::FIRST_FIT);
    current_allocator = AllocatorType::FIRST_FIT;
    if (enable_cache) {
        caches.emplace_back(2048, 64, 1);  // L1 FIFO default
        caches.emplace_back(8192, 64, 2);  // L2 LRU
    }
    if (enable_vm) {
        config_vm(32, 4096);  // 32-bit VA, 4KB pages
    }
    return true;
}

uintptr_t MemorySimulator::malloc(size_t size) {
    ++alloc_attempts;
    uintptr_t addr = strategy->allocate(size, &head, next_id, used_bytes, id_to_block);
    if (addr) ++alloc_successes;
    return addr;
}

bool MemorySimulator::free(int block_id) {
    return strategy->deallocate(block_id, &head, used_bytes, id_to_block);
}

void MemorySimulator::dump() {
    std::cout << std::hex << std::setfill('0') << std::setw(4);
    Block* curr = head;
    while (curr) {
        uintptr_t start = (uintptr_t)curr;
        uintptr_t end = start + curr->size - 1;
        if (curr->allocated) {
            std::cout << "[" << start << " - " << end << "] USED (id=" << std::dec << curr->id << ")" << std::endl;
        } else {
            std::cout << "[" << start << " - " << end << "] FREE" << std::endl;
        }
        curr = curr->next;
    }
    std::cout << std::dec;
}

void MemorySimulator::stats() {
    uintptr_t allocated = 0;
    Block* curr = head;
    while (curr) {
        if (curr->allocated) allocated += curr->size - sizeof(Block);
        curr = curr->next;
    }
    double ext_frag = external_fragmentation(head);
    double int_frag = internal_fragmentation(used_bytes, allocated);
    double util = (double)used_bytes / total_size * 100.0;
    double success_rate = alloc_attempts > 0 ? (double)alloc_successes / alloc_attempts * 100.0 : 0.0;
    std::cout << "Total memory: " << total_size << std::endl;
    std::cout << "Used memory: " << used_bytes << std::endl;
    std::cout << "Internal fragmentation: " << std::fixed << std::setprecision(1) << int_frag << "%" << std::endl;
    std::cout << "External fragmentation: " << ext_frag << "%" << std::endl;
    std::cout << "Allocation success rate: " << success_rate << "%" << std::endl;
    std::cout << "Memory utilization: " << util << "%" << std::endl;
    // Cache stats
    for (size_t i = 0; i < caches.size(); ++i) {
        auto& c = caches[i];
        double hit_ratio = (c.hits + c.misses > 0) ? (double)c.hits / (c.hits + c.misses) * 100.0 : 0.0;
        std::cout << "L" << (i+1) << " hit ratio: " << hit_ratio << "%" << std::endl;
    }
    // VM stats (assume integrated)
    std::cout << "Page faults: " << page_faults << ", Page hits: " << page_hits << std::endl;
}

void MemorySimulator::set_allocator(AllocatorType type) {
    strategy = create_strategy(type, total_size);
    current_allocator = type;
}

void MemorySimulator::access_virtual(uintptr_t va) {
    bool fault = false;
    uintptr_t pa = 0;  // translate(va, fault);  // VM
    if (fault) {
        // handle_fault(va);
    } else {
        uintptr_t time = 0;  // clock++
        
        for (auto& c : caches) {
            
bool access_hit; 
bool hit = c.access(pa, access_hit, time); 
        }
        // Access physical at pa
    }
}

// Stub configs
void MemorySimulator::config_cache(uintptr_t l1_size, uintptr_t block_size, uintptr_t assoc) {
    caches.clear();
    caches.emplace_back(l1_size, block_size, assoc);
}

void MemorySimulator::config_vm(uintptr_t vas, uintptr_t ps) {
    va_size = vas;
    page_size = ps;
    phys_size = total_size;
    // Init page_table
    VirtualMemory vm(vas, ps, phys_size);  // Integrate as member if needed
}

int main() {
    MemorySimulator sim;
    std::string line;
    std::cout << "$ ./memsim" << std::endl << std::endl;

    while (std::getline(std::cin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        if (cmd == "init") {
            uintptr_t size;
            iss >> size;
            bool enable_cache = false, enable_vm = false;
            if (sim.init(size, enable_cache, enable_vm)) {
                std::cout << "Initialized memory of " << size << " bytes." << std::endl;
            } else {
                std::cout << "Init failed." << std::endl;
            }
        } else if (cmd == "set") {
            std::string atype;
            iss >> atype;
            AllocatorType type;
            if (atype == "first_fit") type = AllocatorType::FIRST_FIT;
            else if (atype == "best_fit") type = AllocatorType::BEST_FIT;
            else if (atype == "worst_fit") type = AllocatorType::WORST_FIT;
            else if (atype == "buddy") type = AllocatorType::BUDDY;
            else continue;
            sim.set_allocator(type);
            std::cout << "Set allocator to " << atype << "." << std::endl;
        } else if (cmd == "malloc") {
            size_t size;
            iss >> size;
            uintptr_t addr = sim.malloc(size);
            if (addr) {
                std::cout << "Allocated block id=" << sim.get_last_id() << " at address=0x" 
                          << std::hex << std::setw(4) << std::setfill('0') << addr << std::dec << std::endl;
            } else {
                std::cout << "Allocation failed." << std::endl;
            }
        } else if (cmd == "free") {
            int id;
            iss >> id;
            if (sim.free(id)) {
                std::cout << "Block " << id << " freed and merged" << std::endl;
            } else {
                std::cout << "Free failed." << std::endl;
            }
        } else if (cmd == "dump") {
            sim.dump();
        } else if (cmd == "stats") {
            sim.stats();
        } else if (cmd == "access") {  // For VM
            uintptr_t va;
            iss >> std::hex >> va;
            sim.access_virtual(va);
            std::cout << "Accessed VA 0x" << std::hex << va << std::dec << std::endl;
        } else if (cmd == "exit") {
            break;
        }
    }
    return 0;
}
