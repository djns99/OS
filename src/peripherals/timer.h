#ifndef OS_TIMER_H
#define OS_TIMER_H
#include "utility/types.h"

/**
 * Initialises the timer subsystem
 * @param freq The frequency at which the timer should tick
 */
void init_timer( uint32_t freq );

/**
 * Get the approximate current up time in us
 * @return The approximate current up time in us
 */
uint64_t get_time_us();

/**
 * Get the approximate current up time in ms
 * @return The approximate current up time in ms
 */
uint64_t get_time_ms();

#endif //OS_TIMER_H
