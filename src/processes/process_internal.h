#ifndef OS_PROCESS_INTERNAL_H
#define OS_PROCESS_INTERNAL_H
#include "process.h"
#include "device_process.h"
#include "sporadic_process.h"
#include "periodic_process.h"

extern pid_t current_process;
extern pcb_t idle_pcb;
extern pcb_t pcb_pool[MAXPROCESS];
extern uint64_t current_time_slice;

/**
 * A common cleanup function for all process types
 */
void free_common( pcb_t* );

/**
 * The common scheduling function for all process types. Actually performs the context switch
 * @param new_proc The process to switch to
 */
void sched_common( pcb_t* new_proc );
#endif //OS_PROCESS_INTERNAL_H
