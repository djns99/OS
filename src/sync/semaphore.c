#include "interrupt/interrupt.h"
#include "utility/memops.h"
#include "kernel.h"

int32_t semaphore_pool[MAXSEM];

void OS_InitSem( int s, int n )
{
    semaphore_pool[ s ] = n;
}

void OS_Wait( int s )
{
    disable_interrupts();

    semaphore_pool[ s ]--;
    // TODO Actual queue here
    // Yield is also not correct behaviour
    while( semaphore_pool[ s ] < 0 )
        OS_Yield();

    enable_interrupts();
}

void OS_Signal( int s )
{
    disable_interrupts();
    // Release semaphore
    semaphore_pool[ s ]++;
    // TODO Wake up higher priority processes
    enable_interrupts();
} 

