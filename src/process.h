#ifndef OS_PROCESS_H
#define OS_PROCESS_H
#include "kernel.h"
#include "list.h"
#include "types.h"

typedef PID pid_t;
typedef uint32_t tid_t;

typedef struct {
    tid_t thread;
    uint8_t* stack_ptr;
    list_node_t list_node;
} thread_state_t;

typedef struct {
    pid_t pid;
    list_node_head_t thread_list;
} pcb_t;

extern pcb_t* current_process;

#endif //OS_PROCESS_H
