#ifndef DNFNETWORKTHREAD_H_
#define DNFNETWORKTHREAD_H_

#include <pthread.h>
#include <queue>

#include "PacketHeader.h"
#include "DNFThreadInterface.h"

class CApplication;
class CUdpNetworkThread;
class CUdpRecvBuffer;

// from GuildThread.h
class CUdpNetworkThread : public CThreadInterface
{
public:
    CUdpNetworkThread();
    ~CUdpNetworkThread();
    void attach(CApplication* app);
    void SetUDPQueue(std::queue<CUdpRecvBuffer*>* q);
    void dispatch(void* param);
    CApplication* m_app;   // +0xc
    void* m_queue;         // +0x10
    void* m_lock;          // +0x14
    void* m_bLock;         // +0x18
};

#endif
