// Restored header for zen_os_adapt_mutex.cpp (gunnersvr oracle).
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_MUTEX_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_MUTEX_H_H_

#include <pthread.h>
#include <sys/time.h>
#include <time.h>

namespace ZEN_OS {
    extern int pthread_mutex_unlock(pthread_mutex_t *mutex);
    extern int pthread_mutex_trylock(pthread_mutex_t *mutex);
    extern int pthread_mutex_timedlock(pthread_mutex_t *mutex, const timespec *abs_timeout_spec);
    extern int pthread_mutex_timedlock(pthread_mutex_t *mutex, const timeval *abs_timeout_val);
    extern int pthread_mutex_lock(pthread_mutex_t *mutex);
    extern int pthread_mutex_destroy(pthread_mutex_t *mutex);
    extern int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
    extern int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);
    extern int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
    extern int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared);
    extern int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);
    extern int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
    extern int pthread_mutexattr_init(pthread_mutexattr_t *attr);
    extern int pthread_mutexattr_initex(pthread_mutexattr_t *attr, bool process_share,
                                        bool recursive, bool need_timeout, const char *mutex_name);
} // namespace ZEN_OS

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_MUTEX_H_H_
