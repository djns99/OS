#include "memory/virtual_memory.h"
#include "interrupt/interrupt.h"
#include "utility/debug.h"
#include "utility/memops.h"
#include "process.h"
#include "kernel.h"
#include "utility/types.h"
#include "process_internal.h"
#include "processes/syscall.h"

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
    idle_pcb.type = PERIODIC;
    idle_pcb.state = EXECUTING;
    idle_pcb.interrupt_disables = 1; // Start from 1
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
    KERNEL_ASSERT( new_proc->state != BLOCKED && new_proc->state != STOPPED,
                   "Scheduled process that should not be running" );
    if( new_proc != get_current_process() ) {
        // Set this before the context switch since it is on the stack
        pcb_t* old_proc = get_current_process();
        if( old_proc->state == EXECUTING )
            old_proc->state = READY;
        current_process = new_proc->pid;
        context_switch( &new_proc->context, &old_proc->context );

        get_current_process()->state = EXECUTING;
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

void schedule_blocked( list_head_t* blocked_list )
{
    process_type_t running_type = get_current_process()->type;

    pcb_t* highest_priority = NULL;

    LIST_FOREACH( pcb_t, blocked_list, curr, blocked_list ) {
        KERNEL_ASSERT( highest_priority != curr, "Infinite loop" );

        if( curr->type == DEVICE ) {
            // We are a blocked device function
            // We always take highest priority
            highest_priority = curr;
            break;
        }

        if( curr->type == PERIODIC && periodic_is_ready( curr ) ) {
            // Only one periodic can be active at a time
            // So we are highest priority periodic
            // Need to keep looping in case there is a device function
            highest_priority = curr;
            continue;
        }

        if( !highest_priority || highest_priority->type < curr->type )
            highest_priority = curr;

        // We have a lower priority than a previous one
    }

    // No blocked processes
    if( !highest_priority )
        return;

    highest_priority->state = READY;
    // Remove the node from the blocked list
    list_remove_node( &highest_priority->blocked_list );

    if( running_type == DEVICE )
        return; // Favour the already running device function
    else if( highest_priority->type == DEVICE )
        schedule_next_device();
    else if( running_type == PERIODIC )
        return; // If we are periodic we must be the only one with the active slot
    else if( highest_priority->type == PERIODIC && periodic_is_ready( highest_priority ) )
        schedule_next_periodic(); // Give the time slot back to the periodic process

    // We are both sporadic, or the other's time slot has expired.
    // Keep the currently running process
    KERNEL_ASSERT( running_type == SPORADIC, "Expected sporadic running type" );
}

void block_process( list_head_t* blocked_list, pcb_t* pcb )
{
    pcb->state = BLOCKED;
    list_insert_tail_node( blocked_list, &pcb->blocked_list );

    // Yield the CPU
    OS_Yield();

    KERNEL_ASSERT( pcb->state != BLOCKED, "Blocked process was rescheduled" );
}

int get_param_syscall( uint32_t res, uint32_t _2 )
{
    if( !res )
        return SYS_INVLARG;
    *(int*) res = get_current_process()->arg;
    return SYS_SUCCESS;
}

int OS_GetParam()
{
    int param;
    int sys_res = syscall( SYSCALL_PROCESS_GET_PARAM, (uint32_t) &param, 0 );
    PROCESS_WARNING( sys_res == SYS_SUCCESS, "Error getting process param" );
    return param;
}

int yield_syscall()
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
                if( !schedule_next_periodic() ) // Run preempted periodic process
                    schedule_sporadic(); // Run sporadic process
            break;
    }
    enable_interrupts();
    return SYS_SUCCESS;
}

void OS_Yield()
{
    int res = syscall( SYSCALL_PROCESS_YIELD, 0, 0 );
    KERNEL_WARNING( res == SYS_SUCCESS, "Error yielding process" );
}

int terminate_syscall()
{
    disable_interrupts();
    pcb_t* pcb = get_current_process();
    free_process( pcb );

    yield_syscall(); // Yield the CPU to another process
    // We should never be rescheduled
    KERNEL_ASSERT( false, "Should never reschedule terminated process" );
    enable_interrupts();
    return SYS_FAILED;
}

void OS_Terminate()
{
    KERNEL_ASSERT( current_process != IDLE, "Cannot terminate idle process" );
    syscall( SYSCALL_PROCESS_TERMINATE, 0, 0 );
    KERNEL_ASSERT( false, "Terminate should never return" );
}

typedef struct {
    void (* f)();
    int arg;
    uint32_t level;
    uint32_t n;
    PID out_pid;
} create_syscall_args_t;

int create_syscall( uint32_t param, uint32_t _ )
{
    create_syscall_args_t* args = (create_syscall_args_t*) param;
    if( !args || !args->f )
        return SYS_INVLARG;

    disable_interrupts();
    pcb_t* pcb = alloc_pcb();
    if( pcb == NULL ) {
        enable_interrupts();
        return SYS_FAILED;
    }

    pcb->type = args->level;
    pcb->arg = args->arg;
    pcb->function = args->f;
    pcb->interrupt_disables = 1;
    pcb->stack_size = DEFAULT_STACK_SIZE;
    pcb->context.cr3 = NULL;
    pcb->context.stack = NULL;
    pcb->state = READY;
    init_variable_bitmap( pcb->held_semaphores, MAXSEM );
    bitmap_clear_all( pcb->held_semaphores );

    bool res = false;
    switch( args->level ) {
        case PERIODIC:
            res = init_periodic( pcb, args->n );
            break;
        case SPORADIC:
            res = init_sporadic( pcb, args->n );
            break;
        case DEVICE:
            res = init_device( pcb, args->n );
            break;
    }

    if( !res ) {
        free_common( pcb );
        enable_interrupts();
        return SYS_FAILED;
    }

    pcb->context.cr3 = init_new_process_address_space( pcb->stack_size );
    // Failed to allocate address space
    if( !pcb->context.cr3 ) {
        free_process( pcb );
        enable_interrupts();
        return SYS_FAILED;
    }

    pcb->context.stack = MAX_USER_MEMORY_SIZE - 1;

    fork_process( &pcb->context, &get_current_process()->context, pcb );

    enable_interrupts();

    args->out_pid = pcb->pid;
    return SYS_SUCCESS;
}

/* Process Management primitives */
PID OS_Create( void (* f)( void ), int arg, unsigned int level, unsigned int n )
{
    if( !f )
        return INVALIDPID;
    create_syscall_args_t args = { f, arg, level, n, INVALIDPID };
    int res = syscall( SYSCALL_PROCESS_CREATE, (uint32_t) &args, 0 );
    KERNEL_WARNING( res == SYS_SUCCESS || res == SYS_FAILED, "Error while creating child process" );
    return res == SYS_SUCCESS ? args.out_pid : INVALIDPID;
}

void init_processes()
{
    KERNEL_ASSERT( INVALIDPID == 0, "Invalid pid is expected to be 0" );
    KERNEL_ASSERT( current_process == IDLE, "Did not register the entry process! Do this first" );
    memset8( pcb_pool, 0x0, sizeof( pcb_pool ) );
    init_list( &stopped_processes );
    init_device_state();
    init_sporadic_state();
    init_periodic_state();

    register_syscall_handler( SYSCALL_PROCESS_GET_PARAM, &get_param_syscall );
    register_syscall_handler( SYSCALL_PROCESS_TERMINATE, &terminate_syscall );
    register_syscall_handler( SYSCALL_PROCESS_YIELD, &yield_syscall );
    register_syscall_handler( SYSCALL_PROCESS_CREATE, &create_syscall );
}

__attribute__((unused)) void new_proc_entry_point( void* start_param )
{
    pcb_t* pcb = (pcb_t*) start_param;
    current_process = pcb->pid;
    KERNEL_ASSERT( pcb->interrupt_disables == 1, "Process started with wrong number of interrupts" );
    // We will have interrupt disabled when we start
    enable_interrupts();

    // Initialise our memory
    bool res = process_init_memory( pcb );
    PROCESS_ASSERT( res, "Failed to allocate memory for new process" );

    // Child process will be run and terminate
    pcb->function();
    OS_Terminate();
    KERNEL_ASSERT( false, "Terminated process was rescheduled" );
}