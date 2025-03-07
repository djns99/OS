#ifndef TYPES_H
#define TYPES_H

#define NULL 0

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef uint32_t size_t;
typedef size_t uintptr_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

typedef uint32_t ssize_t;

typedef uint32_t bool;
#define true 1
#define false 0

#define INT32_MAX 0x7FFFFFFF
#define INT32_MIN -0x80000000
#define UINT32_MAX 0xFFFFFFFFu
#define UINT32_MIN 0x0u
#endif