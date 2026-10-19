// Tag-394 model of src/commlib/zenlib/zen_lock_thread_mutex.h.
//
// ORIG DWARF (libtss_sdk.so.1.0.394, CU zen_lock_thread_mutex.cpp):
//   ZEN_Thread_Mutex  size 28 = ZEN_Lock_Base (vptr @0) + pthread_mutex_t
//                     `lock_` @0x04 (24 bytes)
//   virtual order:    ~ZEN_Thread_Mutex, lock, try_lock, unlock   (4 virtuals;
//                     `get_lock` is not virtual)
//   ORIG symbol:      ZEN_Thread_Mutex::ZEN_Thread_Mutex(char const*)
//
// The secagent copy of this header models the *gunnersvr* variant
// (ZEN_Thread_Mutex()/ZEN_Thread_NONR_Mutex/ZEN_Thread_Recursive_Mutex, all
// with a no-argument constructor), which does not match tag-394; this header
// is the 394 model and takes precedence for recon translation units.
//
// `lock_` stays protected exactly as in the ORIG header, so the layout
// contract below checks the size only (offsetof() cannot see it from here).
#ifndef TSS_RECON_ZEN_LOCK_THREAD_MUTEX_H_
#define TSS_RECON_ZEN_LOCK_THREAD_MUTEX_H_

#include <pthread.h>

#include "src/commlib/zenlib/zen_lock_base.h"

class ZEN_Thread_Mutex : public ZEN_Lock_Base {
public:
    // ORIG emits only ZEN_Thread_Mutex(char const*); IDPHackerSafe has no
    // constructor symbol while its inlined construction passes 0 (see
    // DPPkgDelayHacker::DPPkgDelayHacker at 0x000b5c4d), i.e. the parameter
    // carries a default value.
    ZEN_Thread_Mutex(const char *name = 0);
    virtual ~ZEN_Thread_Mutex();

    virtual void lock();
    virtual bool try_lock();
    virtual void unlock();

    pthread_mutex_t *get_lock();

protected:
    pthread_mutex_t lock_;   // +0x04 (24 bytes)
};

typedef char tss_recon_thread_mutex_size[
    sizeof(ZEN_Thread_Mutex) == 28 ? 1 : -1];

#endif // TSS_RECON_ZEN_LOCK_THREAD_MUTEX_H_
