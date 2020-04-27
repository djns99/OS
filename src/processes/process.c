#include "memory/virtual_memory.h"
#include "interrupt/interrupt.h"
#include "utility/debug.h"
#include "utility/memops.h"
#include "process.h"
#include "kernel.h"
#include "utility/types.h"
#include "process_internal.h"

#define DEFAULT_STACK_SIZE 1024*1024

// Define the actual values here so we dont get multiple definitions etc.
pid_t current_process;
pcb_t idle_pcb;
pcb_t pcb_pool[MAXPROCESS];
uint64_t current_time_slice;

list_head_t stopped_processes;

void register_entry_proc()
{
    // Init process to idle
    current_process = IDLE;
    idle_pcb.pid = IDLE;
    idle_pcb.type = PERIODIC; // TODO Is this correct?
    idle_pcb.interrupt_disables = 1; // Start from 1 
}

void init_processes()
{
    KERNEL_ASSERT( INVALIDPID == 0, "Invalid pid is expected to be 0" );
    KERNEL_ASSERT( current_process == IDLE, "Did not register the entry process! Do this first" );
    os_memset8( pcb_pool, 0x0, sizeof( pcb_pool ) );
    init_device_state();
    init_sporadic_state();
    init_periodic_state();

    init_list( &stopped_processes );

    // init_periodic_ordering(); // TODO init idle process
}

pcb_t* alloc_pcb()
{
    for( uint32_t i = 0; i < MAXPROCESS; i++ ) {
        if( pcb_pool[ i ].pid == INVALIDPID ) {
            pcb_t* pcb = pcb_pool + i;
            pcb->pid = i + 1;
            return pcb;
        }
    }

    return NULL;
}

void free_common( pcb_t* pcb )
{
    KERNEL_ASSERT( pcb->interrupt_disables != 0, "Interrupts were enabled while freeing request" );
    if( !pcb->context.cr3 ) {
        pcb->pid = INVALIDPID;
        return;
    }
    pcb->state = STOPPED;

    disable_interrupts();
    list_insert_tail_node( &stopped_processes, &pcb->scheduling_list );
    enable_interrupts();
}

void cleanup_terminated()
{
    list_head_t failed_stops;
    init_list( &failed_stops );
    while( !list_is_empty( &stopped_processes ) ) {
        pcb_t* head = LIST_GET_FIRST( pcb_t, scheduling_list, &stopped_processes );
        list_pop_head( &stopped_processes );

        if( !cleanup_process_address_space( head->context.cr3 ) ) {
            // This process does not have the resources to do cleanup
            list_insert_head_node( &stopped_processes, &head->scheduling_list );
            break;
        }

        // Mark the process as freed
        head->pid = INVALIDPID;
    }
}

void sched_common( pcb_t* new_proc )
{
    disable_interrupts();
    if( new_proc != get_current_process() ) {
        // Set this before the context switch since it is on the stack
        pcb_t* old_proc = get_current_process();
        current_process = new_proc->pid;
        context_switch( &new_proc->context, &old_proc->context );
    }
    // Clean up terminated process
    cleanup_terminated();
    enable_interrupts();
}

void free_process( pcb_t* pcb )
{
    switch( pcb->type ) {
        case PERIODIC:
            free_periodic( pcb );
            break;
        case SPORADIC:
            free_sporadic( pcb );
            break;
        case DEVICE:
            free_device( pcb );
            break;
    }
}

void OS_Terminate()
{
    KERNEL_ASSERT( current_process != IDLE, "Cannot terminate idle process" );
    disable_interrupts();
    pcb_t* pcb = get_current_process();
    free_process( pcb );

    OS_Yield(); // Yield the CPU to another process
    // We should never be rescheduled
    KERNEL_ASSERT( false, "Should never reschedule terminated process" );
    enable_interrupts();
}

void timer_preempt( uint64_t current_tick )
{
    current_time_slice = current_tick;
    disable_interrupts();

    switch( get_current_process()->type ) {
        case PERIODIC:
        case SPORADIC:
            if( !schedule_next_device() )         // Run colliding device process
                if( !schedule_next_periodic() )   // Run preempted periodic process
                    schedule_sporadic();        // Run sporadic process
            break;
        case DEVICE:
            // Cannot be preempted
            break;
    }

    enable_interrupts();
}

pcb_t* get_current_process()
{
    if( current_process == IDLE )
        return &idle_pcb;
    return &pcb_pool[ current_process - 1 ];
}

void OS_Yield()
{
    disable_interrupts();
    switch( get_current_process()->type ) {
        case PERIODIC:
            yield_periodic();
            schedule_sporadic(); // Rerun last sporadic process
            break;
        case SPORADIC:
            yield_sporadic();
            schedule_sporadic(); // Advance head and schedule sporadic
            break;
        case DEVICE:
            yield_device();
            if( !schedule_next_device() ) // Run colliding device process
                if( !continue_periodic() ) // Run preempted periodic process
                    schedule_sporadic(); // Run sporadic process 
            break;
    }
    enable_interrupts();
}

int OS_GetParam()
{
    return get_current_process()->arg;
}

/* Process Management primitives */
PID OS_Create( void (* f)( void ), int arg, unsigned int level, unsigned int n )
{
    disable_interrupts();
    pcb_t* pcb = alloc_pcb();
    if( pcb == NULL )
        return INVALIDPID;

    pcb->type = level;
    pcb->arg = arg;
    pcb->function = f;
    pcb->interrupt_disables = 1;
    pcb->stack_size = DEFAULT_STACK_SIZE;
    pcb->context.cr3 = NULL;
    pcb->context.stack = NULL;

    bool res;
    switch( level ) {
        case PERIODIC:
            res = init_periodic( pcb, n );
            break;
        case SPORADIC:
            res = init_sporadic( pcb, n );
            break;
        case DEVICE:
            res = init_device( pcb, n );
            break;
    }

    if( !res ) {
        free_common( pcb );
        enable_interrupts();
        return INVALIDPID;
    }

    pcb->context.cr3 = init_new_process_address_space( pcb->stack_size );
    // Failed to allocate address space
    if( !pcb->context.cr3 ) {
        free_process( pcb );
        enable_interrupts();
        return INVALIDPID;
    }

    // TODO Does this have to be -16?
    pcb->context.stack = MAX_USER_MEMORY_SIZE - 16;

    fork_process( &pcb->context, &get_current_process()->context, pcb );

    enable_interrupts();

    return pcb->pid;
}

void new_proc_entry_point( void* start_param )
{
    pcb_t* pcb = (pcb_t*) start_param;
    current_process = pcb->pid;
    KERNEL_ASSERT( pcb->interrupt_disables == 1, "Process started with wrong number of interrupts" );
    // We will have interrupt disabled when we start 
    enable_interrupts();

    // Initialise our memory
    process_init_memory( pcb );

    // Child process will be run and terminate
    pcb->function();
    OS_Terminate();
    KERNEL_ASSERT( false, "Terminated process was rescheduled" );
}