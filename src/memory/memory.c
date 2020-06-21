#include "processes/process.h"
#include "memory.h"
#include "kernel.h"
#include "utility/memops.h"
#include "utility/debug.h"
#include "meminfo.h"
#include "virtual_memory.h"
#include "physical_memory.h"
#include "processes/syscall.h"

bool process_init_memory( pcb_t* pcb )
{
    return init_virtual_heap( &pcb->heap, (void*) PAGE_SIZE, (void*) ( MAX_USER_MEMORY_SIZE - pcb->stack_size ),
                              &alloc_address_range, &free_address_range,
                              PAGE_USER_ACCESSIBLE_FLAG | PAGE_MODIFIABLE_FLAG | PAGE_PRESENT_FLAG );
}

void* user_malloc( uint32_t bytes )
{
    pcb_t* current_proc = get_current_process();
    return virtual_heap_alloc( &current_proc->heap, bytes );
}

bool user_free( void* ptr )
{
    pcb_t* current_proc = get_current_process();
    heap_free_res_t res = virtual_heap_free( &current_proc->heap, (void*) ptr );
    PROCESS_WARNING( res != heap_free_fatal, "Process tried to free an illegal address" );
    return res == heap_free_ok;
}

int malloc_syscall( uint32_t size, uint32_t result )
{
    if( !result )
        return SYS_INVLARG;

    *(MEMORY*) result = (MEMORY) user_malloc( size );
    return SYS_SUCCESS;
}

int free_syscall( uint32_t ptr, uint32_t result )
{
    if( !result || !ptr )
        return SYS_INVLARG;

    *(bool*) result = user_free( (void*) ptr );
    return SYS_SUCCESS;
}

MEMORY OS_Malloc( int size )
{
    if( size <= 0 )
        return NULL;

    MEMORY res;
    if( syscall( SYSCALL_MALLOC, size, (uint32_t) &res ) != SYS_SUCCESS )
        return NULL;

    return res;
}

BOOL OS_Free( MEMORY m )
{
    bool res;
    if( syscall( SYSCALL_FREE, m, (uint32_t) &res ) != SYS_SUCCESS )
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

    bool res = init_kernel_memory();
    KERNEL_ASSERT( res, "Failed to init kernel memory" );
}
