#ifndef OS_PROCESS_H
#define OS_PROCESS_H
#include "kernel.h"
#include "utility/list.h"
#include "utility/types.h"

typedef PID pid_t;
typedef uint32_t tid_t;
typedef int periodic_name_t;
typedef int process_type_t;

typedef void (* process_function_t)( void );

typedef uint32_t sporadic_execution_state_t;
typedef enum {
BLOCKED, READY, EXECUTING
} SporadicExeState;

typedef struct {
    tid_t thread;
    uint8_t* stack_ptr;
    list_node_t list_node;
} thread_state_t;

typedef struct {
    pid_t pid;
    list_node_head_t thread_list;
    list_node_t scheduling_list;
    
    int arg;
    process_function_t function;
    
    process_type_t type;
    // Periodic process info
    periodic_name_t periodic_name;
    
    // Sporadic process info
    SporadicExeState state;
} pcb_t;

pid_t current_process;

void timer_tick( uint32_t tick );
void init_processes();

#endif //OS_PROCESS_H
