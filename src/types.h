#ifndef TYPES_H
#define TYPES_H
#ifdef TEST_MODE
#include <stdint.h>
#else
#define NULL 0

typedef int bool;
#define true 1
#define false 0

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;
#endif
#endif