#ifndef OS_PROCESS_INTERNAL_H
#define OS_PROCESS_INTERNAL_H
#include "process.h"
#include "device_process.h"
#include "sporadic_process.h"
#include "periodic_process.h"

pcb_t pcb_pool[MAXPROCESS];

pcb_t* alloc_common();
void free_common( pcb_t* );
void sched_common( pcb_t* new_proc );
#endif //OS_PROCESS_INTERNAL_H
