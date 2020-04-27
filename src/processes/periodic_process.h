#ifndef OS_PERIODIC_PROCESS_H
#define OS_PERIODIC_PROCESS_H
#include "process_internal.h"

void init_periodic_state();
bool init_periodic( pcb_t*, uint32_t n );
void yield_periodic();
bool continue_periodic();
bool schedule_next_periodic();
void free_periodic( pcb_t* );

bool periodic_is_ready( pcb_t* );

#endif //OS_PERIODIC_PROCESS_H
