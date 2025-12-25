#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include "../../include/memsim.h"
#include <list>

class BuddyStrategy : public AllocatorStrategy {
private:
    std::vector<std::list<Block*>> free_lists;  // Indexed by log2(size)
    uintptr_t total_size_log;

public:
    BuddyStrategy(uintptr_t size);
    uintptr_t allocate(size_t size, Block** head, int& next_id, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) override;
    bool deallocate(int id, Block** head, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) override;
    Block* find_buddy(Block* block);
};

#endif
