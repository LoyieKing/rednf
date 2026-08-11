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
    // ORIG __LINE__ 实测 0x16=22（point/auction 同源一致）
#line 22
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
            // ORIG __LINE__ 实测 0x4c=76（point/auction 同源一致）
#line 76
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
        // 有符号 int + (unsigned int) 强转比较 → mov 槽重载 + cmp $0x11,%eax; ja
        int lenCheck = mRearIdx - mParseIdx;
        if ((unsigned int)lenCheck < 0x12)
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
        // ORIG：无 bool 局部，|| 条件物化在 eax（mov $1/jmp/mov $0/test %al）；
        // 右侧写成 mParseIdx + msgSize > mQueueSize + mMaxPacketSize
        // （先求 parse+size 再求 queue+max）
        if (pUser->GetMaxPacketSize() < msgSize
            || mParseIdx + msgSize > mQueueSize + mMaxPacketSize)
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
            G_TraceLog()->sysLog(7, "FAIL: Message From ActiveClose User - msg ident(%d) Qindex(%d)", Message::ident, mParseIdx);
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
        if (mParseIdx < mPartialQueueSize)
        {
        }
        else
        {
            if (mParseIdx != mRearIdx)
            {
                puts("wtf");
            }
            // ORIG __LINE__ 实测 0xf9=249（point/auction 同源一致）
#line 249
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
    // ORIG 局部作用域序：外层（ip3..ip0 最先声明）→ while 体（pRawMsg/msgSize/pTcpUser）→ 内层 if（pTcpUserTmp）
    unsigned int ip3;
    unsigned int ip2;
    unsigned int ip1;
    unsigned int ip0;
    while (!mRecvMsgs.empty())
    {
        Message* pRawMsg = mRecvMsgs.front();
        if (!pRawMsg->getUse())
        {
            if (pRawMsg->getUse())
            {
                TCPUser* pTcpUserTmp = pRawMsg->getUserFromMessage();
                // ORIG：ip 字节按 +3..+0 求值并各自落栈槽（-0x38..-0x2c），
                // ident 以单个 64 位 vararg 一次双字装载进 ebx/esi
                ip3 = (unsigned int)pTcpUserTmp->pSock_->getPeerAdrs()[3];
                ip2 = (unsigned int)pTcpUserTmp->pSock_->getPeerAdrs()[2];
                ip1 = (unsigned int)pTcpUserTmp->pSock_->getPeerAdrs()[1];
                ip0 = (unsigned int)pTcpUserTmp->pSock_->getPeerAdrs()[0];
                G_TraceLog()->sysLog(5, "force delete activeclose size(%d) ident(%d) ip(%d.%d.%d.%d)",
                                     (unsigned int)mRecvMsgs.size(),
                                     Message::ident,
                                     ip0, ip1, ip2, ip3);
            }
            int msgSize = pRawMsg->mSize;
            mRecvMsgs.pop_front();
            TCPUser* pTcpUser = pRawMsg->getUserFromMessage();
            pTcpUser->pmWorkThread->destroyOrderPool(pRawMsg);
            mFrontIdx = mFrontIdx + msgSize;
            if (mFrontIdx < mPartialQueueSize)
            {
            }
            else
            {
                mFrontIdx = 0;
            }
        }
        else
        {
            break;
        }
    }
    if (mRecvMsgs.empty())
    {
        return true;
    }
    return false;
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
