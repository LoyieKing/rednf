#ifndef DNF_TCP_ACCEPT_THREAD_H_
#define DNF_TCP_ACCEPT_THREAD_H_

#include "DNFThreadInterface.h"
#include "DNFTcpSocket.h"

class CTcpNetSystem;
class CMutex;

// ---- CTcpAcceptThread：0x38 ----
class CTcpAcceptThread : public CThreadInterface
{
public:
    CTcpAcceptThread();
    ~CTcpAcceptThread();
    void attach(CTcpNetSystem* net);
    void* dispatch(void* param);
    CTcpNetSystem* m_net;   // +0xc
    CMutex* m_recvQLock;    // +0x10
    CMutex* m_recvBLock;    // +0x14
    TCPSocket m_sock;       // +0x18
    unsigned short m_port;  // +0x34
};

#endif  // DNF_TCP_ACCEPT_THREAD_H_
