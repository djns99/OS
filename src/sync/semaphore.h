#ifndef OS_SEMAPHORE_H
#define OS_SEMAPHORE_H
#include "utility/types.h"

/**
 * Init the Semaphore subsystem
 */
void init_semaphores();

/**
 * Enum with the semaphore compliance modes
 *
 * STRICT:  Follows the strict requirements where the semaphore can only be signalled by a thread that previously waited
 * RELAXED: Allows any thread to signal/wait at any point
 */
typedef enum {
    STRICT = 0, RELAXED,
} semaphore_compliance_mode_t;

/**
 * Set the semaphore compliance mode
 * @param set The compliance mode to set to
 */
void set_sem_compliance( semaphore_compliance_mode_t set );
/**
 * Get the current semaphore compliance mode
 * @return The current semaphore compliance mode
 */
semaphore_compliance_mode_t get_sem_compliance();

#endif //OS_SEMAPHORE_H
