#include "buddy_allocator.h"
#include "block.h"
#include <list>
#include <cmath>

BuddyStrategy::BuddyStrategy(uintptr_t size) {
    total_size_log = static_cast<uintptr_t>(std::log2(size));
    free_lists.resize(total_size_log + 1);
    // Init full block
    Block* initial = create_initial_block(size, 0);
    free_lists[total_size_log].push_back(initial);
}

uintptr_t BuddyStrategy::allocate(size_t size, Block** head, int& next_id, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) {
    int order = static_cast<int>(std::ceil(std::log2(size + sizeof(Block))));
    for (int i = order; i <= static_cast<int>(total_size_log); ++i) {
        if (!free_lists[i].empty()) {
            Block* block = free_lists[i].front(); free_lists[i].pop_front();
            while (i > order) {
                --i;
                uintptr_t buddy_addr = (uintptr_t)block + (1ULL << i);
                Block* buddy = (Block*)buddy_addr;
                buddy->size = 1ULL << i;
                buddy->allocated = false;
                buddy->prev = block;
                buddy->next = nullptr;
                block->next = buddy;
                free_lists[i].push_back(buddy);
            }
            block->allocated = true;
            block->id = next_id++;
            used_bytes += block->size;
            id_to_block[block->id] = block;
            *head = block;  // Update head if needed
            return get_payload_addr(block);
        }
    }
    return 0;
}

bool BuddyStrategy::deallocate(int id, Block** head, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) {
    auto it = id_to_block.find(id);
    if (it == id_to_block.end()) return false;
    Block* block = it->second;
    if (!block->allocated) return false;
    block->allocated = false;
    used_bytes -= block->size;
    id_to_block.erase(it);

    // Coalesce with buddy
    Block* buddy = find_buddy(block);
    int order = static_cast<int>(std::log2(block->size));
    if (buddy && !buddy->allocated) {
        // Merge: remove smaller, update larger
        if ((uintptr_t)buddy > (uintptr_t)block) std::swap(block, buddy);
        free_lists[order].remove(buddy);
        block->size *= 2;
        block->next = buddy->next;
        if (block->next) block->next->prev = block;
        order++;
    }
    free_lists[order].push_back(block);
    return true;
}

Block* BuddyStrategy::find_buddy(Block* block) {
    uintptr_t addr = (uintptr_t)block ^ block->size;  // XOR for buddy addr
    return (Block*)addr;  // Assume aligned
}
