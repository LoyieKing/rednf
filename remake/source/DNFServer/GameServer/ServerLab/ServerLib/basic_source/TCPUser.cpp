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

TCPUser::TCPUser()
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

TCPUser::~TCPUser()
{
}

void TCPUser::initialize()
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

void TCPUser::shutdown()
{
    bDisconnected_ = false;
    bAboutToDisconnect_ = false;
}

void TCPUser::startupAfterSetSocket()
{
    bDisconnected_ = false;
    bAboutToDisconnect_ = false;
}

void TCPUser::setSession(ISession* pSession_)
{
    pSession = pSession_;
}

ISession* TCPUser::getSession()
{
    return pSession;
}

void TCPUser::setSendDataType(ENUM_DATA_TYPE Type)
{
    mSendDataType = Type;
}

TCPUser::ENUM_DATA_TYPE TCPUser::getSendDataType()
{
    return mSendDataType;
}

void TCPUser::setRecvDataType(ENUM_DATA_TYPE Type)
{
    mRecvDataType = Type;
}

TCPUser::ENUM_DATA_TYPE TCPUser::getRecvDataType()
{
    return mRecvDataType;
}

void TCPUser::setSocket(TCPSocket* s)
{
    pSock_ = s;
}

TCPSocket* TCPUser::getSocket()
{
    return pSock_;
}

int TCPUser::getHandle()
{
    return pSock_->getHandle();
}

void TCPUser::setNeedReconnect(bool bReconnect)
{
    needReconnect_ = bReconnect;
}

bool TCPUser::getNeedReconnect()
{
    return needReconnect_;
}

void TCPUser::IncPendingWorkNum()
{
    mPendingWork = mPendingWork + 1;
}

void TCPUser::DecPendingWorkNum()
{
    if (mPendingWork != 0)
    {
        mPendingWork = mPendingWork - 1;
    }
}

void TCPUser::IncPendingSendNum()
{
    mPendingSend = mPendingSend + 1;
}

void TCPUser::DecPendingSendNum()
{
    if (mPendingSend != 0)
    {
        mPendingSend = mPendingSend - 1;
    }
}

void TCPUser::setBindedSession(bool value)
{
    mbBindedSession = value;
}

void TCPUser::SetSending(bool isSending)
{
    mbSending = isSending;
}

void TCPUser::SetWorking(bool isWorking)
{
    mbWorking = isWorking;
}

void TCPUser::postDisconnected(int nReason)
{
    bAboutToDisconnect_ = true;
    G_TraceLog()->sysLog(3, "In postDisconnected ='%d'", nReason);
    nReason_ = nReason;
}

void TCPUser::setLastAccessTime()
{
    tickLast_ = pApp->getTick();
}

__int64 TCPUser::getLastAccessTime()
{
    return tickLast_;
}

bool TCPUser::isAboutToDisconnect() const
{
    return bAboutToDisconnect_;
}

bool TCPUser::isDisconnected() const
{
    return bDisconnected_;
}

bool TCPUser::IsSending() const
{
    if (mPendingSend)
    {
        return true;
    }
    return mbSending;
}

bool TCPUser::IsWorking() const
{
    if (mPendingWork)
    {
        return true;
    }
    return mbWorking;
}

bool TCPUser::isPassiveCloseSyncByWorker()
{
    return mbPassiveCloseSyncByWorker;
}

void TCPUser::setPassiveSyncByWorker(bool value)
{
    mbPassiveCloseSyncByWorker = value;
}

bool TCPUser::isActiveCloseSyncByWorker()
{
    return mbActiveCloseSyncByWorker;
}

void TCPUser::setActiveSyncByWorker(bool value)
{
    mbActiveCloseSyncByWorker = value;
}

bool TCPUser::onPassiveClose(char* file, int line)
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

bool TCPUser::onActiveClose(unsigned int key)
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

WorkThread* TCPUser::getWorkThread()
{
    return pmWorkThread;
}

int TCPUser::onRead()
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

int TCPUser::onWriteByCMsg(CMsgCell* cell)
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

Message* TCPUser::PopSendMessage()
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

void TCPUser::ClearRecvMsgs()
{
    while (!mRecvBuffer.ClearUsedMsgs())
    {
    }
}

void TCPUser::onError()
{
    onPassiveClose("TCPUser.cpp", 0x11a);
}

} // namespace nsl
