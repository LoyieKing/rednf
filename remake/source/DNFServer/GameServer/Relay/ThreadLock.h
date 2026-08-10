#ifndef THREADLOCK_H_
#define THREADLOCK_H_

#include <pthread.h>

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

// ---- TThreadStateControl：lock_@0 / m_bisStarted@1 / m_bisStop@2 / m_bisTerminating@3 /
//      m_bIsTerminated@4（relay 布局，与 channel 一致，与 auction 头文件顺序不同）----

template <class TThreadLock_>
class TThreadStateControl
{
public:
    TThreadStateControl()
    {
        m_bisStarted = false;
        m_bisStop = false;
        m_bisTerminating = false;
        m_bIsTerminated = false;
    }
    ~TThreadStateControl()
    {
    }
    void setStarted()
    {
        lock_.lock();
        m_bisStarted = true;
        lock_.unlock();
    }
    bool isStarted()
    {
        lock_.lock();
        bool bRet = m_bisStarted;
        lock_.unlock();
        return bRet;
    }
    void setStop()
    {
        lock_.lock();
        m_bisStop = true;
        lock_.unlock();
    }
    bool isStop()
    {
        lock_.lock();
        bool bRet = m_bisStop;
        lock_.unlock();
        return bRet;
    }
    void setTerminate()
    {
        lock_.lock();
        m_bisTerminating = true;
        lock_.unlock();
    }
    bool isTerminating()
    {
        lock_.lock();
        bool bRet = m_bisTerminating;
        lock_.unlock();
        return bRet;
    }
    void setTerminated()
    {
        lock_.lock();
        m_bIsTerminated = true;
        lock_.unlock();
    }
    bool isTerminated()
    {
        lock_.lock();
        bool bRet = m_bIsTerminated;
        lock_.unlock();
        return bRet;
    }

private:
    TThreadLock_ lock_;
    bool m_bisStarted;
    bool m_bisStop;
    bool m_bisTerminating;
    bool m_bIsTerminated;
};

typedef pthread_t ThreadHandle;
typedef int ThreadIDType;
typedef TThreadStateControl<TThreadLock<ThreadLock_Zero> > ThreadStateControl;

#endif // THREADLOCK_H_
