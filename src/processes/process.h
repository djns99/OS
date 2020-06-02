#ifndef OS_PROCESS_H
#define OS_PROCESS_H
#include "memory/virtual_heap.h"
#include "kernel.h"
#include "utility/list.h"
#include "utility/types.h"
#include "context_switch.h"

typedef PID pid_t;
typedef uint32_t tid_t;
typedef int periodic_name_t;
typedef int process_type_t;

typedef void (* process_function_t)( void );

typedef enum {
    BLOCKED, READY, EXECUTING, STOPPED
} execution_state_t;

typedef struct {
    pid_t pid;
    list_node_t scheduling_list;
    list_node_t blocked_list;
    proc_context_t context;
    virtual_heap_t heap;
    uint32_t stack_size;
    uint32_t interrupt_disables;
    execution_state_t state;

    int arg;
    process_function_t function;

    process_type_t type;
    union {
        struct {
            // Periodic process info
            periodic_name_t periodic_name;
        };

        struct {
            // Sporadic process info
        };

        struct {
            // Device process info
            uint32_t timeout;
            uint32_t next_wake_up;
        };
    };
} pcb_t;

pcb_t* get_current_process();

void timer_preempt( uint64_t current_tick );
void init_processes();
void register_entry_proc();

// Chooses the highest priority process from the list and schedules it
void schedule_blocked( list_head_t* blocked_list );

// Blocks the specified process on the provided list
void block_process( list_head_t* blocked_list, pcb_t* pcb );

#endif //OS_PROCESS_H
