#include "processes/process.h"
#include "utility/debug.h"
#include "utility/list.h"
#include "interrupt/interrupt.h"
#include "utility/memops.h"
#include "kernel.h"
#include "processes/syscall.h"

typedef struct {
    int32_t val;
    list_head_t blocked_list;
} semaphore_t;

semaphore_t semaphore_pool[MAXSEM];

int init_sem_syscall( uint32_t s, uint32_t n )
{
    if( s >= MAXSEM )
        return SYS_INVLARG;

    PROCESS_ASSERT( list_is_empty( &semaphore_pool[ s ].blocked_list ), "Process tried to reset in use semaphore" );
    semaphore_pool[ s ].val = n;

    return SYS_SUCCESS;
}

int wait_sem_syscall( uint32_t s, uint32_t _ )
{
    if( s >= MAXSEM )
        return SYS_INVLARG;

    disable_interrupts();

    // Need to loop since a new process may have stolen the semaphore before any unblocked processes could wake up
    while( semaphore_pool[ s ].val <= 0 )
        block_process( &semaphore_pool[ s ].blocked_list, get_current_process() );

    semaphore_pool[ s ].val--;

    enable_interrupts();
    return SYS_SUCCESS;
}

int signal_sem_syscall( uint32_t s, uint32_t _ )
{
    disable_interrupts();
    // Release semaphore
    if( ++semaphore_pool[ s ].val > 0 )
        schedule_blocked( &semaphore_pool[ s ].blocked_list );

    enable_interrupts();
    return SYS_SUCCESS;
}

void OS_InitSem( int s, int n )
{
    int res = syscall( SYSCALL_SEMAPHORE_INIT, s, n );
    PROCESS_WARNING( res == SYS_SUCCESS, "Error initialising semaphore" );
}

void OS_Wait( int s )
{
    int res = syscall( SYSCALL_SEMAPHORE_WAIT, s, 0 );
    PROCESS_WARNING( res == SYS_SUCCESS, "Error waiting for semaphore" );
}

void OS_Signal( int s )
{
    int res = syscall( SYSCALL_SEMAPHORE_SIGNAL, s, 0 );
    PROCESS_WARNING( res == SYS_SUCCESS, "Error signalling semaphore" );
}

void init_semaphores()
{
    for( uint32_t i = 0; i < MAXSEM; i++ )
        init_list( &semaphore_pool[ i ].blocked_list );

    register_syscall_handler( SYSCALL_SEMAPHORE_INIT, &init_sem_syscall );
    register_syscall_handler( SYSCALL_SEMAPHORE_WAIT, &wait_sem_syscall );
    register_syscall_handler( SYSCALL_SEMAPHORE_SIGNAL, &signal_sem_syscall );
}
