// nsl::NSLDBThread.cpp
#include <pthread.h>

#include "NSLDBThread.h"
#include "ServiceFactory.h"
#include "Dispatchers.h"
#include "DBDispatcher.h"
#include "DataPool.h"
#include "Message.h"
#include "CommonDataPool.h"
#include "InternalMsgDesign.h"
#include "DBTransactionDesign.h"
#include "MsgCell.h"

namespace nsl {

NSLDBThread::NSLDBThread()
    : dbQueue()
{
    mQueueSize = 0;
    mTransactionCntPerSec = 0;
    pthread_mutex_init(&dbLock, NULL);
    pthread_cond_init(&isFull, NULL);
    pthread_cond_init(&isEmpty, NULL);
}

NSLDBThread::~NSLDBThread()
{
}

void NSLDBThread::PushTransaction(Message* pMessage)
{
    pthread_mutex_lock(&dbLock);
    mQueueSize = mQueueSize + 1;
    dbQueue.push(pMessage);
    pthread_cond_signal(&isEmpty);
    pthread_mutex_unlock(&dbLock);
}

Message* NSLDBThread::PopTransaction()
{
    pthread_mutex_lock(&dbLock);
    while (dbQueue.size() == 0)
    {
        pthread_cond_wait(&isEmpty, &dbLock);
    }
    Message* pMsg = dbQueue.front();
    dbQueue.pop();
    mQueueSize = mQueueSize - 1;
    pthread_mutex_unlock(&dbLock);
    return pMsg;
}

void NSLDBThread::loop(void* temp)
{
    DBDispatcher* pDbDispatcher = pApp->super_Dispatchers.getDBDispatcher();
    Message* pMsg;
    LPDBTR_HEADER pDbTr;
    while (true)
    {
        do
        {
            pMsg = PopTransaction();
            pDbTr = pMsg->getCellFromMessage()->GetDBTr();
        } while (pDbTr->mbWillDelete == true);
        pDbDispatcher->dispatch(pMsg);
        pDbTr->mbWillDelete = true;
        pApp->super_DataPools.getCommonDataPool(tlsThreadId)->destroyMessage(pMsg);
        mTransactionCntPerSec = mTransactionCntPerSec + 1;
    }
}

} // namespace nsl
