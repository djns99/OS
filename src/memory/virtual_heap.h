#ifndef OS_VIRTUAL_HEAP_H
#define OS_VIRTUAL_HEAP_H
#include "utility/types.h"

typedef bool(*page_alloc_func_t)( void*, uint32_t );

typedef struct {
    void* start_addr;
    void* end_addr;
    page_alloc_func_t page_alloc_func;
    uint32_t alloc_flags;
} virtual_heap_t;


void init_virtual_heap( virtual_heap_t* heap, void* start_addr, void* end_addr, page_alloc_func_t page_alloc_func, uint32_t alloc_flags );

#endif //OS_VIRTUAL_HEAP_H
