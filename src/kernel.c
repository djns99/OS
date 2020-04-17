#include "print.h"
#include "kernel.h"

void OS_Init()
{
    set_fg_colour( TEXT_GREEN );
    set_bg_colour( TEXT_BLACK );
    clear_screen();

    OS_InitMemory();
}

void OS_Start()
{
    // TODO Launch shell

    // TODO Sleep forever instead of busy loop
    uint32_t j = 0;
    while( true ) {
//        print( "Sleeping" );
//        for( uint32_t i = 0; i < ( ( j >> 2 ) & 0x3F ); i++ ) {
//            print( "." );
//            for( uint32_t i = 0; i < 1 << 20u; i++ )
//                    asm("nop");
//        }
//
//        print( "\n" );
        j++;
    }
}

void OS_Abort()
{
    print( "\nKernel abort triggered. Ceasing all operation\n" );
    // Disable interrupt and halts
    // Requires hard reboot to recover
    OS_DI();
    asm("hlt");
}

extern void init_memory( size_t kernel_start, size_t kernel_num_sectors );

int entry_point( size_t kernel_start, size_t kernel_num_sectors )
{
    init_memory( kernel_start, kernel_num_sectors );
    OS_Init();
    OS_Start();
}
