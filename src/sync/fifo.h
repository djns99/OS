#ifndef OS_FIFO_H
#define OS_FIFO_H
#include "kernel.h"

/**
 * Init the FIFO subsystem
 */
void init_fifos();

/**
 * Release a fifo
 * @param f The fifo to release
 * @return True if the FIFO was released, false otherwise
 */
bool release_fifo( FIFO f );

#endif //OS_FIFO_H
