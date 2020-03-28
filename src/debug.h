#ifndef OS_DEBUG_H
#define OS_DEBUG_H
#ifdef TEST_MODE
#include <assert.h>
#define KERNEL_ASSERT( condition ) assert( condition )
#else
#define KERNEL_ASSERT( condition ) //TODO
#endif
#endif //OS_DEBUG_H
