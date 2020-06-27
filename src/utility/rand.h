#ifndef OS_RAND_H
#define OS_RAND_H
#include "types.h"

/**
 * Sets the seed for the random generator
 * @param seed The seed to set
 */
void srand( int seed );

/**
 * Generates a random positive number in the range 0-INT32_MAX
 * @return A random number
 */
int rand();

/**
 * Randomly shuffle the specified buffer
 * @param buffer The buffer to shuffle
 * @param elem_size The size of each element in the buffer
 * @param num_elems The number of elements in the buffer to shuffle
 */
void shuffle( void* buffer, uint32_t elem_size, uint32_t num_elems );

#endif //OS_RAND_H
