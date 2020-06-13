#ifndef OS_STRING_H
#define OS_STRING_H
#include "types.h"

size_t strlen( const char* string );

void strcpy( char* out, const char* string );

void strncpy( char* out, const char* string, size_t len );

bool strcmp( const char* out, const char* string );

#endif //OS_STRING_H
