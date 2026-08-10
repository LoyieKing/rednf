#ifndef DBMW_DNFNETWORKTHREAD_H_
#define DBMW_DNFNETWORKTHREAD_H_

#include "DBMWCommon.h"


class CNetworkThread : public CThreadInterface
{
public:
    CNetworkThread();
    ~CNetworkThread();
    void attach(CApplication* app);
    void dispatch(void* param);
    void SetUDPQueue(UdpRecvQueue* q);
    UdpRecvQueue* m_udpQueue;   // +0xc
    void* m_udpHandler;         // +0x10
    CMutex* m_udpQLock;         // +0x14
    CMutex* m_udpBLock;         // +0x18
};

#endif  // DBMW_DNFNETWORKTHREAD_H_
