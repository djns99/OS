#ifndef OS_HELPERS_H
#define OS_HELPERS_H
#include "kernel.h"
#include "types.h"

static inline uint8_t os_count_bits_set( uint32_t val )
{
//    uint8_t i = 0;
//    while( val ) {
//        i += val & 1;
//        val >>= 1;
//    }
//    return i;
    return __builtin_popcount( val );
}

static inline BOOL os_is_power_two( uint32_t val )
{
    return ( val & ( val - 1 ) ) == 0;
}

static inline uint8_t os_log2( uint32_t val )
{
//    uint8_t i = 0;
//    while( val >>= 1 )
//        i++;
//    return i;
    return 31 - __builtin_clz( val );
}

static inline uint8_t os_log2_up( uint32_t val )
{
    return os_log2( val ) + !os_is_power_two( val );
}

#define CEIL_DIV( a, b ) ( ( a + b - 1 ) / b )

#endif //OS_HELPERS_H
