// Restored from secagent binary (DWARF + disassembly), GCC 4.1.0 SUSE, -O2.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/commlib/zenlib/zen_os_adapt_thread.cpp
// Original quirks preserved: adapt_svc_run returns NULL; pthread_createex does
// not destroy the attr on the success path; pthread_join(threadid) forwards to
// the new pthread_join(threadid, void**) overload.

#include "src/commlib/zenlib/zen_os_adapt_thread.h"

#include <pthread.h>
#include <sched.h>

namespace ZEN_OS {

void *pthread_getspecific(pthread_key_t key) {
    return ::pthread_getspecific(key);
}

int pthread_setspecific(pthread_key_t key, const void *data) {
    return ::pthread_setspecific(key, data);
}

int pthread_key_delete(pthread_key_t key) {
    return ::pthread_key_delete(key);
}

int pthread_key_create(pthread_key_t *key, void (*destructor)(void *arg0)) {
    return ::pthread_key_create(key, destructor);
}

int pthread_detach(ZEN_THREAD_ID threadid) {
    return ::pthread_detach(threadid);
}

int pthread_cancel(ZEN_THREAD_ID threadid) {
    return ::pthread_cancel(threadid);
}

ZEN_THREAD_ID pthread_self() {
    return ::pthread_self();
}

int pthread_join(ZEN_THREAD_ID threadid, ZEN_THR_FUNC_RETURN *ret_val) {
    return ::pthread_join(threadid, ret_val);
}

int pthread_join(ZEN_THREAD_ID threadid) {
    ZEN_THR_FUNC_RETURN ret_val;
    return pthread_join(threadid, &ret_val);
}

void pthread_exit(ZEN_THR_FUNC_RETURN thr_ret) {
    ::pthread_exit(thr_ret);
}

void pthread_exit() {
    ::pthread_exit(NULL);
}

int pthread_create(ZEN_THREAD_ID *threadid, const pthread_attr_t *attr,
                   ZEN_THR_FUNC_RETURN (*start_routine)(void *arg0), void *arg) {
    return ::pthread_create(threadid, attr, start_routine, arg);
}

int pthread_create(ZEN_THREAD_ID *threadid, const pthread_attr_t *attr,
                   void (*start_routine)(void *arg0), void *arg) {
    THREAD_START_FUN_ADAPT *adapt_object =
        new THREAD_START_FUN_ADAPT(start_routine, arg);
    return ::pthread_create(threadid, attr, THREAD_START_FUN_ADAPT::adapt_svc_run,
                            adapt_object);
}

int pthread_attr_setex(pthread_attr_t *attr, int detachstate, size_t stacksize,
                       int threadpriority) {
    int ret = pthread_attr_setdetachstate(attr, detachstate);
    if (ret != 0) {
        return ret;
    }
    ret = pthread_attr_setinheritsched(attr, PTHREAD_EXPLICIT_SCHED);
    if (ret != 0) {
        return ret;
    }
    sched_param param;
    param.sched_priority = threadpriority;
    ret = pthread_attr_setschedparam(attr, &param);
    if (ret != 0) {
        return ret;
    }
    if (stacksize <= 0x3fff) {
        stacksize = 0x4000;
    }
    return pthread_attr_setstacksize(attr, stacksize);
}

int pthread_attr_getex(const pthread_attr_t *attr, int *detachstate,
                       size_t *stacksize, int *threadpriority) {
    int ret = pthread_attr_getdetachstate(attr, detachstate);
    if (ret != 0) {
        return ret;
    }
    sched_param param;
    ret = pthread_attr_getschedparam(attr, &param);
    if (ret != 0) {
        return ret;
    }
    *threadpriority = param.sched_priority;
    return pthread_attr_getstacksize(attr, stacksize);
}

int pthread_attr_destroy(pthread_attr_t *attr) {
    return ::pthread_attr_destroy(attr);
}

int pthread_attr_init(pthread_attr_t *attr) {
    return ::pthread_attr_init(attr);
}

int pthread_createex(void (*start_routine)(void *arg0), void *arg,
                     ZEN_THREAD_ID *threadid, int detachstate, size_t stacksize,
                     int threadpriority) {
    pthread_attr_t attr;
    int ret = ::pthread_attr_init(&attr);
    if (ret == 0) {
        ret = pthread_attr_setex(&attr, detachstate, stacksize, threadpriority);
        if (ret == 0) {
            ret = pthread_create(threadid, &attr, start_routine, arg);
        }
        if (ret != 0) {
            ::pthread_attr_destroy(&attr);
        }
    }
    return ret;
}

} // namespace ZEN_OS
