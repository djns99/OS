#include "utility/memops.h"
#include "utility/memops.h"
#include "utility/debug.h"
#include "periodic_process.h"

pcb_t* periodic_pool[MAXPROCESS];

// TODO
int PPPLen = 1;
periodic_name_t PPP[MAXPROCESS] = { IDLE };
periodic_name_t PPPMax[MAXPROCESS] = { INT32_MAX };
uint64_t next_periodic_start;
uint32_t ppp_index;
bool yielded;

void init_periodic_state()
{
    os_memset8( periodic_pool, 0x0, sizeof( periodic_pool ) );

    // TODO Init plan

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
    yielded = true;
}

bool continue_periodic()
{
    KERNEL_ASSERT( get_current_process()->interrupt_disables, "Interrupts enabled when trying to schedule periodic" );

    if( yielded )
        return false;

    sched_common( periodic_pool[ ppp_index ] );
    return true;
}

bool schedule_next_periodic()
{
    KERNEL_ASSERT( get_current_process()->interrupt_disables, "Interrupts enabled when trying to schedule periodic" );

    if( current_time_slice < next_periodic_start )
        return false;

    // Loop while we are behind
    // This will happen if a device process took more than one slot
    // Or if someone had interrupts disabled for a long time
    while( current_time_slice >= next_periodic_start ) {
        ppp_index++;
        ppp_index %= PPPLen;
        next_periodic_start += PPPMax[ ppp_index ];
    }

    // Dont schedule anything if we are idle
    yielded = PPP[ ppp_index ] == IDLE;
    return !yielded;
}

bool init_periodic( pcb_t* pcb, uint32_t n )
{
    // Check if process with name already exists
    if( periodic_pool[ n ] != NULL )
        return false;

    // Allocate the pool entry
    periodic_pool[ n ] = pcb;
    return true;
}
