#ifndef ALLOCATOR_STRATEGY_H
#define ALLOCATOR_STRATEGY_H

#include "block.h"

class FirstFitStrategy : public AllocatorStrategy {
public:
    uintptr_t allocate(size_t size, Block** head, int& next_id, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) override;
    bool deallocate(int id, Block** head, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) override;
};

class BestFitStrategy : public AllocatorStrategy {
public:
    uintptr_t allocate(size_t size, Block** head, int& next_id, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) override;
    bool deallocate(int id, Block** head, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) override;
};

class WorstFitStrategy : public AllocatorStrategy {
public:
    uintptr_t allocate(size_t size, Block** head, int& next_id, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) override;
    bool deallocate(int id, Block** head, uintptr_t& used_bytes, std::map<int, Block*>& id_to_block) override;
};

#endif
