#ifndef DNF_THREAD_INTERFACE_H_
#define DNF_THREAD_INTERFACE_H_

#include <pthread.h>

// ---- CThreadInterface ----
class CThreadInterface
{
public:
    CThreadInterface();
    // ORIG 虚表槽位：stop / join / ~ / dispatch（析构排在 stop/join 之后）
    virtual void stop();
    virtual void join();
    virtual ~CThreadInterface();
    virtual void* dispatch(void* param) = 0;
    bool begin();
    static void* dispatch_proxy(void* param);
    pthread_t m_thread;  // +4
    bool m_stop;         // +8（ORIG：while(m_stop) 直接 test;jne，无 setne 物化）
};

#endif  // DNF_THREAD_INTERFACE_H_
