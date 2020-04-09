#ifndef OS_IO_PORTS_H
#define OS_IO_PORTS_H

typedef uint16_t port_t;

static uint8_t port_read8( port_t port )
{
    uint8_t res;
    asm( "in %%dx, %%al" : "=a"(res) : "d"(port) );
    return res;
}

static void port_write8( port_t port, uint8_t val )
{
    asm( "out %%al, %%dx" :  : "a"(val), "d"(port) );
}

static uint16_t port_read16( port_t port )
{
    uint16_t res;
    asm( "in %%dx, %%ax" : "=a"(res) : "d"(port) );
    return res;
}

static void port_write16( port_t port, uint16_t val )
{
    asm( "out %%ax, %%dx" :  : "a"(val), "d"(port) );
}

#endif //OS_IO_PORTS_H
