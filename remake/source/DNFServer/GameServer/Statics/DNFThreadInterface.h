#ifndef DNF_THREAD_INTERFACE_H_
#define DNF_THREAD_INTERFACE_H_

#include <pthread.h>

class CApplication;

// CThreadInterface：vptr@0 / handle@4 / running@8
// vtable：stop / join / dtor / dtor / dispatch
class CThreadInterface
{
public:
    CThreadInterface();
    virtual ~CThreadInterface();
    virtual void stop();
    virtual void join();
    virtual void dispatch(void* param) = 0;
    bool begin();
    static void* dispatch_proxy(void* temp);
    pthread_t m_handle;   // +4
    char m_running;       // +8
};

#endif // DNF_THREAD_INTERFACE_H_
