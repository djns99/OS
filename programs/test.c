#include "kernel.h"
#include "utility/print.h"

void test2()
{
    while( true ) {
        uint8_t* i_got_ptr = (uint8_t*) OS_Malloc( 1000 );
        print( "I got ptr %p 2\n", i_got_ptr );
        OS_Yield();
        OS_Free( (MEMORY) i_got_ptr );
    }
}

void test()
{
    OS_Create( test2, 1, SPORADIC, 0 );
    while( true ) {
        uint8_t* i_got_ptr = (uint8_t*) OS_Malloc( 1000 );
        print( "I got ptr %p\n", i_got_ptr );
        OS_Yield();
        OS_Free( (MEMORY) i_got_ptr );
    }
}
