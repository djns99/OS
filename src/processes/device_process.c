#include "utility/debug.h"
#include "device_process.h"

list_head_t device_scheduling_list;

void free_device( pcb_t* pcb )
{
    KERNEL_ASSERT( pcb->type == DEVICE, "Freed non device process in free_device()" );
    list_remove_node( &pcb->scheduling_list ); // Removes from device scheduling list
    free_common( pcb );
}

bool init_device( pcb_t* pcb, uint32_t n )
{
    pcb->timeout = n;
    return true;
}

void init_device_state()
{
}

void yield_device()
{
}

bool schedule_next_device()
{
    return 0;
}
