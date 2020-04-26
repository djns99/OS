#include "interrupt/interrupt.h"
#include "utility/debug.h"
#include "utility/string.h"
#include "process.h"
#include "kernel.h"
#include "utility/types.h"
#include "process_internal.h"

list_head_t stopped_processes;

void init_processes()
{
    KERNEL_ASSERT( INVALIDPID == 0, "Invalid pid is expected to be 0" );
    os_memset8( pcb_pool, 0x0, sizeof( pcb_pool ) );
    init_device_state();
    init_sporadic_state();
    init_periodic_state();

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
    pcb->state = STOPPED;
}

void sched_common( pcb_t* new_proc )
{
    disable_interrupts();
    if( new_proc != get_current_process() ) {
        context_switch( &new_proc->context, &get_current_process()->context );
        current_process = new_proc->pid;
    }
    enable_interrupts();
}

void OS_Terminate()
{
    disable_interrupts(); // TODO Spinloock
    pcb_t* pcb = &pcb_pool[ current_process - 1 ];
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

    OS_Yield(); // Yield the CPU
    enable_interrupts();
}

void timer_tick( uint32_t current_tick )
{
    disable_interrupts();

    switch( pcb_pool[ current_process - 1 ].type ) {
        case PERIODIC:
        case SPORADIC:
            if( !schedule_next_device() )         // Run colliding device process
                if( !schedule_next_periodic() )   // Run preempted periodic process
                    schedule_sporadic();        // Run sporadic process 
        case DEVICE:
            // Cannot be preempted
            return;
    }

    enable_interrupts();
}

pcb_t* get_current_process()
{
    return &pcb_pool[ current_process - 1 ];
}

void OS_Yield()
{
    disable_interrupts();
    switch( pcb_pool[ current_process - 1 ].type ) {
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
    return pcb_pool[ current_process - 1 ].arg;
}

/* Process Management primitives */
PID OS_Create( void (* f)( void ), int arg, unsigned int level, unsigned int n )
{
    disable_interrupts();
    pcb_t* pcb = alloc_pcb();
    if( pcb == NULL )
        return INVALIDPID;

    init_list( &pcb->thread_list );

    pcb->type = level;
    pcb->arg = arg;
    pcb->function = f;

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

//    init_new_pcb_memory( pcb );

    enable_interrupts();
    return pcb->pid;
}