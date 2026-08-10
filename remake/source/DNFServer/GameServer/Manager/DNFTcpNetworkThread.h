#ifndef DNF_TCP_NETWORK_THREAD_H_
#define DNF_TCP_NETWORK_THREAD_H_

#include "DNFThreadInterface.h"

class CTcpHandler;
class CTcpNetSystem;
class CMutex;

// ---- CTcpNetworkThread：0x30 ----
class CTcpNetworkThread : public CThreadInterface
{
public:
    CTcpNetworkThread();
    ~CTcpNetworkThread();
    void attach(CTcpNetSystem* net);
    void* dispatch(void* param);
    CTcpNetSystem* m_net;   // +0xc
    void* m_recvQ;          // +0x10
    CTcpHandler* m_handler; // +0x14
    CMutex* m_recvQLock;    // +0x18
    CMutex* m_recvBLock;    // +0x1c
    void* m_sendQ;          // +0x20
    CMutex* m_sendQLock;    // +0x24
    CMutex* m_sendBLock;    // +0x28
    char m_runningFlag;     // +0x2c
};

#endif  // DNF_TCP_NETWORK_THREAD_H_
