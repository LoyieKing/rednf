// 还原自 gunnersvr 二进制（zen_lock_thread_mutex.cpp，GCC 4.1.0 SUSE -O2，2026-08-10）
// ZEN_Thread_Mutex（递归型）/ ZEN_Thread_NONR_Mutex（普通型）/ ZEN_Thread_Recursive_Mutex（递归型）
// 构造：pthread_mutexattr_initex(pshared=0, type=0/1, 忽略位, name) + pthread_mutex_init。
// 失败时按原版记录 "[FAIL RETRUN]Fail in file [%s|%d],function:%s,fail info:%s,return %d,last error %d."
// （__FILE__ 为原始 CI 绝对路径、行号为原文件行号）。
// NONR::systime_lock 超时（ZEN_OS 包装把 errno 110 归一化为 62）静默返回 false；
// Recursive::systime_lock 超时也记日志后返回 false。
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"

#include <errno.h>
#include <sys/time.h>

// ZEN_Trace_LogMsg::debug_output 的最小本地声明（trace 头尚未还原，
// 符号与真实类一致：_ZN16ZEN_Trace_LogMsg12debug_outputE16ZEN_LOG_PRIORITYPKcz）
enum ZEN_LOG_PRIORITY {
    RS_ERROR = 4,
};
struct ZEN_Trace_LogMsg {
    static void debug_output(ZEN_LOG_PRIORITY dbglevel, const char *str_format, ...);
};

namespace ZEN_OS {
int pthread_mutexattr_initex(pthread_mutexattr_t *mutexattr, bool is_pshared,
                             bool is_recursive, bool, const char *);
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutexattr_destroy(pthread_mutexattr_t *mutexattr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_timedlock(pthread_mutex_t *mutex, const timeval *abs_time);
timeval timeval_add(const timeval &tv1, const timeval &tv2);
} // namespace ZEN_OS

#define ZEN_LOCK_TRACE_ERROR(fail_info, ret, err, line) \
    ZEN_Trace_LogMsg::debug_output(RS_ERROR, \
        "[FAIL RETRUN]Fail in file [%s|%d],function:%s,fail info:%s,return %d,last error %d.", \
        "/data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_lock_thread_mutex.cpp", \
        line, __PRETTY_FUNCTION__, fail_info, ret, err)

pthread_mutex_t * ZEN_Thread_Mutex::get_lock() { return &lock_; }
pthread_mutex_t * ZEN_Thread_Recursive_Mutex::get_lock() { return &lock_; }

bool ZEN_Thread_NONR_Mutex::try_lock() {
    int ret = ZEN_OS::pthread_mutex_trylock(&lock_);
    return ret == 0;
}

bool ZEN_Thread_Recursive_Mutex::try_lock() {
    int ret = ZEN_OS::pthread_mutex_trylock(&lock_);
    return ret == 0;
}

bool ZEN_Thread_Mutex::try_lock() {
    int ret = ZEN_OS::pthread_mutex_trylock(&lock_);
    return ret == 0;
}

bool ZEN_Thread_Recursive_Mutex::duration_lock(const ZEN_Time_Value &relative_time) {
    timeval now;
    ::gettimeofday(&now, 0);
    timeval rel_time = (timeval)relative_time;
    timeval abs_time = ZEN_OS::timeval_add(now, rel_time);
    return systime_lock(ZEN_Time_Value(abs_time));
}

bool ZEN_Thread_NONR_Mutex::systime_lock(const ZEN_Time_Value &abs_time) {
    int ret = ZEN_OS::pthread_mutex_timedlock(&lock_, static_cast<const timeval *>(abs_time));
    if (ret == 0) {
        return true;
    }
    int err = errno;
    if (err == 62) { // ZEN_OS::pthread_mutex_timedlock 将 ETIMEDOUT(110) 归一化为 62
        return false;
    }
    ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutex_timedlock", ret, err, 348);
    return false;
}

void ZEN_Thread_NONR_Mutex::unlock() {
    int ret = ZEN_OS::pthread_mutex_unlock(&lock_);
    if (ret != 0) {
        int err = errno;
        ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutex_unlock", ret, err, 332);
    }
}

void ZEN_Thread_NONR_Mutex::lock() {
    int ret = ZEN_OS::pthread_mutex_lock(&lock_);
    if (ret != 0) {
        int err = errno;
        ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutex_lock", ret, err, 305);
    }
}

ZEN_Thread_NONR_Mutex::~ZEN_Thread_NONR_Mutex() {
    int ret = ZEN_OS::pthread_mutex_destroy(&lock_);
    if (ret != 0) {
        int err = errno;
        ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutex_destroy", ret, err, 290);
    }
}

ZEN_Thread_NONR_Mutex::ZEN_Thread_NONR_Mutex(const char *mutex_name)
    : ZEN_Lock_Base(0) {
    pthread_mutexattr_t mutexattr;
    int ret = ZEN_OS::pthread_mutexattr_initex(&mutexattr, false, false, true, mutex_name);
    if (ret != 0) {
        int err = errno;
        ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutexattr_initex", ret, err, 263);
        return;
    }
    ret = ZEN_OS::pthread_mutex_init(&lock_, &mutexattr);
    ZEN_OS::pthread_mutexattr_destroy(&mutexattr);
    if (ret != 0) {
        int err = errno;
        ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutex_init", ret, err, 275);
        return;
    }
}

bool ZEN_Thread_Recursive_Mutex::systime_lock(const ZEN_Time_Value &abs_time) {
    int ret = ZEN_OS::pthread_mutex_timedlock(&lock_, static_cast<const timeval *>(abs_time));
    if (ret == 0) {
        return true;
    }
    int err = errno;
    ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutex_timedlock", ret, err, 221);
    return false;
}

void ZEN_Thread_Recursive_Mutex::unlock() {
    int ret = ZEN_OS::pthread_mutex_unlock(&lock_);
    if (ret != 0) {
        int err = errno;
        ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutex_unlock", ret, err, 208);
    }
}

void ZEN_Thread_Recursive_Mutex::lock() {
    int ret = ZEN_OS::pthread_mutex_lock(&lock_);
    if (ret != 0) {
        int err = errno;
        ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutex_lock", ret, err, 181);
    }
}

ZEN_Thread_Recursive_Mutex::~ZEN_Thread_Recursive_Mutex() {
    int ret = ZEN_OS::pthread_mutex_destroy(&lock_);
    if (ret != 0) {
        int err = errno;
        ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutex_destroy", ret, err, 168);
    }
}

ZEN_Thread_Recursive_Mutex::ZEN_Thread_Recursive_Mutex(const char *mutex_name)
    : ZEN_Lock_Base(0) {
    pthread_mutexattr_t mutexattr;
    int ret = ZEN_OS::pthread_mutexattr_initex(&mutexattr, false, true, true, mutex_name);
    if (ret != 0) {
        int err = errno;
        ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutexattr_initex", ret, err, 143);
        return;
    }
    ret = ZEN_OS::pthread_mutex_init(&lock_, &mutexattr);
    ZEN_OS::pthread_mutexattr_destroy(&mutexattr);
    if (ret != 0) {
        int err = errno;
        ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutex_init", ret, err, 155);
        return;
    }
}

void ZEN_Thread_Mutex::unlock() {
    int ret = ZEN_OS::pthread_mutex_unlock(&lock_);
    if (ret != 0) {
        int err = errno;
        ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutex_unlock", ret, err, 113);
    }
}

void ZEN_Thread_Mutex::lock() {
    int ret = ZEN_OS::pthread_mutex_lock(&lock_);
    if (ret != 0) {
        int err = errno;
        ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutex_lock", ret, err, 86);
    }
}

ZEN_Thread_Mutex::~ZEN_Thread_Mutex() {
    int ret = ZEN_OS::pthread_mutex_destroy(&lock_);
    if (ret != 0) {
        int err = errno;
        ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutex_destroy", ret, err, 73);
    }
}

ZEN_Thread_Mutex::ZEN_Thread_Mutex(const char *mutex_name)
    : ZEN_Lock_Base(0) {
    pthread_mutexattr_t mutexattr;
    int ret = ZEN_OS::pthread_mutexattr_initex(&mutexattr, false, true, false, mutex_name);
    if (ret != 0) {
        int err = errno;
        ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutexattr_initex", ret, err, 47);
        return;
    }
    ret = ZEN_OS::pthread_mutex_init(&lock_, &mutexattr);
    ZEN_OS::pthread_mutexattr_destroy(&mutexattr);
    if (ret != 0) {
        int err = errno;
        ZEN_LOCK_TRACE_ERROR("ZEN_OS::pthread_mutex_init", ret, err, 56);
        return;
    }
}

bool ZEN_Thread_NONR_Mutex::duration_lock(const ZEN_Time_Value &relative_time) {
    timeval now;
    ::gettimeofday(&now, 0);
    ZEN_Time_Value abs_time(now);
    abs_time += relative_time;
    return systime_lock(abs_time);
}
