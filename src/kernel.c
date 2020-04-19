#include "interrupt_handling/interrupt_handlers.h"
#include "utility/print.h"
#include "kernel.h"
#include "peripherals/timer.h"
#include "utility/debug.h"
#include "peripherals/keyboard.h"

void OS_Init()
{
    set_fg_colour( TEXT_GREEN );
    set_bg_colour( TEXT_BLACK );
//    clear_screen();

    init_idt();
    init_timer( 1000 );
    init_keyboard();
    OS_InitMemory();

    OS_EI();
}

void OS_Start()
{
    // TODO Launch shell

    // TODO Sleep until shutdown
    while( true )
            asm("hlt");
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

int entry_point()
{
    OS_Init();
    OS_Start();
}
