#ifndef OS_STRING_H
#define OS_STRING_H
#include "types.h"

size_t os_strlen( const char* string );

void os_strcpy( char* out, const char* string );

void os_strncpy( char* out, const char* string, size_t len );

#endif //OS_STRING_H
