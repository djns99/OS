#ifndef OS_IO_PORTS_H
#define OS_IO_PORTS_H
#include "utility/types.h"
typedef uint16_t port_t;

uint8_t port_read8( port_t port );
void port_write8( port_t port, uint8_t val );
uint16_t port_read16( port_t port );
void port_write16( port_t port, uint16_t val );

#endif //OS_IO_PORTS_H
