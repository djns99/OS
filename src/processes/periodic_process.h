#ifndef OS_PERIODIC_PROCESS_H
#define OS_PERIODIC_PROCESS_H
#include "process_internal.h"

/**
 * Initialise the periodic process scheduling subsystem
 */
void init_periodic_state();

/**
 * Initialise a new periodic process
 * @param pcb The process to init
 * @param n The periodic id of the process
 * @return True if successfully initialised, false otherwise
 */
bool init_periodic( pcb_t*, uint32_t n );

/**
 * Yield a periodic process
 */
void yield_periodic();

/**
 * Schedule the next periodic process
 * @return True if one was scheduled, false otherwise
 */
bool schedule_next_periodic();

/**
 * Free a terminated periodic process
 * @param pcb The process to free
 */
void free_periodic( pcb_t* pcb );

/**
 * Check if a blocked periodic process can be run in the current slot
 * @param pcb The pcb of the process to check
 * @return True if the current time slot belongs to the process, false otherwise
 */
bool periodic_is_ready( pcb_t* pcb );

#endif //OS_PERIODIC_PROCESS_H
