#ifndef OS_CONTEXT_SWITCH_H
#define OS_CONTEXT_SWITCH_H
#include "interrupt/interrupt_handlers.h"
#include "utility/types.h"

typedef struct {
    uint32_t cr3;
    uint32_t stack;
} __attribute__((packed)) proc_context_t;

/**
 * Perform a context switch. When rescheduled the process will continue from here
 * @param new_process_context The process to switch to
 * @param out_process_context The process to switch from
 */
extern void context_switch( proc_context_t* new_process_context, proc_context_t* out_process_context );

/**
 * Create a new process. The new process will start at the function new_proc_entry_point
 * @param new_process_context The process to create
 * @param parent_process_context The creating process
 * @param start_param A param to identify the new process to new_proc_entry_point
 */
extern void
fork_process( proc_context_t* new_process_context, proc_context_t* parent_process_context, void* start_param );

#endif //OS_CONTEXT_SWITCH_H
