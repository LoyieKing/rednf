#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "RecvBuffer.h"
#include "Message.h"
#include "TCPSocket.h"
#include "TCPUser.h"
#include "WorkThread.h"
#include "TraceLog.h"
#include "ServiceFactory.h"

namespace nsl {

RecvBuffer::RecvBuffer(int queueSize, int maxPacketSize)
{
    mFrontIdx = 0;
    mRearIdx = 0;
    mParseIdx = 0;
    mParseStatus = 0;
    mQueue = new char[queueSize];
    mQueueSize = queueSize;
    assert(mQueueSize > maxPacketSize && "Queue size smaller than MAX_PACKET");
    mPartialQueueSize = mQueueSize - maxPacketSize;
    mMaxPacketSize = maxPacketSize;
}

RecvBuffer::~RecvBuffer()
{
    delete[] mQueue;
}

bool RecvBuffer::IsEmpty()
{
    if (mFrontIdx == mRearIdx)
    {
        return true;
    }
    return false;
}

bool RecvBuffer::IsFull()
{
    if ((mFrontIdx == 0 && mParseIdx > mPartialQueueSize) || (mFrontIdx - mRearIdx == 1))
    {
        return true;
    }
    return false;
}

int RecvBuffer::AvailableSize()
{
    if (!IsFull())
    {
        if (mRearIdx < mPartialQueueSize)
        {
            if (mRearIdx < mFrontIdx)
            {
                return (mFrontIdx - mRearIdx) + -1;
            }
            return mPartialQueueSize - mRearIdx;
        }
        if (mParseStatus == 0)
        {
            return mParseIdx - mRearIdx + 0x12;
        }
        if (mParseStatus == 1)
        {
            PACKET_HEADER* pHeader = (PACKET_HEADER*)(mQueue + mParseIdx);
            int msgSize = pHeader->getSize();
            assert(mRearIdx >= mParseIdx);
            if (mRearIdx > mParseIdx)
            {
                return mParseIdx - mRearIdx + msgSize;
            }
            if (mRearIdx == mParseIdx)
            {
                if (mFrontIdx != 0)
                {
                    mRearIdx = 0;
                    mParseIdx = 0;
                }
                else
                {
                    ClearUsedMsgs();
                }
            }
        }
    }
    else
    {
        ClearUsedMsgs();
    }
    return 0;
}

bool RecvBuffer::Parse(TCPUser* pUser)
{
    bool ret = true;
    if (!pUser->isBindedSession())
    {
        pUser->pmWorkThread = pApp->super_Threads.getLittleWorkThread();
    }
    while (mRearIdx > mParseIdx)
    {
        // ORIG：单一 lenCheck（@-0x24），无 parsableLength 双局部
        unsigned int lenCheck = mRearIdx - mParseIdx;
        if (lenCheck < 0x12)
        {
            mParseStatus = 0;
            break;
        }
        mParseStatus = 1;
        PACKET_HEADER* pHeader = (PACKET_HEADER*)(mQueue + mParseIdx);
        int msgSize = pHeader->getSize();
        if (msgSize < 0)
        {
            G_TraceLog()->sysLog(7, "ERR: msgsize < 0 ");
            return false;
        }
        // ORIG：单表达式初始化（bool 在 eax 物化，非栈槽）
        // ORIG：register bool（物化结果留在 al，不落栈槽）
        register bool bOversize = (pUser->GetMaxPacketSize() < msgSize
                                   || (mQueueSize + mMaxPacketSize < mParseIdx + msgSize));
        if (bOversize)
        {
            return false;
        }
        if ((int)lenCheck < msgSize)
        {
            break;
        }
        // ORIG：if ((pMessage = createOrderPool()) == NULL) —— 赋值在条件内物化 sete
        Message* pMessage;
        if ((pMessage = pUser->pmWorkThread->createOrderPool()) == NULL)
        {
            G_TraceLog()->sysLog(7, "FAIL: Message allocation");
            pUser->postDisconnected(3);
            break;
        }
        if (pUser->isActiveCloseSyncByWorker())
        {
            G_TraceLog()->sysLog(7, "FAIL: Message From ActiveClose User - msg ident(%d) Qindex(%d)", (int)Message::ident, (int)(Message::ident >> 32), mParseIdx);
            if (pMessage != NULL)
            {
                pUser->pmWorkThread->destroyOrderPool(pMessage);
            }
            break;
        }
        pMessage->setUserToMessage(pUser);
        pMessage->setCellToMessage(mQueue + mParseIdx, msgSize, msgSize);
        pMessage->setUse(true);
        mRecvMsgs.push_back(pMessage);
        pUser->pmWorkThread->PushTransaction(pMessage);
        mParseIdx = mParseIdx + msgSize;
        if (mParseIdx < 0)
        {
            G_TraceLog()->sysLog(7, "ERR: mParseIdx < 0 ");
            return false;
        }
        if (mPartialQueueSize <= mParseIdx)
        {
            if (mParseIdx != mRearIdx)
            {
                puts("wtf");
            }
            assert(mParseIdx == mRearIdx);
            if (mFrontIdx != 0)
            {
                mRearIdx = 0;
                mParseIdx = 0;
            }
        }
    }
    ClearUsedMsgs();
    return ret;
}

bool RecvBuffer::ClearUsedMsgs()
{
    while (!mRecvMsgs.empty())
    {
        Message* pRawMsg = mRecvMsgs.front();
        if (!pRawMsg->getUse())
        {
            if (pRawMsg->getUse())
            {
                TCPUser* pTcpUserTmp = pRawMsg->getUserFromMessage();
                unsigned int b3 = (unsigned int)pTcpUserTmp->pSock_->getPeerAdrs()[3];
                unsigned int b2 = (unsigned int)pTcpUserTmp->pSock_->getPeerAdrs()[2];
                unsigned int b1 = (unsigned int)pTcpUserTmp->pSock_->getPeerAdrs()[1];
                unsigned int b0 = (unsigned int)pTcpUserTmp->pSock_->getPeerAdrs()[0];
                // ORIG：idLo/idHi/sz 常驻 ebx/esi/edi（register 局部）
                register unsigned int idLo = *(unsigned int*)&Message::ident;
                register unsigned int idHi = *((unsigned int*)&Message::ident + 1);
                register unsigned int sz = (unsigned int)mRecvMsgs.size();
                G_TraceLog()->sysLog(5, "force delete activeclose size(%d) ident(%d) ip(%d.%d.%d.%d)",
                                     sz, idLo, idHi, b0, b1, b2, b3);
            }
            int msgSize = pRawMsg->mSize;
            mRecvMsgs.pop_front();
            pRawMsg->getUserFromMessage()->pmWorkThread->destroyOrderPool(pRawMsg);
            mFrontIdx = mFrontIdx + msgSize;
            if (mPartialQueueSize <= mFrontIdx)
            {
                mFrontIdx = 0;
            }
        }
        else
        {
            break;
        }
    }
    return mRecvMsgs.empty();
}

char* RecvBuffer::GetFront()
{
    return mQueue + mFrontIdx;
}

void RecvBuffer::AdjustRear(int size)
{
    if (0 < size)
    {
        mRearIdx = mRearIdx + size;
    }
}

char* RecvBuffer::GetRear()
{
    return mQueue + mRearIdx;
}

} // namespace nsl
