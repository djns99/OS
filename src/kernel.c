#include "interrupt_handling/interrupt_handlers.h"
#include "print.h"
#include "kernel.h"
#include "timer.h"
#include "debug.h"

void OS_Init()
{
    set_fg_colour( TEXT_GREEN );
    set_bg_colour( TEXT_BLACK );
    clear_screen();
    
    init_idt();
    init_timer( 1 );
    OS_InitMemory();
    
    OS_EI();
}

void OS_Start()
{
    // TODO Launch shell

    // TODO Sleep until shutdown
    while (true);
}

void OS_Abort()
{
    print( "\nKernel abort triggered. Ceasing all operation\n" );
    // TODO Actually power down
    // Disable interrupt and halts
    // Requires hard reboot to recover
    OS_DI();
    asm("hlt");
}

extern void init_memory( void* kernel_end );

int entry_point( void* kernel_end )
{
    init_memory( kernel_end );
    OS_Init();
    OS_Start();
}
