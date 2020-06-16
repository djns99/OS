#ifndef OS_DEVICE_PROCESS_H
#define OS_DEVICE_PROCESS_H
#include "process_internal.h"

/**
 * Initialise the device process scheduling subsystem
 */
void init_device_state();

/**
 * Initialise a new device process
 * @param pcb The process to init
 * @param n The time between scheduling the device function
 * @return True if successfully initialised, false otherwise
 */
bool init_device( pcb_t* pcb, uint32_t n );

/**
 * Yield a device process
 */
void yield_device();

/**
 * Schedules any device process that need to run in the current time slot
 * @return True if there were any scheduled, false if none were ready
 */
bool schedule_next_device();

/**
 * Free a terminated device process
 * @param pcb The process to free
 */
void free_device( pcb_t* pcb );

#endif //OS_DEVICE_PROCESS_H
