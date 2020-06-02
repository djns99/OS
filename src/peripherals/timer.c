#include "processes/process.h"
#include "timer.h"
#include "interrupt/interrupt_handlers.h"
#include "io_ports.h"

uint64_t ms_tick = 0;
uint64_t tenth_ms_tick = 0;

void handler( interrupt_params_t* r )
{
    if( tenth_ms_tick++ >= 10 ) {
        timer_preempt( ms_tick++ );
        tenth_ms_tick = 0;
    }
}

uint64_t get_time_us()
{
    return ms_tick * 1000 + tenth_ms_tick * 100;
}

uint64_t get_time_ms()
{
    return ms_tick;
}

void init_timer( uint32_t freq )
{
    register_handler( IRQ0, &handler );

    uint32_t divisor = 1193180 / freq;
    uint8_t low = divisor;
    uint8_t high = divisor >> 8u;
    port_write8( 0x43, 0x36 );
    port_write8( 0x40, low );
    port_write8( 0x40, high );
}