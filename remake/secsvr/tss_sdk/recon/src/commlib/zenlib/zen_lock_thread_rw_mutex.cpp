// Reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/zenlib/zen_lock_thread_rw_mutex.cpp
// Address range 0x001a1660-0x001a1f8f.
//
// Faithful to the ORIG even where it is surprising:
//   * try_lock_read() calls pthread_rwlock_trywrlock(), not tryrdlock
//     (0x1a17e0 / 0x1a17fb).  This copy/paste artefact is preserved.
//   * the ctor's second failure log says "ZEN_OS::pthread_mutex_init"
//     (0x1a18eb) although the call is pthread_rwlock_init.
//   * duration_lock_* compute now + relative via ZEN_Time_Value and then
//     dispatch virtually through the vtable slot for systime_lock_*.
#include "src/commlib/zenlib/zen_lock_thread_rw_mutex.h"
#include "src/commlib/zenlib/zen_os_adapt_rwlock.h"

#include <errno.h>
#include <sys/time.h>

// ORIG CU static-init block (audit/report/init_orig.json): __ioinit only.
#include "src/ioinit_block.h"

namespace {

const char *const ZEN_RW_MUTEX_SOURCE =
    "/data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/"
    "src/commlib/zenlib/zen_lock_thread_rw_mutex.cpp";

} // namespace

ZEN_Thread_RW_Mutex::ZEN_Thread_RW_Mutex()
    : ZEN_Lock_Base(0) {
    pthread_rwlockattr_t attr;
    int ret = ZEN_OS::pthread_rwlockattr_init(&attr);
    if (ret != 0) {
        ZEN_RECON_ERROR_RETURN(4, ZEN_RW_MUTEX_SOURCE, 47,
                               "ZEN_Thread_RW_Mutex::ZEN_Thread_RW_Mutex()",
                               "ZEN_OS::pthread_rwlockattr_init", ret);
        return;
    }
    ret = ZEN_OS::pthread_rwlock_init(&rw_lock_, &attr);
    ZEN_OS::pthread_rwlockattr_destroy(&attr);
    if (ret != 0) {
        ZEN_RECON_ERROR_RETURN(4, ZEN_RW_MUTEX_SOURCE, 58,
                               "ZEN_Thread_RW_Mutex::ZEN_Thread_RW_Mutex()",
                               "ZEN_OS::pthread_mutex_init", ret);
    }
}

ZEN_Thread_RW_Mutex::~ZEN_Thread_RW_Mutex() {
    int ret = ZEN_OS::pthread_rwlock_destroy(&rw_lock_);
    if (ret != 0) {
        ZEN_RECON_ERROR_RETURN(4, ZEN_RW_MUTEX_SOURCE, 70,
                               "virtual ZEN_Thread_RW_Mutex::~ZEN_Thread_RW_Mutex()",
                               "ZEN_OS::pthread_rwlock_destroy", ret);
    }
}

void ZEN_Thread_RW_Mutex::lock_read() {
    int ret = ZEN_OS::pthread_rwlock_rdlock(&rw_lock_);
    if (ret != 0) {
        ZEN_RECON_ERROR_RETURN(4, ZEN_RW_MUTEX_SOURCE, 83,
                               "virtual void ZEN_Thread_RW_Mutex::lock_read()",
                               "ZEN_OS::pthread_rwlock_rdlock", ret);
    }
}

bool ZEN_Thread_RW_Mutex::try_lock_read() {
    // ORIG quirk: this calls trywrlock (see 0x1a17fb).
    return ZEN_OS::pthread_rwlock_trywrlock(&rw_lock_) == 0;
}

bool ZEN_Thread_RW_Mutex::systime_lock_read(const ZEN_Time_Value &abs_time) {
    int ret = ZEN_OS::pthread_rwlock_timedrdlock(
        &rw_lock_, (const timeval *)abs_time);
    if (ret != 0) {
        ZEN_RECON_ERROR_RETURN(4, ZEN_RW_MUTEX_SOURCE, 113,
                               "virtual bool ZEN_Thread_RW_Mutex::systime_lock_read(const ZEN_Time_Value&)",
                               "ZEN_OS::pthread_rwlock_timedrdlock", ret);
        return false;
    }
    return true;
}

bool ZEN_Thread_RW_Mutex::duration_lock_read(
    const ZEN_Time_Value &relative_time) {
    timeval now_tv;
    ::gettimeofday(&now_tv, NULL);
    ZEN_Time_Value abs_time(now_tv);
    abs_time += relative_time;
    return systime_lock_read(abs_time);
}

void ZEN_Thread_RW_Mutex::lock_write() {
    int ret = ZEN_OS::pthread_rwlock_wrlock(&rw_lock_);
    if (ret != 0) {
        ZEN_RECON_ERROR_RETURN(4, ZEN_RW_MUTEX_SOURCE, 135,
                               "virtual void ZEN_Thread_RW_Mutex::lock_write()",
                               "ZEN_OS::pthread_rwlock_wrlock", ret);
    }
}

bool ZEN_Thread_RW_Mutex::try_lock_write() {
    return ZEN_OS::pthread_rwlock_trywrlock(&rw_lock_) == 0;
}

bool ZEN_Thread_RW_Mutex::systime_lock_write(const ZEN_Time_Value &abs_time) {
    int ret = ZEN_OS::pthread_rwlock_timedwrlock(
        &rw_lock_, (const timeval *)abs_time);
    if (ret != 0) {
        ZEN_RECON_ERROR_RETURN(4, ZEN_RW_MUTEX_SOURCE, 164,
                               "virtual bool ZEN_Thread_RW_Mutex::systime_lock_write(const ZEN_Time_Value&)",
                               "ZEN_OS::pthread_mutex_timedlock", ret);
        return false;
    }
    return true;
}

bool ZEN_Thread_RW_Mutex::duration_lock_write(
    const ZEN_Time_Value &relative_time) {
    timeval now_tv;
    ::gettimeofday(&now_tv, NULL);
    ZEN_Time_Value abs_time(now_tv);
    abs_time += relative_time;
    return systime_lock_write(abs_time);
}

void ZEN_Thread_RW_Mutex::unlock() {
    int ret = ZEN_OS::pthread_rwlock_unlock(&rw_lock_);
    if (ret != 0) {
        ZEN_RECON_ERROR_RETURN(4, ZEN_RW_MUTEX_SOURCE, 189,
                               "virtual void ZEN_Thread_RW_Mutex::unlock()",
                               "ZEN_OS::pthread_rwlock_unlock", ret);
    }
}

pthread_rwlock_t *ZEN_Thread_RW_Mutex::get_lock() {
    return &rw_lock_;
}
