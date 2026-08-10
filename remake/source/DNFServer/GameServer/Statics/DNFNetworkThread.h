#ifndef DNF_NETWORK_THREAD_H_
#define DNF_NETWORK_THREAD_H_

#include <pthread.h>

#include "DNFThreadInterface.h"

class CApplication;
class CUdpRecvBuffer;
class CUdpHandler;

// CNetworkThread：CThreadInterface@0 + queue*@0xc + CUdpHandler*@0x10 +
//                 CMutex*@0x14 + CMutex*@0x18（0x1c）
class CNetworkThread : public CThreadInterface
{
public:
    CNetworkThread();
    virtual ~CNetworkThread();
    virtual void dispatch(void* param);
    void attach(CApplication* app);
    void* m_queue;   // +0xc
    void* m_udp;     // +0x10
    void* m_lock;    // +0x14
    void* m_bLock;   // +0x18
};

#endif // DNF_NETWORK_THREAD_H_
