#include "utility/memops.h"
#include "utility/memops.h"
#include "utility/debug.h"
#include "periodic_process.h"

pcb_t* periodic_pool[MAXPROCESS];

#define NUM_PERIODIC_SLOTS 9
int PPPLen = NUM_PERIODIC_SLOTS;
periodic_name_t PPP[NUM_PERIODIC_SLOTS] = { 1, 2, 3, 4, IDLE, 4, 1, 2, 3 };
periodic_name_t PPPMax[NUM_PERIODIC_SLOTS] = { 10, 15, 7, 11, 1, 9, 10, 5, 13 };
uint64_t next_periodic_start;
uint32_t ppp_index;
bool yielded;

void init_periodic_state()
{
    memset8( periodic_pool, 0x0, sizeof( periodic_pool ) );

    yielded = false;
    ppp_index = 0;
    next_periodic_start = current_time_slice;
}

void free_periodic( pcb_t* pcb )
{
    KERNEL_ASSERT( pcb->type == PERIODIC, "Freed non periodic process in free_periodic()" );
    periodic_pool[ pcb->periodic_name ] = NULL;
    free_common( pcb );
}

void yield_periodic()
{
    // If we are blocked we want the CPU back
    if( get_current_process()->state != BLOCKED )
        yielded = true;
}

bool continue_periodic()
{
    KERNEL_ASSERT( get_current_process()->interrupt_disables, "Interrupts enabled when trying to schedule periodic" );

    if( yielded || periodic_pool[ PPP[ ppp_index ] ]->state == BLOCKED )
        return false;

    sched_common( periodic_pool[ PPP[ ppp_index ] ] );
    return true;
}

bool schedule_next_periodic()
{
    KERNEL_ASSERT( get_current_process()->interrupt_disables, "Interrupts enabled when trying to schedule periodic" );

    if( current_time_slice < next_periodic_start )
        return continue_periodic();

    // Loop while we are behind
    // This will happen if a device process took more than one slot
    // Or if someone had interrupts disabled for a long time
    while( current_time_slice >= next_periodic_start ) {
        ppp_index++;
        ppp_index %= PPPLen;
        next_periodic_start += PPPMax[ ppp_index ];
    }

    // Dont schedule anything if we are idle
    // Or if there is no process currently assigned to this slot
    const periodic_name_t scheduled_process = PPP[ ppp_index ];
    yielded = scheduled_process == IDLE || periodic_pool[ scheduled_process ] == NULL;
    const bool blocked = periodic_pool[ scheduled_process ] && periodic_pool[ scheduled_process ]->state == BLOCKED;
    if( !yielded && !blocked ) {
        sched_common( periodic_pool[ scheduled_process ] );
        return true;
    }
    return false;
}

bool periodic_is_ready( pcb_t* pcb )
{
    const periodic_name_t scheduled_process = PPP[ ppp_index ];
    return scheduled_process != IDLE && periodic_pool[ scheduled_process ] == pcb;
}

bool init_periodic( pcb_t* pcb, uint32_t n )
{
    // Check if process with name already exists
    if( n > MAXPROCESS || periodic_pool[ n ] != NULL )
        return false;

    // Allocate the pool entry
    periodic_pool[ n ] = pcb;
    pcb->periodic_name = n;
    return true;
}
