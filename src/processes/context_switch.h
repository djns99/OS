#ifndef OS_CONTEXT_SWITCH_H
#define OS_CONTEXT_SWITCH_H
#include "interrupt/interrupt_handlers.h"
#include "utility/types.h"

typedef struct {
    uint32_t cr3;
    uint32_t stack;
} __attribute__((packed)) proc_context_t;
extern void context_switch( proc_context_t* new_sp, proc_context_t* out_sp );

#endif //OS_CONTEXT_SWITCH_H
