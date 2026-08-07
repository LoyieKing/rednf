// nsl::TCPSendThread.cpp
#include <stdio.h>
#include <pthread.h>

#include "TCPSendThread.h"
#include "ServiceFactory.h"
#include "Threads.h"
#include "DataPool.h"
#include "TCPUser.h"
#include "Message.h"
#include "TraceLog.h"
#include "TMsgCell.h"
#include "PacketDesign.h"
#include "CommonDataPool.h"

namespace nsl {

TCPSendThread::TCPSendThread()
    : sendQueue()
{
    mQueueSize = 0;
    sequence = 0;
    pthread_mutex_init(&senderLock, NULL);
    pthread_cond_init(&isEmpty, NULL);
}

TCPSendThread::~TCPSendThread()
{
}

void TCPSendThread::PushSendMsg(Message* msg)
{
    G_TraceLog()->sysLog(8, "In PushSendUser");
    pthread_mutex_lock(&senderLock);
    TCPUser* u = msg->getUserFromMessage();
    bool bRet;
    if ((u->isAboutToDisconnect()) || (u->isDisconnected()))
    {
        bRet = true;
    }
    else
    {
        bRet = false;
    }
    if (bRet)
    {
        u->SetSending(false);
        msg->initialize();
        SendDataPoolFree(msg, u);
        G_TraceLog()->sysLog(8, "SEND FREE msg=%d", (int)Message::ident, (int)(Message::ident >> 32));
        pthread_cond_signal(&isEmpty);
        pthread_mutex_unlock(&senderLock);
        G_TraceLog()->sysLog(8, "66don't put this user");
    }
    else
    {
        u->IncPendingSendNum();
        u->SetSending(true);
        if (msg->GetWouldBlock())
        {
            u->PushWouldBlockMessage(msg);
        }
        else
        {
            u->PushSendMessage(msg);
        }
        sendQueue.push(u);
        mQueueSize = mQueueSize + 1;
        pthread_cond_signal(&isEmpty);
        pthread_mutex_unlock(&senderLock);
        G_TraceLog()->sysLog(8, "out PushSendUser");
    }
}

Message* TCPSendThread::PopSendMsg()
{
    G_TraceLog()->sysLog(8, "In PopSendUser this=%x", this);
    pthread_mutex_lock(&senderLock);
    while (sendQueue.size() == 0)
    {
        pthread_cond_wait(&isEmpty, &senderLock);
    }
    TCPUser* u = sendQueue.front();
    Message* msg = u->PopSendMessage();
    sendQueue.pop();
    mQueueSize = mQueueSize - 1;
    u = msg->getUserFromMessage();
    u->DecPendingSendNum();
    pthread_mutex_unlock(&senderLock);
    G_TraceLog()->sysLog(8, "Out PopSendUser");
    return msg;
}

void TCPSendThread::SendDataPoolFree(Message* msg, TCPUser* pUser)
{
    pUser->SetSending(false);
    msg->initialize();
    G_TraceLog()->sysLog(8, "SEND ------ msg=%d", (int)Message::ident, (int)(Message::ident >> 32));
    pApp->super_DataPools.getCommonDataPool(msg->mOwnerWorkId)->destroySendMessage(msg);
}

void TCPSendThread::loop(void* temp)
{
    printf("Start up TCPSendThread-%p\n", temp);
    G_TraceLog()->sysLog(8, "Start up TCPSendThread");
    TMsgCell<524288> tmpbuffer;
    while (true)
    {
        try
        {
            Message* msg = PopSendMsg();
            TCPUser* pUser = msg->getUserFromMessage();
            if ((pUser->isAboutToDisconnect()) || (pUser->isDisconnected()))
            {
                SendDataPoolFree(msg, pUser);
                continue;
            }
            CMsgCell* cell = msg->getCellFromMessage();
            PACKET_HEADER* pPCK = cell->GetPacket();
            pPCK->sequence = sequence;
            sequence = sequence + 1;
            G_TraceLog()->sysLog(8, "SEND MSG who id=%d, add=%p", pUser->mUserId, pUser);
            pPCK = msg->getCellFromMessage()->GetPacket();
            G_TraceLog()->sysLog(8, "SEND PCK ct    =%d", pPCK->getCategory());
            pPCK = msg->getCellFromMessage()->GetPacket();
            G_TraceLog()->sysLog(8, "SEND PCK id    =%d", pPCK->getPacketID());
            pPCK = msg->getCellFromMessage()->GetPacket();
            G_TraceLog()->sysLog(8, "SEND PCK size  =%d", pPCK->sLength);
            if (msg->getDataTypeMask(2) == 0)
            {
                G_TraceLog()->sysLog(8, "size=%d", pPCK->sLength);
                int ret = pUser->onWriteByCMsg(cell);
                if (ret == 0)
                {
                    msg->setOffDataTypeMask(0);
                    msg->setOnDataTypeMask(0);
                    G_TraceLog()->sysLog(8, "Would block ... size=%d", cell->GetSize());
                    msg->SetWouldBlock();
                    PushSendMsg(msg);
                    continue;
                }
                if (pUser->GetPendingSendNum() == 0)
                {
                    pUser->SetSending(false);
                }
            }
            SendDataPoolFree(msg, pUser);
        }
        catch (...)
        {
        }
    }
}

} // namespace nsl
