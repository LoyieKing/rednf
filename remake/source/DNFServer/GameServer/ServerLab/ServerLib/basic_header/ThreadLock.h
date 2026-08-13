#ifndef NSL_THREADLOCK_H_
#define NSL_THREADLOCK_H_

#include <pthread.h>

namespace nsl {

class ThreadLock_linux
{
public:
    ThreadLock_linux();
    ~ThreadLock_linux();
    void lock();
    void unlock();

private:
    pthread_mutex_t handle_;
};

class ThreadLock_Zero
{
public:
    ThreadLock_Zero()
    {
    }
    ~ThreadLock_Zero()
    {
    }
    void lock()
    {
    }
    void unlock()
    {
    }
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

} // namespace nsl

#include "ThreadLock.inl"

#endif // NSL_THREADLOCK_H_
