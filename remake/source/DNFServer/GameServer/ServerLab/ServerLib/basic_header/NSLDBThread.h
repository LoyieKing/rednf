#ifndef NSL_NSLDBTHREAD_H_
#define NSL_NSLDBTHREAD_H_

#include <pthread.h>
#include <queue>

#include "Thread.h"

namespace nsl {

class Message;

class NSLDBThread : public Thread
{
public:
    NSLDBThread();
    virtual ~NSLDBThread();
    virtual void loop(void* pParam);
    void PushTransaction(Message* pMessage);
    Message* PopTransaction();
    size_t GetTransactionCntPerSec()
    {
        return mTransactionCntPerSec;
    }

    std::queue<Message*, std::deque<Message*> > dbQueue;
    int mQueueSize;
    size_t mTransactionCntPerSec;
    pthread_mutex_t dbLock;
    pthread_cond_t isFull;
    pthread_cond_t isEmpty;
};

} // namespace nsl

#endif // NSL_NSLDBTHREAD_H_
