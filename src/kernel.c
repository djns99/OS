#include "utility/debug.h"
#include "memory/memory.h"
#include "sync/semaphore.h"
#include "sync/fifo.h"
#include "processes/syscall.h"
#include "interrupt/interrupt_handlers.h"
#include "interrupt/interrupt.h"
#include "utility/print.h"
#include "kernel.h"
#include "peripherals/timer.h"
#include "peripherals/keyboard.h"
#include "processes/process.h"

void OS_Init()
{
    register_entry_proc();

    set_fg_colour( TEXT_GREEN );
    set_bg_colour( TEXT_BLACK );
    clear_screen();

    init_idt();
    init_syscall();

    OS_InitMemory();
    bool res = init_kernel_memory();
    KERNEL_ASSERT( res, "Failed to init kernel memory" );

    init_timer( 10000 );
    init_keyboard();

    init_fifos();
    init_semaphores();
    init_processes();

    init_interrupts();
}

extern void shell();

void OS_Start()
{
    // TODO Launch shell
    // Launch initial process
    // We are the idle process
    if( OS_Create( shell, 0, SPORADIC, 0 ) == INVALIDPID )
        print( "Failed to start initial program\nSystem will now hang.\n" );

    // Kick off interrupts now
    print( "Enabling interrupts\n" );
    enable_interrupts();

    // Idle loop
    while( true ) {
        // Yield to any new processes
        OS_Yield();
        // Wait until an interrupt comes in and then pass control to new process
        asm("hlt");
    }
}

void OS_Abort()
{
    print( "\nKernel abort triggered. Ceasing all operation\n" );
    // TODO Actually power down?
    // Disable interrupt and halts
    // Requires hard reboot to recover
    disable_interrupts();
    while( true )
            asm("hlt");
}

void entry_point()
{
    OS_Init();
    OS_Start();
}
