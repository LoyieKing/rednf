#ifndef DBMW_DNFTCPACCEPTTHREAD_H_
#define DBMW_DNFTCPACCEPTTHREAD_H_

#include "DBMWCommon.h"
#include "DNFTcpSocket.h"


class CTcpAcceptThread : public CThreadInterface
{
public:
    CTcpAcceptThread();
    ~CTcpAcceptThread();
    void attach(CTcpNetSystem* net);
    void dispatch(void* param);
    CTcpNetSystem* m_net;   // +0xc
    CMutex* m_recvQLock;    // +0x10
    CMutex* m_recvBLock;    // +0x14
    TCPSocket m_sock;       // +0x18
    unsigned short m_port;  // +0x34
};

#endif  // DBMW_DNFTCPACCEPTTHREAD_H_
