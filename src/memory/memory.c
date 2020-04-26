#include "processes/process.h"
#include "memory.h"
#include "kernel.h"
#include "utility/string.h"
#include "utility/debug.h"
#include "utility/helpers.h"
#include "meminfo.h"
#include "virtual_memory.h"
#include "physical_memory.h"

void OS_InitMemory()
{
    init_page_map();
    init_physical_memory();
    init_virtual_memory();
}

void process_init_memory( pcb_t* pcb )
{
    init_virtual_heap( &pcb->heap, (void*) PAGE_SIZE, (void*) MAX_USER_MEMORY_SIZE, alloc_page_at_address, free_page,
                       PAGE_USER_ACCESSIBLE_FLAG | PAGE_MODIFIABLE_FLAG | PAGE_PRESENT_FLAG );
}

MEMORY OS_Malloc( int val )
{
    if( val == 0 )
        return NULL;

    pcb_t* current_proc = get_current_process();

    void* address = virtual_heap_alloc( &current_proc->heap, val );

    // +2 'cause spec
    return (MEMORY)address;
}

BOOL OS_Free( MEMORY m )
{
    PROCESS_ASSERT( m, "Freed NULL pointer" );
    pcb_t* current_proc = get_current_process();
    virtual_heap_free( &current_proc->heap, (void*) m );
    // I don't know why this would fail
    return true;
}

