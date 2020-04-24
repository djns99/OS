#include "memory.h"
#include "virtual_heap.h"

virtual_heap_t kernel_memory_state;

void init_kernel_memory()
{
}

void* kmalloc( uint32_t bytes )
{
    return NULL;
}

void kfree( void* ptr )
{
}