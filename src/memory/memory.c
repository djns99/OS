#include "processes/process.h"
#include "memory.h"
#include "kernel.h"
#include "utility/memops.h"
#include "utility/debug.h"
#include "meminfo.h"
#include "virtual_memory.h"
#include "physical_memory.h"
#include "processes/syscall.h"

void process_init_memory( pcb_t* pcb )
{
    init_virtual_heap( &pcb->heap, (void*) PAGE_SIZE, (void*) ( MAX_USER_MEMORY_SIZE - pcb->stack_size ),
                       &alloc_address_range, &free_address_range,
                       PAGE_USER_ACCESSIBLE_FLAG | PAGE_MODIFIABLE_FLAG | PAGE_PRESENT_FLAG );
}

int malloc_syscall( uint32_t size, uint32_t result )
{
    if( !result )
        return SYS_INVLARG;

    pcb_t* current_proc = get_current_process();
    void* address = virtual_heap_alloc( &current_proc->heap, size );
    *(MEMORY*)result = (MEMORY) address;
    return SYS_SUCCESS;
}

int free_syscall( uint32_t ptr, uint32_t result )
{
    if( !result || !ptr )
        return SYS_INVLARG;

    pcb_t* current_proc = get_current_process();
    heap_free_res_t res = virtual_heap_free( &current_proc->heap, (void*) ptr );
    PROCESS_WARNING( res != heap_free_oom, "Process had insufficient resources to free memory" );
    *(bool*)result = res == heap_free_ok;
    return SYS_SUCCESS;
}


MEMORY OS_Malloc( int size )
{
    if( size <= 0 )
        return NULL;

    MEMORY res;
    if( syscall( SYSCALL_MALLOC, size, (uint32_t)&res ) != SYS_SUCCESS )
        return NULL;

    return res;
}

BOOL OS_Free( MEMORY m )
{
    PROCESS_ASSERT( m, "Freed NULL pointer" );
    bool res;
    if( syscall( SYSCALL_FREE, m, (uint32_t)&res ) != SYS_SUCCESS )
        return false;
    return res;
}

void OS_InitMemory()
{
    register_syscall_handler( SYSCALL_MALLOC, &malloc_syscall );
    register_syscall_handler( SYSCALL_FREE, &free_syscall );

    init_page_map();
    init_physical_memory();
    init_virtual_memory();
}
