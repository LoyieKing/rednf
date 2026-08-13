#ifndef NSL_THREAD_H_
#define NSL_THREAD_H_

#include "ThreadLock.h"
#include "System.h"

namespace nsl {

extern __thread unsigned int tlsThreadId;

template <class TThreadLock_>
class TThreadStateControl
{
public:
    TThreadStateControl()
    {
        // ORIG：m_bisStarted(+4) 先写，再按 m_bisStop/m_bisTerminating/m_bIsTerminated。
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
    bool m_bisStop;
    bool m_bisTerminating;
    bool m_bIsTerminated;
    TThreadLock_ lock_;
    bool m_bisStarted;
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

} // namespace nsl

#endif // NSL_THREAD_H_
