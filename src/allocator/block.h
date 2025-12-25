#ifndef BLOCK_H
#define BLOCK_H

#include "../../include/memsim.h"

// Utils for all allocators
Block* create_initial_block(uintptr_t size, uintptr_t base);
Block* split_block(Block* block, uintptr_t payload_size);
void coalesce(Block* block);
double external_fragmentation(Block* head);
double internal_fragmentation(uintptr_t used, uintptr_t allocated);
uintptr_t get_payload_addr(Block* block);

#endif
