// Reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/zenlib/zen_os_adapt_rwlock.cpp
//
// All thirteen wrappers are plain forwards to the libc pthread_rwlock API
// (verified against ORIG 0x001a39e0-0x001a3cb2); the timeval overloads call
// ZEN_OS::make_timespec() first, exactly like the original PLT sequence.
#include "src/commlib/zenlib/zen_os_adapt_rwlock.h"

// ORIG CU static-init block (audit/report/init_orig.json): __ioinit only.
#include "src/ioinit_block.h"

namespace ZEN_OS {

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock) {
    return ::pthread_rwlock_unlock(rwlock);
}

int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock,
                               const timespec *abs_time) {
    return ::pthread_rwlock_timedwrlock(rwlock, abs_time);
}

int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock,
                               const timeval *abs_time) {
    timespec ts = ZEN_OS::make_timespec(*abs_time);
    return ZEN_OS::pthread_rwlock_timedwrlock(rwlock, &ts);
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) {
    return ::pthread_rwlock_wrlock(rwlock);
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock) {
    return ::pthread_rwlock_trywrlock(rwlock);
}

int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock,
                               const timespec *abs_time) {
    return ::pthread_rwlock_timedrdlock(rwlock, abs_time);
}

int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock,
                               const timeval *abs_time) {
    timespec ts = ZEN_OS::make_timespec(*abs_time);
    return ZEN_OS::pthread_rwlock_timedrdlock(rwlock, &ts);
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock) {
    return ::pthread_rwlock_tryrdlock(rwlock);
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) {
    return ::pthread_rwlock_rdlock(rwlock);
}

int pthread_rwlock_destroy(pthread_rwlock_t *rwlock) {
    return ::pthread_rwlock_destroy(rwlock);
}

int pthread_rwlock_init(pthread_rwlock_t *rwlock,
                        const pthread_rwlockattr_t *attr) {
    return ::pthread_rwlock_init(rwlock, attr);
}

int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared) {
    return ::pthread_rwlockattr_setpshared(attr, pshared);
}

int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr) {
    return ::pthread_rwlockattr_destroy(attr);
}

int pthread_rwlockattr_init(pthread_rwlockattr_t *attr) {
    return ::pthread_rwlockattr_init(attr);
}

} // namespace ZEN_OS
