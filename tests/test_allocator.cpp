#include "../include/memsim.h"
#include <cassert>
#include <iostream>

int test_allocator() {
    MemorySimulator sim;
    sim.init(1024);
    uintptr_t addr1 = sim.malloc(100);
    assert(addr1 != 0);
    uintptr_t addr2 = sim.malloc(200);
    assert(addr2 != 0);
    assert(sim.free(1));  // ID 1
    assert(sim.free(2));  // ID 2
    std::cout << "Tests passed!" << std::endl;
    return 0;
}
