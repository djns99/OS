#include "../kernel.h"
#include "../meminfo.h"
#include "../memory.h"
#include <stdio.h>
#include <stdint.h>

#define TEST_ERROR( msg, file, line ) printf( "Error: %s. At %s:%d", msg, file, line ); return -1;

#define ASSERT_EQ( a, b ) if( (a) != (b) ) { TEST_ERROR( #a  " != " #b, __FILE__, __LINE__ ) }

int main()
{
    OS_InitMemory();

    uint8_t* memory_start = get_memory_start();

    MEMORY mem = OS_Malloc( 2 );

    ASSERT_EQ( (uint8_t*)mem, memory_start + ALLOC_RESERVED_SIZE );


    printf( "Test passed\n" );
}