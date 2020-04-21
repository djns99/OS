#ifndef OS_SPORADIC_PROCESS_H
#define OS_SPORADIC_PROCESS_H
#include "process_internal.h"

void init_sporadic_state();
bool init_sporadic( pcb_t*, uint32_t n );
void yield_sporadic();
void schedule_sporadic();
void free_sporadic( pcb_t* );

#endif //OS_SPORADIC_PROCESS_H
