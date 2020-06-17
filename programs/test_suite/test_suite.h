#ifndef OS_TEST_SUITE_H
#define OS_TEST_SUITE_H
#include "kernel.h"
#include "utility/types.h"

#define MAX_TEST_PROCESSES 10
#define TEST_RUNNER_SEMAPHORE (MAXSEM - 1)

typedef struct {
    char name[64];
    bool (* function)();
} test_t;

bool test_malloc_small();
bool test_malloc_underflow_regression();
bool test_malloc_large();
bool test_malloc_oom();
bool test_malloc_many();
bool test_malloc_variable();
bool test_malloc_fill();

bool test_semaphore_mutex();
bool test_semaphore_n_blocked();
bool test_semaphore_compliance();

bool test_spsc_fifo();
bool test_mpsc_fifo();
bool test_mpmc_fifo();

bool test_process_scheduling();

#endif //OS_TEST_SUITE_H
