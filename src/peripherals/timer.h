#ifndef OS_TIMER_H
#define OS_TIMER_H
#include "utility/types.h"

void init_timer( uint32_t freq );
uint64_t get_time_us();
uint64_t get_time_ms();

#endif //OS_TIMER_H
