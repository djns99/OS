#include "utility/debug.h"
#include "interrupt/interrupt.h"
#include "memory.h"
#include "virtual_memory.h"
#include "virtual_heap.h"

virtual_heap_t kernel_memory_state;

void init_kernel_memory()
{
    init_virtual_heap( &kernel_memory_state, kernel_heap_start(), kernel_heap_end(), kalloc_page_at_address, kfree_page, PAGE_PRESENT_FLAG | PAGE_MODIFIABLE_FLAG );
}

void* kmalloc( uint32_t bytes )
{
    disable_interrupts();
    void* allocation = virtual_heap_alloc( &kernel_memory_state, bytes );
    enable_interrupts();
    if( !allocation )
        // TODO Reap processes
        return NULL;
    return allocation;
}

void kfree( void* ptr )
{
    disable_interrupts();
    bool res = virtual_heap_free( &kernel_memory_state, ptr );
    enable_interrupts();
    KERNEL_ASSERT( res, "Illegal free in kernel" );
}