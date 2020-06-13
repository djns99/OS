#include "utility/debug.h"
#include "utility/memops.h"
#include "syscall.h"
#include "interrupt/interrupt_handlers.h"

syscall_func_t syscalls[SYSCALL_NUM];

void syscall_handler( interrupt_params_t* params )
{
    if( syscalls[ params->eax ] )
        params->eax = syscalls[ params->eax ]( params->ebx, params->ecx );
    else
        params->eax = SYS_UNHANDLED;
}

void register_syscall_handler( uint32_t syscall_id, syscall_func_t func )
{
    KERNEL_ASSERT( syscall_id < SYSCALL_NUM, "Invalid syscall registered" );
    syscalls[ syscall_id ] = func;
}

void init_syscall()
{
    KERNEL_ASSERT( SYSCALL_IRQ == 0x80, "SYSCALL IRQ number did not match expected" );
    register_handler( SYSCALL_IRQ, syscall_handler );
    os_memset8( syscalls, 0x0, sizeof( syscalls ) );
}

int syscall( syscall_t syscall, uint32_t param1, uint32_t param2 )
{
    if( syscall > SYSCALL_NUM )
        return SYS_INVALID;

    int res;
    asm volatile ("mov %1, %%eax;"
                  "mov %2, %%ebx;"
                  "mov %3, %%ecx;"
                  "int $0x80;"
                  "mov %%eax, %0": "=r" (res) :"r" (syscall), "r" (param1), "r" (param2) : "memory");
    return res;
}