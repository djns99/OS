#ifndef OS_DEBUG_H
#define OS_DEBUG_H

#include "kernel.h"
#include "print.h"

#define KERNEL_ASSERT( condition ) do{ if(!(condition)) {                                                  \
 print( "\nKernel assert failed at on condition " #condition ", at %s:%d\n", __FILE__, __LINE__ );        \
 OS_Abort();                                                                                            \
} } while(0)

#endif //OS_DEBUG_H
