#include "utility/debug.h"
#include "interrupt/interrupt.h"
#include "sporadic_process.h"

list_head_t sporadic_scheduling_list;

void init_sporadic_state()
{
    init_list( &sporadic_scheduling_list );
}

void schedule_sporadic()
{
    KERNEL_ASSERT( get_current_process()->interrupt_disables, "Interrupts enabled when trying to schedule sporadic" );

    pcb_t* new_proc = LIST_GET_FIRST( pcb_t, scheduling_list, &sporadic_scheduling_list );
    pcb_t* const original_head = new_proc;
    while ( new_proc && new_proc->state == BLOCKED ) {
        // Still blocked advance the head
        list_advance_head( &sporadic_scheduling_list );
        new_proc = LIST_GET_FIRST( pcb_t, scheduling_list, &sporadic_scheduling_list );
        // All are blocked
        if( new_proc == original_head )
            break;
    } 

    KERNEL_ASSERT( !new_proc || new_proc->state != STOPPED, "Stopped process was still in scheduling list" );
    
    if( !new_proc || new_proc->state == BLOCKED )
        sched_common( &idle_pcb );
    else
        sched_common( new_proc );
}

void free_sporadic( pcb_t* pcb )
{
    KERNEL_ASSERT( pcb->type == SPORADIC, "Freed non sporadic process in free_sporadic()" );
    // Remove from sporadic scheduling list
    list_remove_node( &pcb->scheduling_list );
    free_common( pcb );
}

bool init_sporadic( pcb_t* pcb, uint32_t n )
{
    list_insert_tail_node( &sporadic_scheduling_list, &pcb->scheduling_list );
    return true;
}

void yield_sporadic()
{
    if( get_current_process()->state != STOPPED )
        list_advance_head( &sporadic_scheduling_list );
}