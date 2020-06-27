#ifndef OS_PROCESS_H
#define OS_PROCESS_H
#include "memory/virtual_heap.h"
#include "kernel.h"
#include "utility/list.h"
#include "utility/types.h"
#include "sync/semaphore.h"
#include "context_switch.h"
#include "utility/bitmap.h"

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
    DECLARE_VARIABLE_BITMAP( held_semaphores, MAXSEM );

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

/**
 * Gets the currently running process
 * @return The pcb for the current process
 */
pcb_t* get_current_process();

/**
 * Notify the process scheduling subsystem of a timer event
 * @param current_tick The current time slice
 */
void timer_preempt( uint64_t current_tick );

/**
 * Init the process scheduling subsystem
 */
void init_processes();

/**
 * Registers the idle process to the kernel start state
 */
void register_entry_proc();

/**
 * Schedule blocked process. Chooses the highest priority process from the list
 * and schedules it if it has higher priority than the current process
 * @param blocked_list This list of blocked processes to release
 */
void schedule_blocked( list_head_t* blocked_list );

/**
 * Blocks the specified process and stores it on the provided list
 * @param blocked_list The list of processes to add to
 * @param pcb The process being blocked
 */
void block_process( list_head_t* blocked_list, pcb_t* pcb );

#endif //OS_PROCESS_H
