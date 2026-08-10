#ifndef DNFTCPNETWORKTHREAD_H_
#define DNFTCPNETWORKTHREAD_H_

#include <pthread.h>
#include <queue>

#include "PacketHeader.h"
#include "DNFThreadInterface.h"

class CTcpNetSystem;
class CTcpNetworkThread;

// from GuildThread.h
class CTcpNetworkThread : public CThreadInterface
{
public:
    CTcpNetworkThread();
    ~CTcpNetworkThread();
    void attach(CTcpNetSystem* net);
    void dispatch(void* param);
    CTcpNetSystem* m_net;      // +0xc
    void* m_recvQ;             // +0x10
    void* m_handler;           // +0x14
    void* m_recvQLock;         // +0x18
    void* m_recvBLock;         // +0x1c
    void* m_sendQ;             // +0x20
    void* m_sendQLock;         // +0x24
    void* m_sendBLock;         // +0x28
    char m_runningFlag;        // +0x2c
    char m_pad[3];             // +0x2d
};

#endif
