#include "rand.h"

int64_t seed;

void srand( int _seed )
{
    seed = _seed;
}

int rand()
{

    const int64_t a = 1103515245;
    const int64_t c = 12345;
    seed = ( a * seed + c ) & INT32_MAX;
    return seed;
}

void shuffle( void* buffer, uint32_t elem_size, uint32_t num_elems )
{
    uint8_t* buffer8 = (uint8_t*) buffer;
    while( --num_elems > 0 ) {
        const uint32_t swap_index = ( rand() % num_elems ) * elem_size;
        const uint32_t store_index = num_elems * elem_size;
        // Swap the chosen elements
        for( uint32_t i = 0; i < elem_size; i++ ) {
            uint8_t tmp = buffer8[ swap_index + i ];
            buffer8[ swap_index + i ] = buffer8[ store_index + i ];
            buffer8[ store_index + i ] = tmp;
        }
    }
}