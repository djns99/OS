#include "bitmap.h"
#include "debug.h"

#define FOREACH_WORD( bmp, loop_var ) for( uint32_t* loop_var = bmp + 1; loop_var != bmp + CEIL_DIV( bmp[ 0 ], 32 ) + 1; loop_var++ )

void init_variable_bitmap( bitmap_t bmp, uint32_t num_bits )
{
    bmp[ 0 ] = num_bits;
}

void bitmap_clear_all( bitmap_t bmp )
{
    bitmap_assign_all( bmp, false );
}
void bitmap_set_all( bitmap_t bmp )
{
    bitmap_assign_all( bmp, true );
}

void bitmap_assign_all( bitmap_t bmp, bool val )
{
    FOREACH_WORD( bmp, word )
    {
        *word = val ? 0xFFFFFFFF : 0x0; 
    }
}

void bitmap_flip_all( bitmap_t bmp )
{
    FOREACH_WORD( bmp, word )
    {
        *word ^= 0xFFFFFFFF;
    }
}

void bitmap_clear_bit( bitmap_t bmp, uint32_t idx )
{
    bitmap_assign_bit( bmp, idx, false ); 
}

void bitmap_set_bit( bitmap_t bmp, uint32_t idx )
{
    bitmap_assign_bit( bmp, idx, true );
}
void bitmap_assign_bit( bitmap_t bmp, uint32_t idx, bool val )
{
    KERNEL_ASSERT( idx < bmp[ 0 ], "Index out of range" );
    if( val )
        bmp[ idx / 32 + 1 ] |= (1u<<(idx%32u));
    else
        bmp[ idx / 32 + 1 ] &= ~(1u<<(idx%32u));
}

void bitmap_flip_bit( bitmap_t bmp, uint32_t idx )
{
    KERNEL_ASSERT( idx < bmp[ 0 ], "Index out of range" );
    bmp[ idx / 32 + 1 ] ^= (1u<<(idx%32u));
}

uint32_t bitmap_count_set( bitmap_t bmp )
{
    uint32_t count = 0;
    FOREACH_WORD( bmp, word ) {
        count += __builtin_popcount( *word );
    }
    return count;
}

uint32_t bitmap_find_first_set( bitmap_t bmp )
{
    uint32_t idx = 0;
    FOREACH_WORD( bmp, word ) {
        if( *word )
            return idx + __builtin_ctz( *word );
        idx += 32;
    }
    return bmp[ 0 ];
}

uint32_t get_bitmap_bits( bitmap_t bmp )
{
    return bmp[ 0 ];
}