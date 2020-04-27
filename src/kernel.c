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
    init_timer( 1000 );
    init_keyboard();
    OS_InitMemory();

    init_fifos();
    init_processes();
    init_syscall();
//    syscall( NUM_SYSCALLS );

    init_interrupts();
}

extern void test();

void OS_Start()
{
    // TODO Launch shell
    // Launch initial process
    // We are the idle process
    OS_Create( test, 0, SPORADIC, 0 );

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
