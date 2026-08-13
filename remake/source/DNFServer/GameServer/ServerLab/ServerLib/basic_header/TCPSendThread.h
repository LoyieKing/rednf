#ifndef NSL_TCPSENDTHREAD_H_
#define NSL_TCPSENDTHREAD_H_

#include <pthread.h>
#include <queue>

#include "Thread.h"

namespace nsl {

class Message;
class TCPUser;

class TCPSendThread : public Thread
{
public:
    TCPSendThread();
    virtual ~TCPSendThread();
    virtual void loop(void* pParam);
    void PushSendMsg(Message* msg);
    Message* PopSendMsg();
    void SendDataPoolFree(Message* msg, TCPUser* pUser);

    std::queue<TCPUser*, std::deque<TCPUser*> > sendQueue;
    pthread_mutex_t senderLock;
    pthread_cond_t isEmpty;
    size_t mQueueSize;
    unsigned int sequence;
};

} // namespace nsl

#endif // NSL_TCPSENDTHREAD_H_
