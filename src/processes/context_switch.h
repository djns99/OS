#ifndef OS_CONTEXT_SWITCH_H
#define OS_CONTEXT_SWITCH_H
#include "interrupt/interrupt_handlers.h"
#include "utility/types.h"

typedef struct {
    uint32_t cr3;
    uint32_t stack;
} __attribute__((packed)) proc_context_t;
extern void context_switch( proc_context_t* new_process_context, proc_context_t* out_process_context );
extern void fork_process( proc_context_t* new_process_context, proc_context_t* parent_process_context, void* start_param );

#endif //OS_CONTEXT_SWITCH_H
