#ifndef DNF_NETWORK_THREAD_H_
#define DNF_NETWORK_THREAD_H_

#include "DNFThreadInterface.h"

class CApplication;

// CNetworkThread：CThreadInterface@0 + app@0xc + queue*[10]@0x10 +
//                 CMutex*[10]@0x38 + CMutex*@0x60（0x64）
class CNetworkThread : public CThreadInterface
{
public:
    CNetworkThread();
    virtual ~CNetworkThread();
    virtual void dispatch(void* param);
    void attach(CApplication* app, int idx);
    void* m_udp;                     // +0xc（CUdpHandler*）
    void* m_queues[10];              // +0x10
    void* m_locks[10];               // +0x38
    void* m_bLock;                   // +0x60
};

#endif // DNF_NETWORK_THREAD_H_
