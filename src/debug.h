#ifndef OS_DEBUG_H
#define OS_DEBUG_H
#ifdef TEST_MODE
#include <assert.h>
#define KERNEL_ASSERT( condition ) assert( condition )
#else
#define KERNEL_ASSERT( condition ) ((void)0)
#endif
#endif //OS_DEBUG_H
