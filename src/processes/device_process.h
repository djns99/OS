#ifndef OS_DEVICE_PROCESS_H
#define OS_DEVICE_PROCESS_H
#include "process_internal.h"

void init_device_state();
bool init_device( pcb_t*, uint32_t n );
void yield_device();
bool schedule_next_device();
void free_device( pcb_t* );

#endif //OS_DEVICE_PROCESS_H
