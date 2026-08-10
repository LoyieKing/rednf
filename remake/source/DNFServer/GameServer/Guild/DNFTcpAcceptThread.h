#ifndef DNFTCPACCEPTTHREAD_H_
#define DNFTCPACCEPTTHREAD_H_

#include <pthread.h>
#include <queue>

#include "PacketHeader.h"
#include "DNFThreadInterface.h"

class CTcpAcceptThread;
class CTcpNetSystem;
class TCPSocket;

// from GuildThread.h
class CTcpAcceptThread : public CThreadInterface
{
public:
    CTcpAcceptThread();
    ~CTcpAcceptThread();
    void attach(CTcpNetSystem* net);
    void dispatch(void* param);
    CTcpNetSystem* m_net;      // +0xc
    void* m_recvQLock;         // +0x10
    void* m_recvBLock;         // +0x14
    char m_sock[0x1c];         // +0x18 TCPSocket
    unsigned short m_port;     // +0x34
    char m_pad[2];             // +0x36
};

#endif
