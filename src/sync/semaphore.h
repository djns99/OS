#ifndef OS_SEMAPHORE_H
#define OS_SEMAPHORE_H
#include "utility/types.h"

void init_semaphores();

typedef enum {
    STRICT = 0,
    RELAXED,
} semaphore_compliance_mode_t;

void set_sem_compliance( semaphore_compliance_mode_t set );
semaphore_compliance_mode_t get_sem_compliance();

#endif //OS_SEMAPHORE_H
