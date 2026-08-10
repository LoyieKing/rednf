#ifndef DNF_NETWORK_THREAD_H_
#define DNF_NETWORK_THREAD_H_

#include "DNFPacketBuffer.h"
#include "DNFThreadInterface.h"

class CApplication;
class CMutex;

// ---- CUdpNetworkThread：0x30 ----
class CUdpNetworkThread : public CThreadInterface
{
public:
    CUdpNetworkThread();
    ~CUdpNetworkThread();
    void attach(CApplication* app);
    void* dispatch(void* param);
    void SetUDPQueue(UdpRecvQueue* q);
    UdpRecvQueue* m_udpQueue;   // +0xc
    void* m_udpHandler;         // +0x10
    CMutex* m_udpQLock;         // +0x14
    CMutex* m_udpBLock;         // +0x18
};

#endif  // DNF_NETWORK_THREAD_H_
