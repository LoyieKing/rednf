// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DataPool/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

#include "DataPool/DataPool.inc"
{
    LogSendMessagePool = new boost::object_pool<Message, boost::default_user_allocator_new_delete>(0x400, 0);
    BufferLogSend = new boost::object_pool<SendBuffer, boost::default_user_allocator_new_delete>(0x400, 0);
    TCPSocketPool = new object_pool_by_boost_pool<TCPSocket, int, int, int>(0x800);
    TCPUserPool = new object_pool_by_boost_pool<TCPUser, int, int, int>(0x800);
    mTcpUserCount = 0;
}

#include "DataPool/getLogMessage.inc"
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LogSendMsgLock);
    Message* msg = LogSendMessagePool->malloc();
    msg->setUserToMessage(u);
    msg->setOnDataTypeMask(u->getSendDataType());
    SendBuffer* pSendBuffer = BufferLogSend->malloc();
    msg->setStringToMessage(pSendBuffer);
    return msg;
}

#include "DataPool/destroyLogMessage.inc"
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LogSendMsgLock);
    BufferLogSend->free(msg->getSendBufferFromMessage());
    LogSendMessagePool->free(msg);
}

#include "DataPool/createTCPSocket.inc"
{
    TCPSocket* r = TCPSocketPool->construct();
    if (r == NULL)
    {
        return NULL;
    }
    return r;
}

#include "DataPool/destroyTCPSocket.inc"
{
    TCPSocketPool->free(pTCPSocket);
}

#include "DataPool/createTCPUser.inc"
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

#include "DataPool/destroyTCPUser.inc"
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

#include "DataPool/GetTcpUserCount.inc"
{
    return mTcpUserCount;
}

} // namespace nsl
