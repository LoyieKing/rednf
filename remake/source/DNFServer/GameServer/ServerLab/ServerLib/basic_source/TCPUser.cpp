// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TCPUser/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <stdio.h>

#include "TCPUser.h"
#include "TCPSocket.h"
#include "ISession.h"
#include "WorkThread.h"
#include "RecvBuffer.h"
#include "TraceLog.h"
#include "Script.h"
#include "ServiceFactory.h"
#include "DataPools.h"
#include "DataPool.h"

namespace nsl {

static unsigned int user_count = 0;
int ddebug = 0;

#include "TCPUser/TCPUser.inc"
    : mRecvBuffer(G_Script()->findIntValue(0, 0xc), G_Script()->findIntValue(0, 0xd))
{
    bDisconnected_ = false;
    bAboutToDisconnect_ = false;
    user_count = user_count + 1;
    mUserId = user_count;
    pSession = 0;
    mbBindedSession = false;
    mPendingWork = 0;
    mPendingSend = 0;
    mMaxPacketSize = G_Script()->findIntValue(0, 0xd);
}

#include "TCPUser/dtor_TCPUser.inc"
{
}

#include "TCPUser/initialize.inc"
{
    bDisconnected_ = false;
    bAboutToDisconnect_ = false;
    mbSending = false;
    DisLast_ = 0;
    m_PosByWouldBlock = 0;
    m_SizeByWouldBlock = 0;
    wouldBlockQueue.clear();
    pSession = 0;
    mbBindedSession = false;
    mbPassiveCloseSyncByWorker = false;
    mbActiveCloseSyncByWorker = false;
    pmWorkThread = 0;
    setLastAccessTime();
}

#include "TCPUser/shutdown.inc"
{
    bDisconnected_ = false;
    bAboutToDisconnect_ = false;
}

#include "TCPUser/startupAfterSetSocket.inc"
{
    bDisconnected_ = false;
    bAboutToDisconnect_ = false;
}

#include "TCPUser/setSession.inc"
{
    pSession = pSession_;
}

#include "TCPUser/getSession.inc"
{
    return pSession;
}

#include "TCPUser/setSendDataType.inc"
{
    mSendDataType = Type;
}

#include "TCPUser/getSendDataType.inc"
{
    return mSendDataType;
}

#include "TCPUser/setRecvDataType.inc"
{
    mRecvDataType = Type;
}

#include "TCPUser/getRecvDataType.inc"
{
    return mRecvDataType;
}

#include "TCPUser/setSocket.inc"
{
    pSock_ = s;
}

#include "TCPUser/getSocket.inc"
{
    return pSock_;
}

#include "TCPUser/getHandle.inc"
{
    return pSock_->getHandle();
}

#include "TCPUser/setNeedReconnect.inc"
{
    needReconnect_ = bReconnect;
}

#include "TCPUser/getNeedReconnect.inc"
{
    return needReconnect_;
}

#include "TCPUser/IncPendingWorkNum.inc"
{
    mPendingWork = mPendingWork + 1;
}

#include "TCPUser/DecPendingWorkNum.inc"
{
    if (mPendingWork != 0)
    {
        mPendingWork = mPendingWork - 1;
    }
}

#include "TCPUser/IncPendingSendNum.inc"
{
    mPendingSend = mPendingSend + 1;
}

#include "TCPUser/DecPendingSendNum.inc"
{
    if (mPendingSend != 0)
    {
        mPendingSend = mPendingSend - 1;
    }
}

#include "TCPUser/setBindedSession.inc"
{
    mbBindedSession = value;
}

#include "TCPUser/SetSending.inc"
{
    mbSending = isSending;
}

#include "TCPUser/SetWorking.inc"
{
    mbWorking = isWorking;
}

#include "TCPUser/postDisconnected.inc"
{
    bAboutToDisconnect_ = true;
    G_TraceLog()->sysLog(3, "In postDisconnected ='%d'", nReason);
    nReason_ = nReason;
}

#include "TCPUser/setLastAccessTime.inc"
{
    tickLast_ = pApp->getTick();
}

#include "TCPUser/getLastAccessTime.inc"
{
    return tickLast_;
}

#include "TCPUser/isAboutToDisconnect.inc"
{
    return bAboutToDisconnect_;
}

#include "TCPUser/isDisconnected.inc"
{
    return bDisconnected_;
}

#include "TCPUser/IsSending.inc"
{
    if (mPendingSend)
    {
        return true;
    }
    return mbSending;
}

#include "TCPUser/IsWorking.inc"
{
    if (mPendingWork)
    {
        return true;
    }
    return mbWorking;
}

#include "TCPUser/isPassiveCloseSyncByWorker.inc"
{
    return mbPassiveCloseSyncByWorker;
}

#include "TCPUser/setPassiveSyncByWorker.inc"
{
    mbPassiveCloseSyncByWorker = value;
}

#include "TCPUser/isActiveCloseSyncByWorker.inc"
{
    return mbActiveCloseSyncByWorker;
}

#include "TCPUser/setActiveSyncByWorker.inc"
{
    mbActiveCloseSyncByWorker = value;
}

#include "TCPUser/onPassiveClose.inc"
{
    G_TraceLog()->sysLog(3, "call onPassiveClose : '%s', '%d'\tTotal Con = %d", file, line, pApp->super_DataPools.getDataPool()->GetTcpUserCount());
    G_TraceLog()->sysLog(3,
        "bAboutToDisconnect_=%d, bDisconnected_=%d, getSession=%x, isSyncByWorker=%d, isBindedSession=%d",
        (unsigned int)bAboutToDisconnect_, (unsigned int)bDisconnected_, this,
        isPassiveCloseSyncByWorker(), isBindedSession());
    if (bAboutToDisconnect_ != true)
    {
        bAboutToDisconnect_ = true;
    }
    // ORIG: bDisconnected_ 为真直接 return true；IsSending/IsWorking 检查在非断开分支。
    if (bDisconnected_ != false)
    {
        return true;
    }
    if (!IsSending() && !IsWorking())
    {
        pApp->super_DataPools.getDataPool()->destroyTCPUser(this);
        return true;
    }
    G_TraceLog()->sysLog(0, "onPassiveClose(): mPendingSendNum not 0");
    return false;
}

#include "TCPUser/onActiveClose.inc"
{
    G_TraceLog()->sysLog(0, "call onActiveClose");
    G_TraceLog()->sysLog(5,
        "bAboutToDisconnect_=%d, bDisconnected_=%d, getSession=%x, isSyncByWorker=%d, isBindedSession=%d, pending send num=%d, pending work num=%d",
        (unsigned int)bAboutToDisconnect_, (unsigned int)bDisconnected_, getSession(),
        isActiveCloseSyncByWorker(), isBindedSession(), GetPendingSendNum(),
        GetPendingWorkNum());
    if (ddebug++ <= 0x13)
    {
        puts("call onActiveClose");
        printf("bAboutToDisconnect_=%d, bDisconnected_=%d, getSession=%p, isSyncByWorker=%d, isBindedSession=%d\n",
               (unsigned int)bAboutToDisconnect_, (unsigned int)bDisconnected_,
               getSession(), isActiveCloseSyncByWorker(), isBindedSession());
    }
    if (mUserId == key)
    {
        if (bAboutToDisconnect_ != true)
        {
            bAboutToDisconnect_ = true;
        }
        // ORIG：正条件 if/else（直 test+je + mov $1+jmp），!x 会物化 xor+test+je。
        if (bDisconnected_)
        {
            return true;
        }
        else
        {
            pApp->super_DataPools.getDataPool()->destroyTCPUser(this);
        }
    }
    return true;
}

#include "TCPUser/getWorkThread.inc"
{
    return pmWorkThread;
}

#include "TCPUser/onRead.inc"
{
    G_TraceLog()->sysLog(8, "S=======================================================================[%d]", mUserId);
    int nRead = 0;
    int size = mRecvBuffer.AvailableSize();
    if (0 < size)
    {
        nRead = pSock_->recv(mRecvBuffer.GetRear(), size);
        if (0 < nRead)
        {
            G_TraceLog()->sysLog(4, "available size : %d, nRead %d\n", size, nRead);
            mRecvBuffer.AdjustRear(nRead);
            if (!mRecvBuffer.Parse(this))
            {
                postDisconnected(1);
            }
        }
        else
        {
            if (nRead == -1)
            {
                postDisconnected(0);
            }
        }
    }
    return nRead;
}

#include "TCPUser/onWriteByCMsg.inc"
{
    if (pSock_ == 0)
    {
        G_TraceLog()->sysLog(7, "onWrite_ : Error(1)");
        return -1;
    }
    if (pSock_->getHandle() < 0)
    {
        G_TraceLog()->sysLog(7, "onWrite_ : Error(2)");
        return -2;
    }
    if (isAboutToDisconnect() || isDisconnected())
    {
        G_TraceLog()->sysLog(7, "onWrite_ : Error(3)");
        return -3;
    }
    if (cell->GetSize() == 0)
    {
        G_TraceLog()->sysLog(7, "onWrite_ : Error(4)");
        return -4;
    }
    int totalSize = cell->GetSize();
    while (m_PosByWouldBlock != cell->GetSize())
    {
        int nSent = pSock_->send(cell->GetBuf() + m_PosByWouldBlock, cell->GetSize() - m_PosByWouldBlock);
        if (nSent > 0)
        {
            m_PosByWouldBlock = m_PosByWouldBlock + nSent;
            totalSize = totalSize - nSent;
            G_TraceLog()->sysLog(3, "SENT  \xBA\xB8\xB3\xBD\xB7\xAE=%d, \xB3\xB2\xC0\xBA\xB7\xAE=%d, \xC3\xD1=%d", m_PosByWouldBlock, totalSize, cell->GetSize());
            G_TraceLog()->sysLog(3, "+++SENT  \xBA\xB8\xB3\xBD\xB7\xAE=%d, \xB3\xB2\xC0\xBA\xB7\xAE=%d, \xC3\xD1=%d", m_PosByWouldBlock, totalSize, cell->GetSize());
        }
        else
        {
            if (nSent == 0)
            {
                G_TraceLog()->sysLog(3, "WOULD \xBA\xB8\xB3\xBD\xB7\xAE=%d, \xB3\xB2\xC0\xBA\xB7\xAE=%d, \xC3\xD1=%d", m_PosByWouldBlock, totalSize, cell->GetSize());
                return 0;
            }
            G_TraceLog()->sysLog(3, "Fail OnWriteByCMsg");
            postDisconnected(0x1e);
            if (nSent == -100)
            {
                getSession()->onDoClose();
            }
            return -5;
        }
    }
    G_TraceLog()->sysLog(3, "SEND  \xBA\xB8\xB3\xBD\xB7\xAE=%d, \xB3\xB2\xC0\xBA\xB7\xAE=%d, \xC3\xD1=%d", m_PosByWouldBlock, totalSize, cell->GetSize());
    m_PosByWouldBlock = 0;
    totalSize = 0;
    return 1;
}

#include "TCPUser/PopSendMessage.inc"
{
    Message* pMessage;
    if (!wouldBlockQueue.empty())
    {
        pMessage = wouldBlockQueue.front();
        wouldBlockQueue.pop_front();
    }
    else
    {
        pMessage = mSendMessageQueue.front();
        mSendMessageQueue.pop_front();
    }
    return pMessage;
}

#include "TCPUser/ClearRecvMsgs.inc"
{
    while (!mRecvBuffer.ClearUsedMsgs())
    {
    }
}

#include "TCPUser/onError.inc"
{
    onPassiveClose("TCPUser.cpp", 0x11a);
}

} // namespace nsl
