#ifndef OS_SYSCALL_H
#define OS_SYSCALL_H
#include "utility/types.h"

#define SYS_SUCCESS 0
#define SYS_FAILED -1
#define SYS_INVALID -2
#define SYS_UNHANDLED -3
#define SYS_INVLARG -4

typedef enum {
    SYSCALL_PROCESS_YIELD = 0,
    SYSCALL_PROCESS_TERMINATE,
    SYSCALL_PROCESS_CREATE,
    SYSCALL_PROCESS_GET_PARAM,
    SYSCALL_SEMAPHORE_INIT,
    SYSCALL_SEMAPHORE_WAIT,
    SYSCALL_SEMAPHORE_SIGNAL,
    SYSCALL_SEMAPHORE_COMPLIANCE,
    SYSCALL_FIFO_INIT,
    SYSCALL_FIFO_WRITE,
    SYSCALL_FIFO_READ,
    SYSCALL_MALLOC,
    SYSCALL_FREE,
    SYSCALL_READ_LINE,
    SYSCALL_NUM,
} syscall_t;

void init_syscall();
int syscall( syscall_t syscall, uint32_t param1, uint32_t param2 );

typedef int(*syscall_func_t)( uint32_t, uint32_t );

void register_syscall_handler( uint32_t syscall_id, syscall_func_t func );

#endif //OS_SYSCALL_H
