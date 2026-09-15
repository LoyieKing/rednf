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
    // ORIG _ZTV16CThreadInterface 槽序实证（slot0=stop / slot1=join / slot2,3=析构 /
    // slot4=dispatch）⇒ 析构必须在 stop()/join() **之后**声明（声明序即虚槽序）。
    // 成员布局/sizeof 不变；这是真实 ABI 缺陷（原先经基类指针调 stop()/join() 会落到析构槽）。
    virtual void stop();
    virtual void join();
    virtual ~CThreadInterface();
    virtual void dispatch(void* param) = 0;
    bool begin();
    static void* dispatch_proxy(void* temp);
    pthread_t m_handle;   // +4
    bool m_running;       // +8
};

#endif // DNF_THREAD_INTERFACE_H_
