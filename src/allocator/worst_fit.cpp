#include "allocator_strategy.h"
#include "block.h"
#include <cstdint>

uintptr_t WorstFitStrategy::allocate(size_t size, Block** head, int& next_id, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) {
    uintptr_t req_size = sizeof(Block) + size;
    Block* curr = *head;
    Block* worst = nullptr;
    uintptr_t worst_fit = 0;
    while (curr) {
        if (!curr->allocated && curr->size >= req_size && curr->size > worst_fit) {
            worst = curr;
            worst_fit = curr->size;
        }
        curr = curr->next;
    }
    if (!worst) return 0;
    if (worst->size > req_size) split_block(worst, size);
    worst->allocated = true;
    worst->id = next_id++;
    used_bytes += req_size;
    id_to_block[worst->id] = worst;
    return get_payload_addr(worst);
}

bool WorstFitStrategy::deallocate(int id, Block** head, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) {
    return FirstFitStrategy().deallocate(id, head, used_bytes, id_to_block);
}
