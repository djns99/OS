#ifndef OS_TEST_SUITE_H
#define OS_TEST_SUITE_H
#include "kernel.h"
#include "utility/types.h"

#define TEST_RUNNER_SEMAPHORE (MAXSEM - 1)

typedef struct {
    char name[32];
    bool(* function)();
} test_t;

bool test_malloc_small();
bool test_malloc_large();
bool test_malloc_oom();
bool test_malloc_many();
bool test_malloc_variable();
bool test_malloc_fill();

#endif //OS_TEST_SUITE_H
