#include "kernel.h"
#include "utility/print.h"

void test2() {
    while (true)
    {
        print( "Goodbye World! %d\n", OS_GetParam() );
        OS_Yield();
    }
}

void test() {
    OS_Create( test2, 1, SPORADIC, 0 );
    while (true) {
        print( "Hello World! %d\n", OS_GetParam() );
        OS_Yield();
    }
    
}
