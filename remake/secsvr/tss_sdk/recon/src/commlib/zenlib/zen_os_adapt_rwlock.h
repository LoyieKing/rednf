// ZEN_OS pthread rwlock wrappers, reconstructed from
// libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.0, -O2).
//
// Every function is a direct PLT forward to the libc pthread entry point.
// The two timeval overloads first convert through ZEN_OS::make_timespec()
// (zen_os_adapt_time.cpp) and call the timespec overload.
#ifndef TSS_RECON_ZEN_OS_ADAPT_RWLOCK_H_
#define TSS_RECON_ZEN_OS_ADAPT_RWLOCK_H_

#include <pthread.h>
#include <sys/time.h>
#include <time.h>

namespace ZEN_OS {

// zen_os_adapt_time.cpp
timespec make_timespec(const timeval &tv);

int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared);

int pthread_rwlock_init(pthread_rwlock_t *rwlock,
                        const pthread_rwlockattr_t *attr);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);

int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock,
                               const timespec *abs_time);
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock,
                               const timeval *abs_time);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock,
                               const timespec *abs_time);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock,
                               const timeval *abs_time);

} // namespace ZEN_OS

#endif // TSS_RECON_ZEN_OS_ADAPT_RWLOCK_H_
