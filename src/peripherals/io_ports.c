#include "io_ports.h"

uint8_t port_read8( port_t port )
{
    uint8_t res;
    asm( "in %%dx, %%al" : "=a"(res) : "d"(port));
    return res;
}

void port_write8( port_t port, uint8_t val )
{
    asm( "out %%al, %%dx" :  : "a"(val), "d"(port));
}

uint16_t port_read16( port_t port )
{
    uint16_t res;
    asm( "in %%dx, %%ax" : "=a"(res) : "d"(port));
    return res;
}

void port_write16( port_t port, uint16_t val )
{
    asm( "out %%ax, %%dx" :  : "a"(val), "d"(port));
}

