#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>

#include "ThreadLock.h"
#include "Exception.h"
#include "System.h"

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
