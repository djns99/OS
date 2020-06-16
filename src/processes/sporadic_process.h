#ifndef OS_SPORADIC_PROCESS_H
#define OS_SPORADIC_PROCESS_H
#include "process_internal.h"

/**
 * Initialise the sporadic process scheduling subsystem
 */
void init_sporadic_state();

/**
 * Initialise a new sporadic process
 * @param pcb The process to init
 * @param n Ignored
 * @return True if successfully initialised, false otherwise
 */
bool init_sporadic( pcb_t*, uint32_t n );

/**
 * Yield a sporadic process
 */
void yield_sporadic();

/**
 * Schedule the next sporadic process. Or the idle process if there are none
 */
void schedule_sporadic();

/**
 * Free a terminated sporadic process
 * @param pcb The process to free
 */
void free_sporadic( pcb_t* );

#endif //OS_SPORADIC_PROCESS_H
