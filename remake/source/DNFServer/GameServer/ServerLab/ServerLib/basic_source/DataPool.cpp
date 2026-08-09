#include <time.h>

#include "DataPool.h"
#include "Message.h"
#include "SendBuffer.h"
#include "TCPUser.h"
#include "TCPSocket.h"
#include "ISession.h"
#include "WorkThread.h"
#include "TraceLog.h"
#include "ServiceFactory.h"
#include "Reactor.h"

namespace nsl {

__int64 Message::ident = 0;

DataPool::DataPool()
{
    LogSendMessagePool = new boost::object_pool<Message, boost::default_user_allocator_new_delete>(0x400, 0);
    BufferLogSend = new boost::object_pool<SendBuffer, boost::default_user_allocator_new_delete>(0x400, 0);
    TCPSocketPool = new object_pool_by_boost_pool<TCPSocket, int, int, int>(0x800);
    TCPUserPool = new object_pool_by_boost_pool<TCPUser, int, int, int>(0x800);
    mTcpUserCount = 0;
}

Message* DataPool::getLogMessage(TCPUser* u)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LogSendMsgLock);
    Message* msg = LogSendMessagePool->malloc();
    msg->setUserToMessage(u);
    msg->setOnDataTypeMask(u->getSendDataType());
    SendBuffer* pSendBuffer = BufferLogSend->malloc();
    msg->setStringToMessage(pSendBuffer);
    return msg;
}

void DataPool::destroyLogMessage(Message* msg)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LogSendMsgLock);
    BufferLogSend->free(msg->getSendBufferFromMessage());
    LogSendMessagePool->free(msg);
}

TCPSocket* DataPool::createTCPSocket()
{
    TCPSocket* r = TCPSocketPool->construct();
    if (r == NULL)
    {
        return NULL;
    }
    return r;
}

void DataPool::destroyTCPSocket(TCPSocket* pTCPSocket)
{
    TCPSocketPool->free(pTCPSocket);
}

TCPUser* DataPool::createTCPUser()
{
    TCPUser* r = TCPUserPool->construct();
    if (r == NULL)
    {
        return NULL;
    }
    r->initialize();
    mTcpUserCount = mTcpUserCount + 1;
    G_TraceLog()->sysLog(3, "createTCPUser(): Total Con = %d", GetTcpUserCount());
    return r;
}

void DataPool::destroyTCPUser(TCPUser* pTCPUser)
{
    if (pTCPUser->isBindedSession())
    {
        G_TraceLog()->sysLog(8, "destroy!!! isSending=%d, rp-%d, sp-%d, wc-%d",
            pTCPUser->IsSending(),
            pTCPUser->GetPendingWorkNum(),
            pTCPUser->GetPendingSendNum(),
            pTCPUser->getWorkThread()->orderQueue.size());
    }
    else
    {
        G_TraceLog()->sysLog(8, "destroy!!! isSending=%d, rp-%d, sp-%d",
            pTCPUser->IsSending(),
            pTCPUser->GetPendingWorkNum(),
            pTCPUser->GetPendingSendNum());
    }
    if (pTCPUser->IsSending())
    {
        __int64 curTime = time(NULL);
        if (pTCPUser->DisLast_ == 0)
        {
            pTCPUser->DisLast_ = curTime;
            pTCPUser->bDisconnected_ = false;
            return;
        }
        if (curTime - pTCPUser->DisLast_ < 0x78)
        {
            pTCPUser->bDisconnected_ = false;
            return;
        }
    }
    G_TraceLog()->sysLog(8, "destroy!!!aaa");
    pTCPUser->ClearRecvMsgs();
    G_TraceLog()->sysLog(8, "destroy!!!bbb");
    if (pTCPUser->getSession() != NULL)
    {
        pTCPUser->getSession()->onClose(false);
        pTCPUser->setSession(NULL);
    }
    TCPSocket* sock = pTCPUser->getSocket();
    if (sock != NULL)
    {
        EpollReactor<TCPUser>* pReactor = pApp->super_Reactor.getReactor();
        pReactor->unregistHandle(pTCPUser);
        sock->close();
        destroyTCPSocket(sock);
    }
    pTCPUser->setSocket(NULL);
    pTCPUser->SetSending(false);
    pTCPUser->bDisconnected_ = true;
    pTCPUser->~TCPUser();
    TCPUserPool->free(pTCPUser);
    mTcpUserCount = mTcpUserCount - 1;
    G_TraceLog()->sysLog(3, "destroyTCPUser(): Total Con = %d", GetTcpUserCount());
}

unsigned int DataPool::GetTcpUserCount()
{
    return mTcpUserCount;
}

} // namespace nsl
