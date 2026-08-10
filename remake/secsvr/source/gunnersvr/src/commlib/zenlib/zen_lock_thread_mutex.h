// 还原头文件（基于 gunnersvr DWARF + 二进制 vtable 验证，2026-08-10）
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_LOCK_THREAD_MUTEX_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_LOCK_THREAD_MUTEX_H_H_

#include <pthread.h>

#include "src/commlib/zenlib/zen_lock_base.h"

// sizeof = 28
struct ZEN_Thread_NONR_Mutex : public ZEN_Lock_Base { // line 111
protected:
    pthread_mutex_t lock_;
public:
    ZEN_Thread_NONR_Mutex(const ZEN_Thread_NONR_Mutex &arg0);
    ZEN_Thread_NONR_Mutex(const char *arg0); // line 248
    virtual ~ZEN_Thread_NONR_Mutex(); // line 282
    virtual void lock(); // line 297
    virtual bool try_lock(); // line 311
    virtual void unlock(); // line 325
    virtual bool systime_lock(const ZEN_Time_Value &abs_time); // line 339
    virtual bool duration_lock(const ZEN_Time_Value &relative_time); // line 359
};
// sizeof = 28
struct ZEN_Thread_Recursive_Mutex : public ZEN_Lock_Base { // line 63
protected:
    pthread_mutex_t lock_;
public:
    ZEN_Thread_Recursive_Mutex(const ZEN_Thread_Recursive_Mutex &arg0);
    ZEN_Thread_Recursive_Mutex(const char *arg0); // line 129
    virtual ~ZEN_Thread_Recursive_Mutex(); // line 161
    virtual void lock(); // line 174
    virtual bool try_lock(); // line 187
    virtual void unlock(); // line 201
    virtual bool systime_lock(const ZEN_Time_Value &abs_time); // line 214
    virtual bool duration_lock(const ZEN_Time_Value &relative_time); // line 229
    pthread_mutex_t * get_lock(); // line 238
};
// sizeof = 28
struct ZEN_Thread_Mutex : public ZEN_Lock_Base { // line 22
protected:
    pthread_mutex_t lock_;
public:
    ZEN_Thread_Mutex(const ZEN_Thread_Mutex &arg0);
    ZEN_Thread_Mutex(const char *arg0); // line 34
    virtual ~ZEN_Thread_Mutex(); // line 66
    virtual void lock(); // line 79
    virtual bool try_lock(); // line 92
    virtual void unlock(); // line 106
    pthread_mutex_t * get_lock(); // line 119
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_LOCK_THREAD_MUTEX_H_H_
