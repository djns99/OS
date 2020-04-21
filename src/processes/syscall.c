#include <utility/debug.h>
#include "syscall.h"
#include "interrupt/interrupt_handlers.h"

void syscall_handler( interrupt_params_t* params )
{
    print("SYSCALL %d\n", params->eax);
    switch( params->eax ) {
        default: {
            // Return with error
            params->eax = -1;
        }
    }
}

void init_syscall()
{
    register_handler( SYSCALL_IRQ, syscall_handler );
}

void syscall( syscall_t syscall )
{
    KERNEL_ASSERT( SYSCALL_IRQ == 0x80, "SYSCALL IRQ number did not match expected" );
    asm volatile ("mov %0, %%eax;"
        "int $0x80;"::"r" (syscall));
}