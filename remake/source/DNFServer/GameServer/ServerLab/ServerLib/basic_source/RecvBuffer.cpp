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
    mQueue = (char*)operator new(queueSize);
    mQueueSize = queueSize;
    assert(mQueueSize > maxPacketSize && "Queue size smaller than MAX_PACKET");
    mPartialQueueSize = mQueueSize - maxPacketSize;
    mMaxPacketSize = maxPacketSize;
}

RecvBuffer::~RecvBuffer()
{
    if (mQueue != NULL)
    {
        operator delete(mQueue);
    }
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
            return mQueueSize - mRearIdx + 0x12;
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
        unsigned int parsableLength = mRearIdx - mParseIdx;
        if (parsableLength < 0x12)
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
        if (pUser->GetMaxPacketSize() < msgSize || (mQueueSize + mMaxPacketSize < mParseIdx + msgSize))
        {
            return false;
        }
        if ((int)parsableLength < msgSize)
        {
            break;
        }
        Message* pMessage = pUser->pmWorkThread->createOrderPool();
        if (pMessage == NULL)
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
LAB_DONE:
    ClearUsedMsgs();
    return ret;
}

bool RecvBuffer::ClearUsedMsgs()
{
    while (!mRecvMsgs.empty())
    {
        Message* pRawMsg = mRecvMsgs.front();
        if (pRawMsg->getUse())
        {
            break;
        }
        if (pRawMsg->getUse())
        {
            TCPUser* pTcpUserTmp = pRawMsg->getUserFromMessage();
            unsigned char* adrs;
            unsigned char b0, b1, b2, b3;
            adrs = pTcpUserTmp->pSock_->getPeerAdrs();
            b3 = adrs[3];
            adrs = pTcpUserTmp->pSock_->getPeerAdrs();
            b2 = adrs[2];
            adrs = pTcpUserTmp->pSock_->getPeerAdrs();
            b1 = adrs[1];
            adrs = pTcpUserTmp->pSock_->getPeerAdrs();
            b0 = adrs[0];
            G_TraceLog()->sysLog(5, "force delete activeclose size(%d) ident(%d) ip(%d.%d.%d.%d)",
                                 (int)mRecvMsgs.size(), (int)Message::ident,
                                 (int)(Message::ident >> 32), (int)b0, (int)b1, (int)b2, (int)b3);
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
