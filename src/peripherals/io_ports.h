#ifndef OS_IO_PORTS_H
#define OS_IO_PORTS_H
#include "utility/types.h"

typedef uint16_t port_t;

/**
 * Read an 8 bit value from a port
 * @param port The port to read from
 * @return The value read
 */
uint8_t port_read8( port_t port );
/**
 * Write an 8 bit value to a port
 * @param port The port to write to
 * @param val The value to write
 */
void port_write8( port_t port, uint8_t val );

/**
 * Read a 16 bit value from a port
 * @param port The port to read from
 * @return The value read
 */
uint16_t port_read16( port_t port );
/**
 * Write a 16 bit value to a port
 * @param port The port to write to
 * @param val The value to write
 */
void port_write16( port_t port, uint16_t val );

#endif //OS_IO_PORTS_H
