#ifndef OS_BITMAP_H
#define OS_BITMAP_H
#include "math.h"

#define DECLARE_BITMAP( name, bits ) DECLARE_VARIABLE_BITMAP( name, bits ) = { bits }
#define DECLARE_VARIABLE_BITMAP( name, max_bits ) uint32_t name[ CEIL_DIV( max_bits, 32 ) + 1 ]
typedef uint32_t* bitmap_t;

void init_variable_bitmap( bitmap_t bmp, uint32_t num_bits );

void bitmap_clear_all( bitmap_t bmp );
void bitmap_set_all( bitmap_t bmp );
void bitmap_assign_all( bitmap_t bmp, bool val );
void bitmap_flip_all( bitmap_t bmp );

void bitmap_clear_bit( bitmap_t bmp, uint32_t idx );
void bitmap_set_bit( bitmap_t bmp, uint32_t idx );
void bitmap_assign_bit( bitmap_t bmp, uint32_t idx, bool val );
void bitmap_flip_bit( bitmap_t bmp, uint32_t idx );

uint32_t bitmap_count_set( bitmap_t bmp );
uint32_t bitmap_find_first_set( bitmap_t bmp );

uint32_t get_bitmap_bits( bitmap_t bmp );

#define FOREACH_BIT( bmp, loop_var ) for( uint32_t _bitmap_word = 1; _bitmap_word <= bmp[0]; _bitmap_word++ ) \
                                for( uint32_t _bitmap_bit = 0; _bitmap_bit < ((_bitmap_word < bmp[0]) ? ((bmp[0]%32) ?: 32 ) : 32 ); _bitmap_bit++ ) \
                                    if( (loop_var = (bmp[_bitmap_word] >> _bitmap_bit) & 0x1u) || true )

#endif //OS_BITMAP_H
