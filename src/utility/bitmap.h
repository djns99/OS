#ifndef OS_BITMAP_H
#define OS_BITMAP_H
#include "math.h"

/**
 * Declare a statically sized bitmap with the specified number of bits
 */
#define DECLARE_BITMAP( name, bits ) DECLARE_VARIABLE_BITMAP( name, bits ) = { bits }
/**
 * Declare a max size bitmap with an variable number of bits
 */
#define DECLARE_VARIABLE_BITMAP( name, max_bits ) uint32_t name[ CEIL_DIV( max_bits, 32 ) + 1 ]

typedef uint32_t* bitmap_t;

/**
 * Initialise a variable sized bitmap to the specified number of bits
 * @param bmp The bitmap to initialise
 * @param num_bits The number of bits to initialise to
 */
void init_variable_bitmap( bitmap_t bmp, uint32_t num_bits );

/**
 * Clear all bits in the bitmap to 0
 * @param bmp Bitmap to clear
 */
void bitmap_clear_all( bitmap_t bmp );
/**
 * Set all bits in the bitmap to 1
 * @param bmp Bitmap to set
 */
void bitmap_set_all( bitmap_t bmp );

/**
 * Assign all bits in the bitmap to the provided value
 * @param bmp The bitmap to set
 * @param val The value to set to
 */
void bitmap_assign_all( bitmap_t bmp, bool val );

/**
 * Invert all the bits in the bitmap
 * @param bmp The bitmap to flip
 */
void bitmap_flip_all( bitmap_t bmp );

/**
 * Clear the bit at the specified index
 * @param bmp The bitmap to update
 * @param idx The bit to clear
 */
void bitmap_clear_bit( bitmap_t bmp, uint32_t idx );
/**
 * Set the bit at the specified index
 * @param bmp The bitmap to update
 * @param idx The bit to set
 */
void bitmap_set_bit( bitmap_t bmp, uint32_t idx );
/**
 * Assign the bit at the specified index
 * @param bmp The bitmap to update
 * @param idx The bit to assign
 * @param val The value to set it to
 */
void bitmap_assign_bit( bitmap_t bmp, uint32_t idx, bool val );
/**
 * Flip the bit at the specified index
 * @param bmp The bitmap to update
 * @param idx The bit to flip
 */
void bitmap_flip_bit( bitmap_t bmp, uint32_t idx );
/**
 * Get the bit at the specified index
 * @param bmp The bitmap to update
 * @param idx The bit to get
 * @return The value of the bit
 */
bool bitmap_get_bit( bitmap_t bmp, uint32_t idx );

/**
 * Count the number of set bits in the bitmap
 * @param bmp The bitmap to count the bits in
 * @return The number of set bits
 */
uint32_t bitmap_count_set( bitmap_t bmp );

/**
 * Find the of the first set bit in the bitmap
 * @param bmp The bitmap to find the bit in
 * @return The index of the first set bit in the bitmap
 */
uint32_t bitmap_find_first_set( bitmap_t bmp );

/**
 * Find the of the first set bit in the bitmap, starting from a specific offset
 * @param bmp The bitmap to find the bit in
 * @param start_id The bit to start searching from
 * @return The index of the first set bit after start_id
 */
uint32_t bitmap_find_first_set_from( bitmap_t bmp, uint32_t start_id );

/**
 * Get the size of the bitmap
 * @param bmp The bitmap to get the size of
 * @return The number of bits in the bitmap
 */
uint32_t get_bitmap_bits( bitmap_t bmp );

// Loop through every bit in the bitmap
#define FOREACH_BIT( bmp, loop_var ) for( uint32_t _bitmap_word = 1; _bitmap_word <= bmp[0]; _bitmap_word++ ) \
                                for( uint32_t _bitmap_bit = 0; _bitmap_bit < ((_bitmap_word < bmp[0]) ? ((bmp[0]%32) ?: 32 ) : 32 ); _bitmap_bit++ ) \
                                    if( (loop_var = (bmp[_bitmap_word] >> _bitmap_bit) & 0x1u) || true )

#endif //OS_BITMAP_H
