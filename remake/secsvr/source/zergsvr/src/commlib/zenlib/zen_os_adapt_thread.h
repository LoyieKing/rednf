// Restored header for zen_os_adapt_thread.cpp (gunnersvr oracle).
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_THREAD_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_THREAD_H_H_

#include "src/commlib/zenlib/zen_os_adapt_predefine.h"

#include <pthread.h>
#include <sched.h>
#include <stddef.h>

typedef void *ZEN_THR_FUNC_RETURN;

class THREAD_START_FUN_ADAPT {
protected:
    void (*start_routine_)(void *arg0);
    void *arg_;

public:
    THREAD_START_FUN_ADAPT(void (*start_routine)(void *arg0), void *arg)
        : start_routine_(start_routine), arg_(arg) {}
    ~THREAD_START_FUN_ADAPT() {}

    static void *adapt_svc_run(void *adapt_svc) {
        THREAD_START_FUN_ADAPT *my_adapt = (THREAD_START_FUN_ADAPT *)adapt_svc;
        my_adapt->start_routine_(my_adapt->arg_);
        delete my_adapt;
        // Original binary returns NULL here (xor eax,eax); quirk preserved.
        return NULL;
    }
};

namespace ZEN_OS {
    extern void *pthread_getspecific(pthread_key_t key);
    extern int pthread_setspecific(pthread_key_t key, const void *data);
    extern int pthread_key_delete(pthread_key_t key);
    extern int pthread_key_create(pthread_key_t *key, void (*destructor)(void *arg0));
    extern int pthread_detach(ZEN_THREAD_ID threadid);
    extern int pthread_cancel(ZEN_THREAD_ID threadid);
    extern ZEN_THREAD_ID pthread_self();
    extern int pthread_join(ZEN_THREAD_ID threadid);
    extern void pthread_exit(ZEN_THR_FUNC_RETURN thr_ret);
    extern void pthread_exit();
    extern int pthread_create(ZEN_THREAD_ID *threadid, const pthread_attr_t *attr,
                              ZEN_THR_FUNC_RETURN (*start_routine)(void *arg0), void *arg);
    extern int pthread_create(ZEN_THREAD_ID *threadid, const pthread_attr_t *attr,
                              void (*start_routine)(void *arg0), void *arg);
    extern int pthread_attr_setex(pthread_attr_t *attr, int detachstate, size_t stacksize,
                                  int threadpriority);
    extern int pthread_attr_getex(const pthread_attr_t *attr, int *detachstate,
                                  size_t *stacksize, int *threadpriority);
    extern int pthread_attr_destroy(pthread_attr_t *attr);
    extern int pthread_attr_init(pthread_attr_t *attr);
    extern int pthread_createex(void (*start_routine)(void *arg0), void *arg,
                                ZEN_THREAD_ID *threadid, int detachstate, size_t stacksize,
                                int threadpriority);
} // namespace ZEN_OS

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_THREAD_H_H_
