#include "processes/process.h"
#include "memory.h"
#include "kernel.h"
#include "utility/memops.h"
#include "utility/debug.h"
#include "utility/math.h"
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
    init_virtual_heap( &pcb->heap, (void*) PAGE_SIZE, (void*) ( MAX_USER_MEMORY_SIZE - pcb->stack_size ),
                       &alloc_address_range, &free_address_range,
                       PAGE_USER_ACCESSIBLE_FLAG | PAGE_MODIFIABLE_FLAG | PAGE_PRESENT_FLAG );
}

MEMORY OS_Malloc( int size )
{
    if( size <= 0 )
        return NULL;

    pcb_t* current_proc = get_current_process();
    void* address = virtual_heap_alloc( &current_proc->heap, size );
    return (MEMORY) address;
}

BOOL OS_Free( MEMORY m )
{
    PROCESS_ASSERT( m, "Freed NULL pointer" );
    pcb_t* current_proc = get_current_process();
    heap_free_res_t res = virtual_heap_free( &current_proc->heap, (void*) m );
    if( res == heap_free_ok )
        return true;

    PROCESS_WARNING( res != heap_free_oom, "Process had insufficient resources to free memory" );
    return false;
}

