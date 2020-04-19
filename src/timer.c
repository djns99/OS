#include "timer.h"
#include "interrupt_handling/interrupt_handlers.h"
#include "io_ports.h"
#include "print.h"

uint64_t tick = 0;
void handler( registers_t r )
{

}

void init_timer( uint32_t freq )
{
    register_handler( IRQ0, &handler );
    
    uint32_t divisor = 1193180 / freq;
    uint8_t low = divisor;
    uint8_t high = divisor>>8;
    port_write8( 0x43, 0x36 );
    port_write8( 0x40, low );
    port_write8( 0x40, high );
}