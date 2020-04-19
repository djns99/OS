#include "utility/debug.h"
#include "utility/string.h"
#include "process.h"
#include "kernel.h"
#include "utility/types.h"

pcb_t pcb_pool[MAXPROCESS];
pcb_t* periodic_pool[MAXPROCESS];

// TODO
int PPPLen = 1;
periodic_name_t PPP[MAXPROCESS] = { IDLE };
periodic_name_t PPPMax[MAXPROCESS] = { INT32_MAX };

list_node_head_t sporadic_scheduling_list;
list_node_head_t device_scheduling_list;

void init_processes()
{
    KERNEL_ASSERT( INVALIDPID == 0, "Invalid pid is expected to be 0" );
    os_memset( pcb_pool, 0x0, sizeof( pcb_pool ) );
    os_memset( periodic_pool, 0x0, sizeof( periodic_pool ) );

    // init_periodic_ordering(); // TODO init idle process

    init_list( &sporadic_scheduling_list );
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

/* Process Management primitives */
PID OS_Create( void (* f)( void ), int arg, unsigned int level, unsigned int n )
{
    if( level != PERIODIC && level != SPORADIC && level != DEVICE )
        return INVALIDPID;

    if( level == PERIODIC ) {
        if( periodic_pool[ n ] != NULL )  // Allocated two periodic processes with the same name
            return INVALIDPID;
    }

    pcb_t* pcb = alloc_pcb();
    if( pcb == NULL )
        return INVALIDPID;

    init_list( &pcb->thread_list );

    pcb->type = level;
    pcb->arg = arg;
    pcb->function = f;

    if( level == PERIODIC ) {
        periodic_pool[ n ] = pcb;
    } else if( level == SPORADIC ) {
        pcb->state = READY;
        list_insert_tail_node( &sporadic_scheduling_list, &pcb->scheduling_list );
    }
}

void schedule_next_sporadic()
{
    list_advance_head( &sporadic_scheduling_list );
    // schedule_sporadic();
}

void free_periodic( pcb_t* pcb )
{
    KERNEL_ASSERT( pcb->type == PERIODIC, "Freed non periodic process in free_periodic()" );
    pcb->pid = INVALIDPID;
    periodic_pool[ pcb->periodic_name ] = NULL;
}

void free_sporadic( pcb_t* pcb )
{
    KERNEL_ASSERT( pcb->type == SPORADIC, "Freed non sporadic process in free_sporadic()" );
    pcb->pid = INVALIDPID;
    if( pcb->state != BLOCKED )
        list_remove_node( &pcb->scheduling_list ); // Remove from sporadic scheduling list
}

void free_device( pcb_t* pcb )
{
    KERNEL_ASSERT( pcb->type == DEVICE, "Freed non device process in free_device()" );
    pcb->pid = INVALIDPID;
    list_remove_node( &pcb->scheduling_list ); // Removes from device scheduling list 
}

void OS_Terminate()
{
    OS_DI();
    pcb_t* pcb = &pcb_pool[ current_process - 1 ];
    switch( pcb->type ) {
        case PERIODIC:
            free_periodic( pcb );
            // schedule_sporadic();
            break;
        case SPORADIC:
            free_sporadic( pcb );
            schedule_next_sporadic();
            break;
        case DEVICE:
            free_device( pcb );
            // if(!continue_next_device())
            //      if(!continue_periodic())
            //          schedule_sporadic();
            break;
    }
    OS_EI();
}

void OS_Yield()
{
    OS_DI();
    switch( pcb_pool[ current_process - 1 ].type ) {
        case PERIODIC:
            // schedule_sporadic();
            break;
        case SPORADIC:
            schedule_next_sporadic();
            break;
        case DEVICE:
            // if(!continue_next_device())
            //      if(!continue_periodic())
            //          schedule_sporadic();
            break;
    }
    OS_EI();
}

int OS_GetParam()
{
    return pcb_pool[ current_process - 1 ].arg;
}