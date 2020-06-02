#include "processes/process.h"
#include "utility/debug.h"
#include "utility/list.h"
#include "interrupt/interrupt.h"
#include "utility/memops.h"
#include "kernel.h"

typedef struct {
    int32_t val;
    list_head_t blocked_list;
} semaphore_t;

semaphore_t semaphore_pool[MAXSEM];

void init_semaphores()
{
    for( uint32_t i = 0; i < MAXSEM; i++ )
        init_list( &semaphore_pool[ i ].blocked_list );
}

void OS_InitSem( int s, int n )
{
    PROCESS_ASSERT( list_is_empty( &semaphore_pool[ s ].blocked_list ), "Process tried to reset in use semaphore" );
    semaphore_pool[ s ].val = n;
}

void OS_Wait( int s )
{
    disable_interrupts();

    // Need to loop since a new process may have stolen the semaphore before any unblocked processes could wake up
    while( semaphore_pool[ s ].val <= 0 )
        block_process( &semaphore_pool[ s ].blocked_list, get_current_process() );

    semaphore_pool[ s ].val--;

    enable_interrupts();
}

void OS_Signal( int s )
{
    disable_interrupts();
    // Release semaphore
    if( ++semaphore_pool[ s ].val > 0 )
        schedule_blocked( &semaphore_pool[ s ].blocked_list );

    enable_interrupts();
} 

