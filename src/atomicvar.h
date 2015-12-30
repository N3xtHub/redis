/* This file implements atomic counters using __atomic or __sync macros if
 * available, otherwise synchronizing different threads using a mutex.
 *
 * The exported interaface is composed of three macros:
 *
 * atomicIncr(var,count,mutex) -- Increment the atomic counter
 * atomicDecr(var,count,mutex) -- Decrement the atomic counter
 * atomicGet(var,dstvar,mutex) -- Fetch the atomic counter value
 *
 * If atomic primitives are availble (tested in config.h) the mutex
 * is not used.
 *
 * Never use return value from the macros. To update and get use instead:
 *
 *  atomicIncr(mycounter,...);
 *  atomicGet(mycounter,newvalue);
 *  doSomethingWith(newvalue);
 *
 */

#include <pthread.h>

#ifndef __ATOMIC_VAR_H
#define __ATOMIC_VAR_H

#if defined(__ATOMIC_RELAXED)
/* Implementation using __atomic macros. */

#define atomicIncr(var,count,mutex) __atomic_add_fetch(&var,(count),__ATOMIC_RELAXED)
#define atomicDecr(var,count,mutex) __atomic_sub_fetch(&var,(count),__ATOMIC_RELAXED)
#define atomicGet(var,dstvar,mutex) do { \
    dstvar = __atomic_load_n(&var,__ATOMIC_RELAXED); \
} while(0)

#elif defined(HAVE_ATOMIC)
/* Implementation using __sync macros. */

#define atomicIncr(var,count,mutex) __sync_add_and_fetch(&var,(count))
#define atomicDecr(var,count,mutex) __sync_sub_and_fetch(&var,(count))
#define atomicGet(var,dstvar,mutex) do { \
    dstvar = __sync_sub_and_fetch(&var,0); \
} while(0)

#else
/* Implementation using pthread mutex. */

#define atomicIncr(var,count,mutex) do { \
    pthread_mutex_lock(&mutex); \
    var += (count); \
    pthread_mutex_unlock(&mutex); \
} while(0)

#define atomicDecr(var,count,mutex) do { \
    pthread_mutex_lock(&mutex); \
    var -= (count); \
    pthread_mutex_unlock(&mutex); \
} while(0)

#define atomicGet(var,dstvar,mutex) do { \
    pthread_mutex_lock(&mutex); \
    dstvar = var; \
    pthread_mutex_unlock(&mutex); \
} while(0)
#endif

#endif /* __ATOMIC_VAR_H */
