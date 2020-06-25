#include "processes/process.h"
#include "utility/debug.h"
#include "utility/list.h"
#include "interrupt/interrupt.h"
#include "kernel.h"
#include "processes/syscall.h"

typedef struct {
    int32_t val;
    list_head_t blocked_list;
} semaphore_t;

semaphore_t semaphore_pool[MAXSEM];
semaphore_compliance_mode_t compliance_mode = STRICT;

int compliance_sem_syscall( uint32_t compliance_should_get, uint32_t complaince_param )
{
    if( compliance_should_get )
        *(semaphore_compliance_mode_t*) complaince_param = compliance_mode;
    else
        compliance_mode = complaince_param;
    return SYS_SUCCESS;
}

void set_sem_compliance( semaphore_compliance_mode_t mode )
{
    bool valid_mode = mode == STRICT || mode == RELAXED;
    PROCESS_WARNING( valid_mode, "Set illegal semaphore compliance mode. Defaulting to STRICT" );
    if( !valid_mode )
        mode = STRICT;
    int res = syscall( SYSCALL_SEMAPHORE_COMPLIANCE, false, mode );
    KERNEL_WARNING( res == SYS_SUCCESS, "Error changing compliance mode" );
}

semaphore_compliance_mode_t get_sem_compliance()
{
    semaphore_compliance_mode_t mode;
    int res = syscall( SYSCALL_SEMAPHORE_COMPLIANCE, true, (uint32_t) &mode );
    KERNEL_WARNING( res == SYS_SUCCESS, "Error getting compliance mode" );
    return mode;
}

int init_sem_syscall( uint32_t s, uint32_t n )
{
    if( s >= MAXSEM || ( n <= 0 && compliance_mode == STRICT ) )
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

    get_current_process()->held_semaphores[ s ] = false;

    // Need to loop since a new process may have stolen the semaphore before any unblocked processes could wake up
    while( semaphore_pool[ s ].val <= 0 )
        block_process( &semaphore_pool[ s ].blocked_list, get_current_process() );

    semaphore_pool[ s ].val--;
    get_current_process()->held_semaphores[ s ] = true;

    enable_interrupts();
    return SYS_SUCCESS;
}

int signal_sem_syscall( uint32_t s, uint32_t _ )
{
    disable_interrupts();

    bool holds_sem = compliance_mode != STRICT || get_current_process()->held_semaphores[ s ];
    // Release semaphore
    if( holds_sem && ++semaphore_pool[ s ].val > 0 )
        schedule_blocked( &semaphore_pool[ s ].blocked_list );

    get_current_process()->held_semaphores[ s ] = false;
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
    register_syscall_handler( SYSCALL_SEMAPHORE_COMPLIANCE, &compliance_sem_syscall );
}
