#ifndef OS_DEBUG_H
#define OS_DEBUG_H

#include "kernel.h"
#include "print.h"

// Stub function to break on in GDB
static inline void debug_kern_warn_break()
{}

#define KERNEL_ASSERT( condition, msg ) do{ if(!(condition)) {                                                                   \
 set_fg_colour( TEXT_RED );                                                                                                      \
 print( "\nKernel assert failed at %s:%d\nAssertion:\n\t%s\nWith message:\n\t%s\n", __FILE__, __LINE__,#condition, (msg) );      \
 OS_Abort();                                                                                                                     \
} } while(0)

#define KERNEL_WARNING( condition, msg ) do{ if(!(condition)) {                                                                   \
 int old_colour = get_fg_colour();                                                                                                \
 set_fg_colour( TEXT_BROWN );                                                                                                     \
 print( "\nKernel warning at %s:%d\nAssertion:\n\t%s\nWith message:\n\t%s\n", __FILE__, __LINE__,#condition, (msg) );             \
 debug_kern_warn_break(); \
 set_fg_colour( old_colour ); \
} } while(0)

#define PROCESS_ASSERT( condition, msg ) do{ if(!(condition)) {                                                                   \
 int old_colour = get_fg_colour();                                                                                                \
 set_fg_colour( TEXT_RED );                                                                                                       \
 print( "\nUser process assert failed at %s:%d\nAssertion:\n\t%s\nWith message:\n\t%s\n", __FILE__, __LINE__,#condition, (msg) ); \
 set_fg_colour( old_colour );                                                                                                     \
 OS_Terminate();                                                                                                                  \
} } while(0)

#define PROCESS_WARNING( condition, msg ) do{ if(!(condition)) {                                                                   \
 int old_colour = get_fg_colour();                                                                                                 \
 set_fg_colour( TEXT_BROWN );                                                                                                      \
 print( "\nUser process warning at %s:%d\nAssertion:\n\t%s\nWith message:\n\t%s\n", __FILE__, __LINE__,#condition, (msg) );        \
 debug_kern_warn_break(); \
 set_fg_colour( old_colour ); \
} } while(0)

#endif //OS_DEBUG_H
