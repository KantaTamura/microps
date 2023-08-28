#ifndef MICROPS_PLATFORM_H
#define MICROPS_PLATFORM_H

#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>

// memory

static inline void *memory_alloc(size_t size) {
    return calloc(1, size);
}

static inline void memory_free(void *ptr) {
    free(ptr);
}

// mutex

typedef pthread_mutex_t mutex_t;

#define MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER

static inline int mutex_init(mutex_t *mutex) {
    return pthread_mutex_init(mutex, NULL);
}

static inline int mutex_lock(mutex_t *mutex) {
    return pthread_mutex_lock(mutex);
}

static inline int mutex_unlock(mutex_t *mutex) {
    return pthread_mutex_unlock(mutex);
}

#endif //MICROPS_PLATFORM_H
