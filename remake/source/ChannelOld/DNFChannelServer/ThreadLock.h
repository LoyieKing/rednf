#ifndef THREADLOCK_H_
#define THREADLOCK_H_

#include "stdafx.h"

class ThreadLock_linux
{
public:
    ThreadLock_linux();
    ~ThreadLock_linux();
    void lock()
    {
        pthread_mutex_lock(&handle_);
    }
    void unlock()
    {
        pthread_mutex_unlock(&handle_);
    }
    void _lock();

private:
    pthread_mutex_t handle_;
};

template <class TThreadLock_>
class TThreadLock
{
public:
    TThreadLock()
    {
    }
    ~TThreadLock()
    {
    }
    void lock()
    {
        impl_.lock();
    }
    void unlock()
    {
        impl_.unlock();
    }

private:
    TThreadLock_ impl_;
};

template <class TThreadLock_>
class TScopedLock
{
public:
    TScopedLock(TThreadLock_& in_lock_)
    {
        lock_ = &in_lock_;
        lock_->lock();
    }
    ~TScopedLock()
    {
        lock_->unlock();
    }

private:
    TThreadLock_* lock_;
};

typedef TThreadLock<ThreadLock_linux> ThreadLock;

#endif // THREADLOCK_H_
