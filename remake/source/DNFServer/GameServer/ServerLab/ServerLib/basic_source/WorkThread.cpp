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
        // ORIG: idLo/idHi 常驻 ebx/esi（register 局部，直接双字装载，无 shift 机器码）
        register unsigned int idLo = (unsigned int)Message::ident;
        register unsigned int idHi = (unsigned int)(Message::ident >> 32);
        G_TraceLog()->sysLog(4, "RECV PUSH USER=%x, ID=%d, msg=%d", u, u->mUserId, (int)idLo, (int)idHi);
    }
    mQueueSize = mQueueSize + 1;
    pthread_cond_signal(&isEmpty);
    pthread_mutex_unlock(&workerLock);
}

IMessageStruct* WorkThread::PopTransaction()
{
    pthread_mutex_lock(&workerLock);
    while (orderQueue.size() == 0)
    {
        pthread_cond_wait(&isEmpty, &workerLock);
    }
    Message* msg = (Message*)orderQueue.front();
    orderQueue.pop();
    // ORIG: jbe join(nop); Dec; jmp join+1; nop; join: mQueueSize--  (goto 复现空块/nop)
    if ((unsigned int)((int)msg->mMsgType - 1) <= 2)
    {
        goto join;
    }
    msg->getUserFromMessage()->DecPendingWorkNum();
join:
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
    // proxyLoop passes Thread* as temp (== this); ORIG loads arg@0xc for GetThreadId
    tlsThreadId = ((WorkThread*)temp)->GetThreadId();
    // ORIG 局部声明序（DWARF decl_line 375/377/378/380/382/383/390/393/398/404/405/411/413/414）
    IMessageStruct* recvMessage;
    ITimeEntity* teMsg;
    Message* pkMsg;
    Message* pMsg;
    TCPUser* pUser;
    CMsgCell* recvMsg;
    TCPDispatcher* handlerTCP = pApp->super_Dispatchers.getTCPDispatcher();
    InterDispatcher* pInterHandler = pApp->super_Dispatchers.GetInterDispatcher();
    TimerThread* timerThread = pApp->super_Threads.getTimerThread();
    TCPUser::ENUM_DATA_TYPE DataType;
    int CompressLen = 0x3c00;
    TMsgCell<524288> tmpBuffer;
    CMsgCell* encMsg = GetMessageBuffer(0x80000);
    CMsgCell* zipMsg = GetMessageBuffer(0x80000);
    while (true)
    {
    RETRY_MSG:
        recvMessage = PopTransaction();
        // ORIG: cmpl/jne process; nop; jmp top（while(true)+continue 复现；null 不递增 mTransactionCntPerSec）
        if (recvMessage == NULL)
        {
            continue;
        }

        int return_code = 0;
        switch (recvMessage->mMsgType)
        {
        case 1:
        {
            pMsg = (Message*)recvMessage;
            INTERNALMSG_HEADER* pInterMsg = pMsg->getCellFromMessage()->GetInternalMsg();
            if (!pInterMsg->bWillDelete)
            {
                pInterHandler->dispatch(pMsg);
                pInterMsg->bWillDelete = true;
                pApp->super_Threads.getWorkThread(pInterMsg->mOwnerWorkId)
                    ->PushTransaction(recvMessage);
            }
            else
            {
                pInterMsg->bWillDelete = false;
                pApp->super_DataPools.getCommonDataPool(tlsThreadId)->destroyMessage(pMsg);
            }
            break;
        }
        case 2:
        {
            teMsg = (ITimeEntity*)recvMessage;
            if (!teMsg->isTerminated())
            {
                return_code = teMsg->operator()();
                if (return_code != 0)
                {
                    G_TraceLog()->sysLog(7, "Fail: TIME : failed to handle '%d', error_code('%d').",
                                         teMsg->proc_id, return_code);
                }
                // ORIG：acUser 检查直接复用 teMsg（无独立 pkMsg 槽）
                if (((Message*)teMsg)->acUser != NULL)
                {
                    if (((Message*)teMsg)->mBufferType != BUFFER_TYPE_NOT_SETTED)
                    {
                        // acUser is a refcount stored in a pointer-sized field (integer -1, not element ptr arith)
                        ((Message*)teMsg)->acUser = (TCPUser*)((char*)((Message*)teMsg)->acUser - 1);
                        if (((Message*)teMsg)->acUser == NULL)
                        {
                            *(char*)&((Message*)teMsg)->mpSendBuffer = 0;
                            timerThread->PushTimeReqEvent(teMsg);
                        }
                    }
                }
            }
            else
            {
                // virtual destroyTimeEntity — leave as direct call for vtable *%reg form
                pApp->super_DataPools.getCommonDataPool(tlsThreadId)->destroyTimeEntity(teMsg);
            }
            break;
        }
        case 3:
            break;
        default:
        {
            pkMsg = (Message*)recvMessage;
            pUser = pkMsg->getUserFromMessage();
            if (pUser->isAboutToDisconnect() || pUser->isDisconnected())
            {
                pUser->SetWorking(false);
                register unsigned int idLo = (unsigned int)Message::ident;
                register unsigned int idHi = (unsigned int)(Message::ident >> 32);
                G_TraceLog()->sysLog(7, "\xb2\xf7\xb1\xe4 \xc0\xaf\xc0\xfa\xb0\xa1 worker\xb7\xce \xb5\xe9\xbe\xee\xbf\xd4\xb4\xd9. msg-%d", (int)idLo, (int)idHi);
                pkMsg->setUse(false);
                pUser->setActiveSyncByWorker(true);
                G_ActiveNetClose()->pushActiveClose(pUser);
                // ORIG: disconnect path skips mTransactionCntPerSec++ and re-pops
                goto RETRY_MSG;
            }
            else
            {
                recvMsg = pkMsg->getCellFromMessage();
                DataType = pUser->getRecvDataType();
                // ORIG：GetPacket() 结果直用，无 pHeader 局部（避免多一个栈槽）
                G_TraceLog()->sysLog(4, "RECV PCK ct    =%d", recvMsg->GetPacket()->getCategory());
                G_TraceLog()->sysLog(4, "RECV PCK id    =%d", recvMsg->GetPacket()->getPacketID());
                G_TraceLog()->sysLog(4, "RECV PCK seq   =%u", recvMsg->GetPacket()->sequence);
                recvMsg->GetPacket();
                if (DataType == TCPUser::RECV_DATA_NORMAL)
                {
                    handlerTCP->dispatch(pUser, pkMsg);
                    pkMsg->setUse(false);
                    if (pUser->GetPendingWorkNum() == 0)
                    {
                        pUser->SetWorking(false);
                    }
                }
                zipMsg->Clear();
                encMsg->Clear();
                CompressLen = 0x3c00;
                register unsigned int idLo = (unsigned int)Message::ident;
                register unsigned int idHi = (unsigned int)(Message::ident >> 32);
                G_TraceLog()->sysLog(8, "work ended id=%d", (int)idLo, (int)idHi);
            }
            break;
        }
        }
        mTransactionCntPerSec = mTransactionCntPerSec + 1;
    }
}

} // namespace nsl
