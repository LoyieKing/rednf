#include <pthread.h>

#include "WorkThread.h"
#include "Message.h"
#include "TCPUser.h"
#include "TraceLog.h"
#include "Script.h"
#include "ServiceFactory.h"
#include "Dispatchers.h"
#include "Threads.h"
#include "TimerThread.h"
#include "TCPDispatcher.h"
#include "InterDispatcher.h"
#include "CommonDataPool.h"
#include "ActiveNetClose.h"
#include "TMsgCell.h"
#include "GetMessageBuffer.h"

namespace nsl {

__thread unsigned int tlsThreadId;

WorkThread::WorkThread(int id)
    : orderQueue()
{
    OrderPool = new boost::object_pool<Message, boost::default_user_allocator_new_delete>(0x800, 0);
    mId = id;
    pthread_mutex_init(&workerLock, NULL);
    pthread_cond_init(&isEmpty, NULL);
    mQueueSize = 0;
    mTransactionCntPerSec = 0;
}

WorkThread::~WorkThread()
{
}

void WorkThread::SetArea(IArea* pArea)
{
    mpArea = pArea;
}

IArea* WorkThread::getWorkArea()
{
    return mpArea;
}

Message* WorkThread::createOrderPool()
{
    Message* msg = ((boost::object_pool<Message, boost::default_user_allocator_new_delete>*)OrderPool)->construct();
    if (msg != NULL)
    {
        msg->initialize();
    }
    return msg;
}

void WorkThread::destroyOrderPool(Message* msg)
{
    ((boost::object_pool<Message, boost::default_user_allocator_new_delete>*)OrderPool)->free(msg);
}

void WorkThread::PushTransaction(IMessageStruct* pMessage)
{
    pthread_mutex_lock(&workerLock);
    if ((int)pMessage->mMsgType - 1U < 3)
    {
        orderQueue.push(pMessage);
    }
    else
    {
        TCPUser* u = ((Message*)pMessage)->getUserFromMessage();
        if (u->isAboutToDisconnect() || u->isDisconnected())
        {
            u->SetWorking(false);
            ((Message*)pMessage)->setUse(false);
            pthread_cond_signal(&isEmpty);
            pthread_mutex_unlock(&workerLock);
            G_TraceLog()->sysLog(8, "161don't put this user");
            return;
        }
        u->IncPendingWorkNum();
        u->SetWorking(true);
        orderQueue.push(pMessage);
        G_TraceLog()->sysLog(4, "RECV PUSH USER=%x, ID=%d, msg=%d", u, u->mUserId, (int)Message::ident, (int)(Message::ident >> 32));
    }
    mQueueSize = mQueueSize + 1;
    pthread_cond_signal(&isEmpty);
    pthread_mutex_unlock(&workerLock);
}

IMessageStruct* WorkThread::PopTransaction()
{
    pthread_mutex_lock(&workerLock);
    while (true)
    {
        if (orderQueue.size() != 0)
        {
            break;
        }
        pthread_cond_wait(&isEmpty, &workerLock);
    }
    Message* msg = (Message*)orderQueue.front();
    orderQueue.pop();
    if (2 < (int)msg->mMsgType - 1U)
    {
    msg->getUserFromMessage()->DecPendingWorkNum();
    }
    mQueueSize = mQueueSize - 1;
    pthread_mutex_unlock(&workerLock);
    return (IMessageStruct*)msg;
}

int WorkThread::GetThreadId()
{
    return mId;
}

size_t WorkThread::GetQueueSizeNoLock()
{
    return mQueueSize;
}

size_t WorkThread::GetTransactionCntPerSec()
{
    return mTransactionCntPerSec;
}

void WorkThread::InitTransactionCntPerSec()
{
    mTransactionCntPerSec = 0;
}

void WorkThread::loop(void* temp)
{
    G_TraceLog()->sysLog(8, "Start up WorkThread");
    tlsThreadId = GetThreadId();
    TCPDispatcher* handlerTCP = pApp->super_Dispatchers.getTCPDispatcher();
    InterDispatcher* pInterHandler = pApp->super_Dispatchers.GetInterDispatcher();
    TimerThread* timerThread = pApp->super_Threads.getTimerThread();
    TMsgCell<524288> tmpBuffer;
    CMsgCell* encMsg = GetMessageBuffer(0x80000);
    CMsgCell* zipMsg = GetMessageBuffer(0x80000);
    do
    {
        IMessageStruct* recvMessage;
        do
        {
            recvMessage = PopTransaction();
        } while (recvMessage == NULL);
        char msgType = recvMessage->mMsgType;
        if (msgType == 2)
        {
            ITimeEntity* teMsg = (ITimeEntity*)recvMessage;
            if (teMsg->isTerminated())
            {
                CommonDataPool* pool = pApp->super_DataPools.getCommonDataPool(tlsThreadId);
                pool->destroyTimeEntity(recvMessage);
            }
            else
            {
                int err = teMsg->operator()();
                if (err != 0)
                {
                    G_TraceLog()->sysLog(7, "Fail: TIME : failed to handle '%d', error_code('%d').", ((Message*)recvMessage)->mOwnerWorkId, err);
                }
                Message* pkMsg = (Message*)recvMessage;
                if (pkMsg->acUser != NULL && pkMsg->mBufferType != BUFFER_TYPE_NOT_SETTED)
                {
                    pkMsg->acUser = pkMsg->acUser - 1;
                    if (pkMsg->acUser == NULL)
                    {
                        *(char*)&pkMsg->mpSendBuffer = 0;
                        timerThread->PushTimeReqEvent(teMsg);
                    }
                }
            }
        }
        else if (msgType == 3)
        {
        }
        else if (msgType == 1)
        {
            CMsgCell* cell = ((Message*)recvMessage)->getCellFromMessage();
            INTERNALMSG_HEADER* pInterMsg = cell->GetInternalMsg();
            if (pInterMsg->bWillDelete == true)
            {
                pInterMsg->bWillDelete = false;
                CommonDataPool* pool = pApp->super_DataPools.getCommonDataPool(tlsThreadId);
                pool->destroyMessage((Message*)recvMessage);
            }
            else
            {
                pInterHandler->dispatch((Message*)recvMessage);
                pInterMsg->bWillDelete = true;
                WorkThread* pWorkThread = pApp->super_Threads.getWorkThread(pInterMsg->mOwnerWorkId);
                pWorkThread->PushTransaction(recvMessage);
            }
        }
        else
        {
            Message* pMsg = (Message*)recvMessage;
            TCPUser* pUser = pMsg->getUserFromMessage();
            if (!(pUser->isAboutToDisconnect() || pUser->isDisconnected()))
            {
                CMsgCell* recvMsg = pMsg->getCellFromMessage();
                TCPUser::ENUM_DATA_TYPE dataType = pUser->getRecvDataType();
                PACKET_HEADER* pHeader = recvMsg->GetPacket();
                G_TraceLog()->sysLog(4, "RECV PCK ct    =%d", pHeader->getCategory());
                pHeader = recvMsg->GetPacket();
                G_TraceLog()->sysLog(4, "RECV PCK id    =%d", pHeader->getPacketID());
                pHeader = recvMsg->GetPacket();
                G_TraceLog()->sysLog(4, "RECV PCK seq   =%u", pHeader->sequence);
                recvMsg->GetPacket();
                if (dataType == TCPUser::RECV_DATA_NORMAL)
                {
                    handlerTCP->dispatch(pUser, pMsg);
                    pMsg->setUse(false);
                    if (pUser->GetPendingWorkNum() == 0)
                    {
                        pUser->SetWorking(false);
                    }
                }
                zipMsg->Clear();
                encMsg->Clear();
                G_TraceLog()->sysLog(8, "work ended id=%d", (int)Message::ident, (int)(Message::ident >> 32));
            }
            else
            {
                pUser->SetWorking(false);
                G_TraceLog()->sysLog(7, "\xB0\xED\xB9\xD6\xBD\xBA \xC6\xC7%B4\xE2 \xB5\xB5\xC3\xDF\xBD\xBA \xC5\xB8\xC0\xCE\xC6\xC4\xC6\xC4", (int)Message::ident, (int)(Message::ident >> 32));
                pMsg->setUse(false);
                pUser->setActiveSyncByWorker(true);
                G_ActiveNetClose()->pushActiveClose(pUser);
            }
        }
        mTransactionCntPerSec = mTransactionCntPerSec + 1;
    } while (true);
}

} // namespace nsl
