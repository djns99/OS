#ifndef OS_MEMORY_H
#define OS_MEMORY_H
#include "types.h"
#define MIN_ALLOC_SIZE_LOG ((uint32_t)1u)
#define MIN_ALLOC_SIZE (1u<<MIN_ALLOC_SIZE_LOG)
#define ALLOC_RESERVED_SIZE_LOG ((uint32_t)1u)
#define ALLOC_RESERVED_SIZE (1u<<ALLOC_RESERVED_SIZE_LOG)

#define FREE_BIT 15
#define FREE_BIT_MASK (1u<<FREE_BIT)
#define FIRST_SLAB_ELEM_BIT 14
#define FIRST_SLAB_ELEM_BIT_MASK (1u<<FIRST_SLAB_ELEM_BIT)
#define NUM_FLAGS 2
// 4096+64 provides efficient packing for control blocks
#define SLAB_SIZE 4096+64

#define MAX_CACHE_ENTRIES (31 - MIN_ALLOC_SIZE_LOG)

typedef uint16_t slab_elem_cb_t;

typedef uint8_t slab_t[SLAB_SIZE];

typedef struct
{
    uint32_t num_slabs;
    slab_t** slabs;
} slab_list_t;

typedef struct
{
    slab_list_t slab_cache[MAX_CACHE_ENTRIES]; // List for each order of magnitude
    slab_list_t free_slabs;
} memory_state_t;

// TODO this should be part of the PCB?
memory_state_t memory_state;

#endif //OS_MEMORY_H
