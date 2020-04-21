#ifndef OS_SYSCALL_H
#define OS_SYSCALL_H

typedef enum {
    NUM_SYSCALLS = 0,
} syscall_t;

void init_syscall();
void syscall( syscall_t syscall );

#endif //OS_SYSCALL_H
