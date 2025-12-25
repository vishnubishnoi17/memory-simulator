# Design Document

## Memory Layout and Assumptions
- Physical memory: Contiguous bytes from 0x0000, explicit block headers (size, flag, links, id).
- Units: Bytes.
- Fragmentation: Internal = (alloc - used)/alloc; External = small free holes / total free.
- Simplifications: Single process for VM; direct-mapped cache; no real disk I/O.

## Allocation Strategies
- First/Best/Worst Fit: Linked list scan, split/coalesce.
- Buddy: Power-2 lists, XOR for buddies.

## Buddy System
- Free lists[0..logN], recursive split.

## Cache Hierarchy
- L1 (small, FIFO), L2 (larger, LRU).
- Access: tag match, evict least recent.

## Virtual Memory Model
- 32-bit VA, 4KB pages, single-level PT.
- Replacement: FIFO queue or LRU timestamps.
- Flow: VA → PT → PA → Cache → Mem.
- Page fault: Evict victim, load, +100 cycle penalty (symbolic).

## Address Translation Flow
1. Extract VPN, check valid.
2. If fault: select victim (FIFO/LRU), evict, assign frame.
3. PA = frame << log(page_size) + offset.

## Limitations
- No multi-process VM.
- Cache: No write-back.
- No L3.

## Diagrams
Memory Blocks: [Block Header]--[Payload]--[Next Block]
Cache: Addr → Set/Tag → Hit? Data : Miss → Lower Level
VM: VA (VPN|Offset) → PTE (valid, frame) → PA (Frame|Offset)
