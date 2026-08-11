#ifndef DNF_THREAD_INTERFACE_H_
#define DNF_THREAD_INTERFACE_H_

#include <pthread.h>

// CThreadInterface：vptr@0 / handle@4 / running@8
// vtable：stop / join / dtor / dtor / dispatch
class CThreadInterface
{
public:
    CThreadInterface();
    virtual void stop();
    virtual void join();
    virtual ~CThreadInterface();
    virtual void dispatch(void* param) = 0;
    bool begin();
    static void* dispatch_proxy(void* temp);
    pthread_t m_handle;   // +4
    bool m_running;       // +8（ORIG 为 bool：while 条件直出 test/jne，char 会多 setne 物化）
};

#endif // DNF_THREAD_INTERFACE_H_
