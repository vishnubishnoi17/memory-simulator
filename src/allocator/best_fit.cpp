#include "allocator_strategy.h"
#include "block.h"
#include <cstdint>

uintptr_t BestFitStrategy::allocate(size_t size, Block** head, int& next_id, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) {
    uintptr_t req_size = sizeof(Block) + size;
    Block* curr = *head;
    Block* best = nullptr;
    uintptr_t best_fit = UINT64_MAX;
    while (curr) {
        if (!curr->allocated && curr->size >= req_size && curr->size < best_fit) {
            best = curr;
            best_fit = curr->size;
        }
        curr = curr->next;
    }
    if (!best) return 0;
    if (best->size > req_size) split_block(best, size);
    best->allocated = true;
    best->id = next_id++;
    used_bytes += req_size;
    id_to_block[best->id] = best;
    return get_payload_addr(best);
}

bool BestFitStrategy::deallocate(int id, Block** head, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) {
    return FirstFitStrategy().deallocate(id, head, used_bytes, id_to_block);
}
