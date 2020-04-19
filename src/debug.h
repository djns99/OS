#ifndef OS_DEBUG_H
#define OS_DEBUG_H

#include "kernel.h"
#include "print.h"

#define KERNEL_ASSERT( condition, msg ) do{ if(!(condition)) {                                                                   \
 print( "\nKernel assert failed at %s:%d\nAssertion:\n\t%s\nWith message:\n\t%s", __FILE__, __LINE__,#condition, (msg) );        \
 OS_Abort();                                                                                                                     \
} } while(0)

#endif //OS_DEBUG_H
