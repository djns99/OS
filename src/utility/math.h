#ifndef OS_MATH_H
#define OS_MATH_H
#include "types.h"

/**
 * Count the number of bits set in the value
 * @param val The value to count set bits
 * @return The number of set bits
 */
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

/**
 * Check if a value is a power of two
 * @param val The value to check
 * @return True if it is a power of two, false otherwise
 */
static inline bool os_is_power_two( uint32_t val )
{
    return ( val & ( val - 1 ) ) == 0;
}

/**
 * Calculate the logarithm in base 2, rounding down
 * @param val The value whos log2 to find
 * @return The logarithm
 */
static inline uint8_t os_log2( uint32_t val )
{
//    uint8_t i = 0;
//    while( val >>= 1 )
//        i++;
//    return i;
    return 31 - __builtin_clz( val );
}

/**
 * Calculate the logarithm in base 2, rounding up
 * @param val The value whos log2 to find
 * @return The logarithm
 */
static inline uint8_t os_log2_up( uint32_t val )
{
    return os_log2( val ) + !os_is_power_two( val );
}

/**
 * Divide a by b rounding up
 */
#define CEIL_DIV( a, b ) ( ( a + b - 1 ) / b )

/**
 * Calculate the minimum of a and b
 */
#define MIN( a, b ) ( (a) < (b) ) ? (a) : (b)

#endif //OS_MATH_H
