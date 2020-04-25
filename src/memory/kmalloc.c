#include "memory.h"
#include "virtual_memory.h"
#include "virtual_heap.h"

virtual_heap_t kernel_memory_state;

void init_kernel_memory()
{
    init_virtual_heap( &kernel_memory_state, kernel_heap_start(), kernel_heap_end(), kalloc_page_at_address, PAGE_PRESENT_FLAG | PAGE_MODIFIABLE_FLAG );
}

void* kmalloc( uint32_t bytes )
{
    return NULL;
}

void kfree( void* ptr )
{
}