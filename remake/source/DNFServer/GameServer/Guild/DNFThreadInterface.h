#ifndef DNFTHREADINTERFACE_H_
#define DNFTHREADINTERFACE_H_

#include <pthread.h>
#include <queue>

#include "PacketHeader.h"

class CThreadInterface;

// from GuildThread.h
class CThreadInterface
{
public:
    CThreadInterface();
    virtual ~CThreadInterface();
    virtual void stop();
    virtual void join();
    virtual void dispatch(void* param) = 0;
    int begin();
    static void* dispatch_proxy(void* temp);
    pthread_t m_thread;   // +4
    bool m_running;       // +8
};

#endif
