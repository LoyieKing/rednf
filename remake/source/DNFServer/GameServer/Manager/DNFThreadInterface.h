#ifndef DNF_THREAD_INTERFACE_H_
#define DNF_THREAD_INTERFACE_H_

#include <pthread.h>

// ---- CThreadInterface ----
class CThreadInterface
{
public:
    CThreadInterface();
    virtual ~CThreadInterface();
    virtual char begin();
    virtual void* dispatch_proxy(void* param);
    virtual void* dispatch(void* param) = 0;
    virtual void stop();
    virtual void join();
    pthread_t m_thread;  // +4
    char m_stop;         // +8
};

#endif  // DNF_THREAD_INTERFACE_H_
