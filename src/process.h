#ifndef OS_PROCESS_H
#define OS_PROCESS_H
#include "memory.h"
#include "list.h"
#include "types.h"

typedef uint32_t pid_t;
typedef uint32_t tid_t;

typedef struct {
    // Instruction pointer
    uint32_t eip;
    // Stack pointers
    uint32_t esp;
    uint32_t ebp;
    // General purpose
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    // Flags register
    uint32_t flags;
} register_state_t;

typedef struct
{
    tid_t thread;
    register_state_t state;
    list_node_t list_node;
} thread_state_t;

typedef struct
{
    pid_t pid;
    list_node_head_t thread_list;
    memory_state_t memory_state;
} pcb_t;

#endif //OS_PROCESS_H
