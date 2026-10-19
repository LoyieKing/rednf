// ZEN_Thread_RW_Mutex, reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/zenlib/zen_lock_thread_rw_mutex.cpp
//
// ORIG DWARF: sizeof(ZEN_Thread_RW_Mutex) == 36
//   +0x00 vptr (ZEN_Lock_Base)
//   +0x04 pthread_rwlock_t rw_lock_  (32 bytes)
// ORIG vtable (address point 0x293cc8) order:
//   D1,D0,UNLOCK,lock_read,try_lock_read,systime_lock_read,
//   duration_lock_read,lock_write,try_lock_write,systime_lock_write,
//   duration_lock_write ... see zen_recon_support.h for ZEN_Lock_Base slots.
#ifndef TSS_RECON_ZEN_LOCK_THREAD_RW_MUTEX_H_
#define TSS_RECON_ZEN_LOCK_THREAD_RW_MUTEX_H_

#include <pthread.h>
#include "src/commlib/zenlib/zen_recon_support.h"
#include "src/commlib/zenlib/zen_os_adapt_rwlock.h"

class ZEN_Thread_RW_Mutex : public ZEN_Lock_Base {
protected:
    pthread_rwlock_t rw_lock_;

public:
    ZEN_Thread_RW_Mutex(const ZEN_Thread_RW_Mutex &arg0);
    ZEN_Thread_RW_Mutex();
    virtual ~ZEN_Thread_RW_Mutex();

    virtual void lock_read();
    virtual bool try_lock_read();
    virtual bool systime_lock_read(const ZEN_Time_Value &abs_time);
    virtual bool duration_lock_read(const ZEN_Time_Value &relative_time);
    virtual void lock_write();
    virtual bool try_lock_write();
    virtual bool systime_lock_write(const ZEN_Time_Value &abs_time);
    virtual bool duration_lock_write(const ZEN_Time_Value &relative_time);
    virtual void unlock();

    pthread_rwlock_t *get_lock();

private:
    ZEN_Thread_RW_Mutex &operator=(const ZEN_Thread_RW_Mutex &);
};

#endif // TSS_RECON_ZEN_LOCK_THREAD_RW_MUTEX_H_
