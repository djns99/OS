#ifndef OS_MEMOPS_H
#define OS_MEMOPS_H
#include "types.h"

void os_memcpy( void* dest, const void* src, size_t len );
bool os_memcmp( const void* buf1, const void* buf2, size_t len );

void os_memset8( void* dest, uint8_t val, size_t len );
void os_memset16( void* dest, uint16_t val, size_t len );
void os_memset32( void* dest, uint32_t val, size_t len );
void os_memset64( void* dest, uint64_t val, size_t len );

#endif //OS_MEMOPS_H
