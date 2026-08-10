// Restored from gunnersvr binary (DWARF + disassembly), GCC 4.1.0 SUSE, -O2.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_os_adapt_mutex.cpp
// Note: original quirks preserved (see below).

#include "src/commlib/zenlib/zen_os_adapt_mutex.h"

#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

namespace ZEN_OS {

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
    return ::pthread_mutex_unlock(mutex);
}

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
    return ::pthread_mutex_trylock(mutex);
}

int pthread_mutex_timedlock(pthread_mutex_t *mutex, const timespec *abs_timeout_spec) {
    int result = ::pthread_mutex_timedlock(mutex, abs_timeout_spec);
    if (result != 0 && errno == EDEADLK) {
        // Original quirk: translates errno EDEADLK -> ETIMEDOUT but returns
        // the unchanged pthread result code.
        errno = ETIMEDOUT;
    }
    return result;
}

int pthread_mutex_timedlock(pthread_mutex_t *mutex, const timeval *abs_timeout_val) {
    timespec abs_timeout_spec;
    abs_timeout_spec.tv_sec = abs_timeout_val->tv_sec;
    abs_timeout_spec.tv_nsec = abs_timeout_val->tv_usec * 1000;
    return ZEN_OS::pthread_mutex_timedlock(mutex, &abs_timeout_spec);
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
    return ::pthread_mutex_lock(mutex);
}

int pthread_mutex_destroy(pthread_mutex_t *mutex) {
    return ::pthread_mutex_destroy(mutex);
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
    return ::pthread_mutex_init(mutex, attr);
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type) {
    return ::pthread_mutexattr_gettype(attr, type);
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
    return ::pthread_mutexattr_settype(attr, type);
}

int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared) {
    return ::pthread_mutexattr_getpshared(attr, pshared);
}

int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared) {
    return ::pthread_mutexattr_setpshared(attr, pshared);
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
    // Original binary quirk (verified at 0x080a3030): this wrapper calls
    // pthread_mutexattr_INIT instead of pthread_mutexattr_destroy.
    return ::pthread_mutexattr_init(attr);
}

int pthread_mutexattr_init(pthread_mutexattr_t *attr) {
    return ::pthread_mutexattr_init(attr);
}

int pthread_mutexattr_initex(pthread_mutexattr_t *attr, bool process_share,
                             bool recursive, bool need_timeout, const char *mutex_name) {
    // need_timeout and mutex_name are unused by the original.
    int result = ::pthread_mutexattr_init(attr);
    if (result == 0) {
        result = ::pthread_mutexattr_setpshared(
            attr, process_share ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE);
        if (result == 0) {
            result = ::pthread_mutexattr_settype(
                attr, recursive ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_NORMAL);
        }
    }
    if (result != 0) {
        ZEN_OS::pthread_mutexattr_destroy(attr);
        // Original quirk: any failure returns -1, not the error code.
        return -1;
    }
    return 0;
}

} // namespace ZEN_OS
