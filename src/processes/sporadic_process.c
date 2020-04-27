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
    
    if( list_is_empty( &sporadic_scheduling_list ) ) {
        // We have no work to do, schedule the idle process until the next process
        sched_common( &idle_pcb );
        return;
    }
    
    pcb_t* new_proc = LIST_GET_FIRST( pcb_t, scheduling_list, &sporadic_scheduling_list );
    sched_common( new_proc );
}

void free_sporadic( pcb_t* pcb )
{
    KERNEL_ASSERT( pcb->type == SPORADIC, "Freed non sporadic process in free_sporadic()" );
    if( pcb->state != BLOCKED )
        list_remove_node( &pcb->scheduling_list ); // Remove from sporadic scheduling list
    free_common( pcb );
}

bool init_sporadic( pcb_t* pcb, uint32_t n )
{
    pcb->state = READY;
    list_insert_tail_node( &sporadic_scheduling_list, &pcb->scheduling_list );
    return true;
}

void yield_sporadic()
{
    list_advance_head( &sporadic_scheduling_list );
}