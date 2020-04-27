#include "interrupt/interrupt.h"
#include "utility/debug.h"
#include "device_process.h"

list_head_t device_scheduling_list;

void device_insert_ordered( pcb_t* device_proc )
{
    pcb_t* prev = NULL;
    LIST_FOREACH( pcb_t, scheduling_list, curr, &device_scheduling_list ) {
        if( curr->next_wake_up > device_proc->next_wake_up )
            break;
        prev = curr;
    }

    if( !prev )
        list_insert_head_node( &device_scheduling_list, &device_proc->scheduling_list );
    else
        list_insert_after_node( &prev->scheduling_list, &device_proc->scheduling_list );
}

void free_device( pcb_t* pcb )
{
    KERNEL_ASSERT( pcb->type == DEVICE, "Freed non device process in free_device()" );
    list_remove_node( &pcb->scheduling_list ); // Removes from device scheduling list
    free_common( pcb );
}

bool init_device( pcb_t* pcb, uint32_t n )
{
    pcb->timeout = n;
    pcb->next_wake_up = current_time_slice + pcb->timeout;
    device_insert_ordered( pcb );
    return true;
}

void init_device_state()
{
    init_list( &device_scheduling_list );
}

void yield_device()
{
    pcb_t* device = get_current_process();
    // If we are stopped we dont want to reinsert
    // If we are blocked we want the CPU back ASAP
    if( device->state == STOPPED || device->state == BLOCKED )
        return;
    device->next_wake_up = current_time_slice + device->timeout;
    list_remove_node( &device->scheduling_list );
    device_insert_ordered( device );
}

bool schedule_next_device()
{
    KERNEL_ASSERT( get_current_process()->interrupt_disables, "Interrupts enabled when trying to schedule device" );
    
    LIST_FOREACH( pcb_t, scheduling_list, curr, &device_scheduling_list ) {
        // All the ones after this are still sleeping
        if( curr->next_wake_up > current_time_slice )
            return false;
        if( curr->state != BLOCKED )
            continue;
        sched_common( curr );
        return true;
    }
    
    // All are blocked
    return false;
}
