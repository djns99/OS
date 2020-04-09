#ifndef OS_STRING_H
#define OS_STRING_H
#include "types.h"

void os_memcpy( void* dest, const void* src, size_t len );

void os_memset( void* dest, uint8_t val, size_t len );

void os_memset16( void* dest, uint16_t val, size_t len );

void os_memset32( void* dest, uint32_t val, size_t len );

void os_memset64( void* dest, uint64_t val, size_t len );

size_t os_strlen( const char* string );

void os_strcpy( char* out, const char* string );

void os_strncpy( char* out, const char* string, size_t len );

#endif //OS_STRING_H
