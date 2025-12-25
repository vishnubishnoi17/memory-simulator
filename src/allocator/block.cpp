#include "block.h"
#include <cmath>
#include <vector>

Block* create_initial_block(uintptr_t size, uintptr_t base) {
    Block* b = (Block*)base;
    b->size = size;
    b->allocated = false;
    b->prev = nullptr;
    b->next = nullptr;
    b->id = 0;
    return b;
}

Block* split_block(Block* block, uintptr_t payload_size) {
    uintptr_t header_size = sizeof(Block);
    if (block->size < header_size + payload_size + header_size) return nullptr;
    Block* new_b = (Block*)((uintptr_t)block + header_size + payload_size);
    new_b->size = block->size - (header_size + payload_size);
    new_b->allocated = false;
    new_b->prev = block;
    new_b->next = block->next;
    new_b->id = 0;
    block->size = header_size + payload_size;
    block->next = new_b;
    if (new_b->next) new_b->next->prev = new_b;
    return new_b;
}

void coalesce(Block* block) {
    // Coalesce prev if free
    if (block->prev && !block->prev->allocated) {
        block->prev->size += block->size;
        block->prev->next = block->next;
        if (block->next) block->next->prev = block->prev;
        block = block->prev;  // Update to merged
    }
    // Coalesce next if free
    if (block->next && !block->next->allocated) {
        block->size += block->next->size;
        block->next = block->next->next;
        if (block->next) block->next->prev = block;
    }
}

double external_fragmentation(Block* head) {
    std::vector<uintptr_t> free_sizes;
    Block* curr = head;
    uintptr_t total_free = 0, avg_alloc = 128;  // Assume avg request 128 bytes
    while (curr) {
        if (!curr->allocated) {
            uintptr_t free_payload = curr->size - sizeof(Block);
            free_sizes.push_back(free_payload);
            total_free += free_payload;
        }
        curr = curr->next;
    }
    if (free_sizes.empty()) return 0.0;
    uintptr_t small_holes = 0;
    for (auto s : free_sizes) if (s < 2 * avg_alloc) small_holes += s;
    return total_free > 0 ? (double)small_holes / total_free * 100.0 : 0.0;
}

double internal_fragmentation(uintptr_t used, uintptr_t allocated) {
    if (allocated == 0) return 0.0;
    return (double)(allocated - used) / allocated * 100.0;
}

uintptr_t get_payload_addr(Block* block) {
    return (uintptr_t)block + sizeof(Block);
}
