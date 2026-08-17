#ifndef NSL_WORKTHREAD_H_
#define NSL_WORKTHREAD_H_

#include <pthread.h>
#include <queue>

#include "Thread.h"

namespace nsl {

class IMessageStruct;
class IArea;
class Message;

class WorkThread : public Thread
{
public:
    WorkThread();
    WorkThread(int id);
    virtual ~WorkThread();
    virtual void loop(void* temp);
    void PushTransaction(IMessageStruct* pMsg);
    IMessageStruct* PopTransaction();
    // ORIG：头内 inline（point 无 out-of-line 符号）
    IArea* getWorkArea()
    {
        return mpArea;
    }
    void SetArea(IArea* pArea);
    Message* createOrderPool();
    void destroyOrderPool(Message* pMsg);
    int GetThreadId();
    size_t GetQueueSizeNoLock();
    size_t GetTransactionCntPerSec();
    void InitTransactionCntPerSec();

    std::queue<IMessageStruct*> orderQueue;
    int mId;
    IArea* mpArea;
    void* OrderPool;
    pthread_mutex_t workerLock;
    pthread_cond_t isFull;
    pthread_cond_t isEmpty;
    size_t mQueueSize;
    size_t mTransactionCntPerSec;
};

} // namespace nsl

#endif // NSL_WORKTHREAD_H_
