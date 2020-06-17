#ifndef OS_MEMOPS_H
#define OS_MEMOPS_H
#include "types.h"

/**
 * Copy memory between two buffers
 * @param dest The destination buffer
 * @param src The source buffer
 * @param len The number of bytes to copy
 */
void memcpy( void* dest, const void* src, size_t len );

/**
 * Compare two buffers
 * @param buf1 The first buffer
 * @param buf2 The second buffer
 * @param len The number of bytes to compate
 * @return True if the buffers are identical, false otherwise
 */
bool memcmp( const void* buf1, const void* buf2, size_t len );

/**
 * Set each byte in a buffer
 * @param dest The buffer to set
 * @param val The value to set
 * @param len The number of bytes to set
 */
void memset8( void* dest, uint8_t val, size_t len );

/**
 * Set each uint16_t in a buffer
 * @param dest The buffer to set
 * @param val The value to set
 * @param len The number of bytes to set
 */
void memset16( void* dest, uint16_t val, size_t len );

/**
 * Set each uint32_t in a buffer
 * @param dest The buffer to set
 * @param val The value to set
 * @param len The number of bytes to set
 */
void memset32( void* dest, uint32_t val, size_t len );

/**
 * Set each uint64_t in a buffer
 * @param dest The buffer to set
 * @param val The value to set
 * @param len The number of bytes to set
 */
void memset64( void* dest, uint64_t val, size_t len );

#endif //OS_MEMOPS_H
