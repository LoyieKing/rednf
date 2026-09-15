#ifndef DBMW_DNFTCPNETWORKTHREAD_H_
#define DBMW_DNFTCPNETWORKTHREAD_H_

#include "DBMWCommon.h"


class CTcpNetworkThread : public CThreadInterface
{
public:
    CTcpNetworkThread();
    ~CTcpNetworkThread();
    void attach(CTcpNetSystem* net);
    void dispatch(void* param);
    CTcpNetSystem* m_net;   // +0xc
    void* m_recvQ;          // +0x10
    CTcpHandler* m_handler; // +0x14
    CMutex* m_recvQLock;    // +0x18
    CMutex* m_recvBLock;    // +0x1c
    void* m_sendQ;          // +0x20
    CMutex* m_sendQLock;    // +0x24
    CMutex* m_sendBLock;    // +0x28
    bool m_runningFlag;     // +0x2c
};

#endif  // DBMW_DNFTCPNETWORKTHREAD_H_
