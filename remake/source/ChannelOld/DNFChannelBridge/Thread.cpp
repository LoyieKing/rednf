#include "Thread.h"
#include "Exception.h"
#include "System.h"

Thread::Thread()
{
    handle_ = 0;
    id_ = 0;
}

Thread::~Thread()
{
    if (states_.isStarted() && !isTerminated())
    {
        throw Exception("[Thread::~Thread] abnormal destruction. you should terminate the thread,first.");
    }
}

bool Thread::begin()
{
    states_.setStarted();
    id_ = pthread_create(&handle_, NULL, proxyLoop, this);
    if (id_ < 0)
    {
        throw Exception("[Thread::begin] can't begin thread");
    }
    return true;
}

void Thread::setTerminate()
{
    states_.setTerminate();
}

bool Thread::isTerminated()
{
    return states_.isTerminated();
}

void Thread::waitForTerminated(int nTimeSleep)
{
    setTerminate();
    while (!isTerminated())
    {
        TSystem<LinuxSystem>::sleep(nTimeSleep);
    }
}

void* Thread::proxyLoop(void* temp)
{
    Thread* inst = (Thread*)temp;
    inst->loop(temp);
    return 0;
}
