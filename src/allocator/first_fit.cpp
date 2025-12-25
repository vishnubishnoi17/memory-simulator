#include "allocator_strategy.h"
#include "block.h"
#include <cstdint>

uintptr_t FirstFitStrategy::allocate(size_t size, Block** head, int& next_id, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) {
    uintptr_t req_size = sizeof(Block) + size;
    Block* curr = *head;
    while (curr) {
        if (!curr->allocated && curr->size >= req_size) {
            if (curr->size > req_size) split_block(curr, size);
            curr->allocated = true;
            curr->id = next_id++;
            used_bytes += req_size;
            id_to_block[curr->id] = curr;
            return get_payload_addr(curr);
        }
        curr = curr->next;
    }
    return 0;  // Fail
}

bool FirstFitStrategy::deallocate(int id, Block** head, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) {
    auto it = id_to_block.find(id);
    if (it == id_to_block.end()) return false;
    Block* block = it->second;
    if (!block->allocated) return false;
    block->allocated = false;
    used_bytes -= block->size;
    id_to_block.erase(it);
    coalesce(block);
    return true;
}
