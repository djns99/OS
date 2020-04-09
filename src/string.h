#ifndef OS_STRING_H
#define OS_STRING_H
#include "types.h"

void os_memcpy( void* dest, const void* src, uint32_t len );
void os_memset( void* dest, uint8_t val, uint32_t len );

uint32_t os_strlen( const char* string );
void os_strcpy( char* out, const char* string );
void os_strncpy( char* out, const char* string, uint32_t len );

#endif //OS_STRING_H
