#include "utility/string.h"
#include "utility/debug.h"
#include "periodic_process.h"

pcb_t* periodic_pool[MAXPROCESS];

// TODO
int PPPLen = 1;
periodic_name_t PPP[MAXPROCESS] = { IDLE };
periodic_name_t PPPMax[MAXPROCESS] = { INT32_MAX };

void init_periodic_state()
{
    os_memset8( periodic_pool, 0x0, sizeof( periodic_pool ) );

    // TODO
}

void free_periodic( pcb_t* pcb )
{
    KERNEL_ASSERT( pcb->type == PERIODIC, "Freed non periodic process in free_periodic()" );
    periodic_pool[ pcb->periodic_name ] = NULL;
    free_common( pcb );
}

void yield_periodic()
{
}

bool continue_periodic()
{
    return 0;
}

bool schedule_next_periodic()
{
    return 0;
}

bool init_periodic( pcb_t* pcb, uint32_t n )
{
    return 0;
}
