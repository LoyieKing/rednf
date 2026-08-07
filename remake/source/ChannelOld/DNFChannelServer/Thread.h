#ifndef THREAD_H_
#define THREAD_H_

#include "stdafx.h"
#include "ThreadLock.h"

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

class Thread
{
public:
    Thread();
    virtual ~Thread();
    virtual void loop(void* pParam) = 0;

    bool begin();
    void setStop()
    {
        states_.setStop();
    }
    bool isStop()
    {
        return states_.isStop();
    }
    void setTerminate();
    bool isTerminating()
    {
        return states_.isTerminating();
    }
    bool isTerminated();
    void setTerminated()
    {
        states_.setTerminated();
    }
    void waitForTerminated(int nTimeSleep);
    static void* proxyLoop(void* temp);

protected:
    ThreadHandle handle_;
    ThreadIDType id_;
    ThreadStateControl states_;
};

#endif // THREAD_H_
