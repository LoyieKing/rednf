#include "ChannelServiceApp.h"
#include "Socket.h"
#include <errno.h>

ChannelServiceApp::TCPUserStates::TCPUserStates()
{
}

ChannelServiceApp::TCPUser::TCPUser()
{
    nAccID_ = 0;
    bDisconnected_ = false;
    bAboutToDisconnect_ = false;
}

ChannelServiceApp::TCPUser::~TCPUser()
{
}

void ChannelServiceApp::TCPUser::shutdown()
{
    nAccID_ = 0;
    bDisconnected_ = false;
    bAboutToDisconnect_ = false;
}

void ChannelServiceApp::TCPUser::startupAfterSetSocket()
{
    nAccID_ = 0;
    bDisconnected_ = false;
    bAboutToDisconnect_ = false;
}

void ChannelServiceApp::TCPUser::postDisconnected(int nReason)
{
    bAboutToDisconnect_ = true;
    gFileLogInfo.Lock();
    gFileLogInfo << "In postDisconnected =" << nReason << endl;
    gFileLogInfo.Unlock();
    nReason_ = nReason;
}

void ChannelServiceApp::TCPUser::setLastAccessTime()
{
    tickLast_ = TManager<ChannelService>::getManager()->getTick();
}

bool ChannelServiceApp::TCPUser::isToWrite() const
{
    int nSize = bufferSend_.getPushedLength();
    return 0 < nSize;
}

bool ChannelServiceApp::TCPUser::isIdle() const
{
    __int64 now = TManager<ChannelService>::getManager()->getTick();
    __int64 gap = now - tickLast_;
    if (tickLast_ == 0 || gap < 0 || (gap < 1 && (unsigned int)gap < 0x927c1))
    {
        return false;
    }
    return true;
}

void ChannelServiceApp::TCPUser::onAccept()
{
    bufferRecv_.clear();
    bufferSend_.clear();
}

void ChannelServiceApp::TCPUser::onError()
{
    onClose("TCPUser.cpp", 0x57);
}

void ChannelServiceApp::TCPUser::onClose(char* file, int line)
{
    bool dis = bDisconnected_;
    char* szError = strerror(*__errno_location());
    unsigned int nAccID = getACCID();
    gFileLogInfo.Lock();
    gFileLogInfo << "call onClose from " << file << ", " << line << ", TCPUSER=" << nAccID << ", error=" << szError << ", dis=" << dis << endl;
    gFileLogInfo.Unlock();
    if (bDisconnected_ == false)
    {
        TManager<ChannelService>::getManager()->UserPools::destroyTCPUser(this, "TCPUser.cpp", 0x6f);
        if (pSock_ != NULL)
        {
            pSock_->close();
        }
        bDisconnected_ = true;
    }
}

void ChannelServiceApp::TCPUser::onRead(char* file, int line)
{
    gFileLogInfo.Lock();
    gFileLogInfo << "OnRead Call from  " << file << ", line " << line << ", TCPUSER=" << getACCID() << endl;
    gFileLogInfo.Unlock();
    if (pSock_->getHandle() > -1)
    {
        if (!(isAboutToDisconnect() || isDisconnected()))
        {
            TScopedLock<TThreadLock<ThreadLock_linux> > rlock(lockRecv_);
            onRead_();
        }
    }
}

void ChannelServiceApp::TCPUser::onRead_()
{
    char buf[0xa0000];
    int nRead = pSock_->recv(buf, 0xa0000);
    int test_count = 0;
    gFileLogError.Lock();
    gFileLogError << "TCPUser.cpp" << " read=" << nRead << endl;
    gFileLogError.Unlock();
    if (nRead < 1)
    {
        if (nRead < 0)
        {
            postDisconnected(1);
            return;
        }
    }
    else
    {
        int nRet = bufferRecv_.push(buf, nRead, "TCPUser.cpp", 0xa0);
        if (nRet < 0)
        {
            gFileLogInfo.Lock();
            gFileLogInfo << "TCPUser.cpp" << "Fail to push = " << nRet << endl;
            gFileLogInfo.Unlock();
            postDisconnected(0);
            return;
        }
    }
    do
    {
        if (bufferRecv_.getPopLengthToEnd() < 0xb)
        {
            if (bufferRecv_.getPushedLength() < 0xb)
            {
                gFileLogInfo.Lock();
                gFileLogInfo << "need more header data : PushedLength=" << bufferRecv_.getPushedLength() << endl;
                gFileLogInfo.Unlock();
                return;
            }
            tagPacketHeader hdr;
            if (!bufferRecv_.peekCopy(0xb, (char*)&hdr))
            {
                gFileLogInfo.Lock();
                gFileLogInfo << "3.recv critical error occure!" << endl;
                gFileLogInfo.Unlock();
                postDisconnected(9);
                return;
            }
            int nMessageSize = hdr.getSize();
            if ((0xa0000 < nMessageSize) || (nMessageSize < 1))
            {
                postDisconnected(6);
                return;
            }
            if (nMessageSize < 1)
            {
                gFileLogInfo.Lock();
                gFileLogInfo << "2.recv critical error occure!" << endl;
                gFileLogInfo.Unlock();
                postDisconnected(8);
                return;
            }
            if (bufferRecv_.getPushedLength() < nMessageSize)
            {
                gFileLogInfo.Lock();
                gFileLogInfo << "2.need more body data : messagesize=" << nMessageSize << endl;
                gFileLogInfo.Unlock();
                return;
            }
            char* szBuf = (char*)malloc(nMessageSize);
            if (szBuf == NULL)
            {
                postDisconnected(0x10);
                return;
            }
            if (!bufferRecv_.popCopy(nMessageSize, szBuf))
            {
                free(szBuf);
                gFileLogInfo.Lock();
                gFileLogInfo << "1.\xbf\xa9\xb1\xe2\xbc\xad pop error \x20\xb0\xa1 \xb6\xb3\xbe\xee\xc1\xf6\xb8\xe9 \xbe\xc8\xb5\xc8\xb4\xd9." << endl;
                gFileLogInfo.Unlock();
                postDisconnected(7);
                return;
            }
            TManager<ChannelService>::getManager()->getTCPHandlerRelay()->dispatch(this, szBuf, nMessageSize, 0);
            free(szBuf);
        }
        else
        {
            tagPacketHeader* hdr = (tagPacketHeader*)bufferRecv_.peekPop();
            int nMessageSize = hdr->getSize();
            if ((0xa0000 < nMessageSize) || (nMessageSize < 1))
            {
                gFileLogInfo.Lock();
                gFileLogInfo << "Error Reason : nMessageSize " << nMessageSize << endl;
                gFileLogInfo.Unlock();
                postDisconnected(2);
                return;
            }
            if (nMessageSize < 1)
            {
                gFileLogInfo.Lock();
                gFileLogInfo << "recv critical error occure!" << endl;
                gFileLogInfo.Unlock();
                postDisconnected(5);
                return;
            }
            if (bufferRecv_.isPopStraight(nMessageSize))
            {
                TManager<ChannelService>::getManager()->getTCPHandlerRelay()->dispatch(this, (char*)hdr, nMessageSize, 0);
                if (bufferRecv_.pop(nMessageSize) < 0)
                {
                    gFileLogInfo.Lock();
                    gFileLogInfo << "1.\xbf\xa9\xb1\xe2\xbc\xad pop error \x20\xb0\xa1 \xb6\xb3\xbe\xee\xc1\xf6\xb8\xe9 \xbe\xc8\xb5\xc8\xb4\xd9." << endl;
                    gFileLogInfo.Unlock();
                    postDisconnected(3);
                    return;
                }
            }
            else
            {
                if (bufferRecv_.getPushedLength() < nMessageSize)
                {
                    return;
                }
                char* szBuf = (char*)malloc(nMessageSize);
                if (szBuf == NULL)
                {
                    postDisconnected(0x10);
                    return;
                }
                if (!bufferRecv_.popCopy(nMessageSize, szBuf))
                {
                    free(szBuf);
                    gFileLogInfo.Lock();
                    gFileLogInfo << "1.\xbf\xa9\xb1\xe2\xbc\xad pop error \x20\xb0\xa1 \xb6\xb3\xbe\xee\xc1\xf6\xb8\xe9 \xbe\xc8\xb5\xc8\xb4\xd9." << endl;
                    gFileLogInfo.Unlock();
                    postDisconnected(4);
                    return;
                }
                TManager<ChannelService>::getManager()->getTCPHandlerRelay()->dispatch(this, szBuf, nMessageSize, 0);
                free(szBuf);
            }
        }
        gFileLogInfo.Lock();
        gFileLogInfo << "recv empty? =" << bufferRecv_.isEmpty() << endl;
        gFileLogInfo.Unlock();
        test_count = test_count + 1;
        if (bufferRecv_.isEmpty() != 0)
        {
            return;
        }
    } while (true);
}

void ChannelServiceApp::TCPUser::onWrite(char* file, int line)
{
    gFileLogInfo.Lock();
    gFileLogInfo << "onWrite : Call from  index=" << ", " << file << ", line " << line << endl;
    gFileLogInfo.Unlock();
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockSend_);
    onWrite_();
}

void ChannelServiceApp::TCPUser::onWrite_()
{
    int count = 0;
    if (pSock_ == NULL)
    {
        gFileLogInfo.Lock();
        gFileLogInfo << "onWrite_ : Error(1)" << endl;
        gFileLogInfo.Unlock();
    }
    else if (pSock_->getHandle() < 0)
    {
        gFileLogInfo.Lock();
        gFileLogInfo << "onWrite_ : Error(2)" << endl;
        gFileLogInfo.Unlock();
    }
    else if (isAboutToDisconnect() || isDisconnected())
    {
        gFileLogInfo.Lock();
        gFileLogInfo << "onWrite_ : Error(3)" << endl;
        gFileLogInfo.Unlock();
    }
    else if (bufferSend_.isEmpty() == 0)
    {
        do
        {
            gFileLogInfo.Lock();
            gFileLogInfo << "start~~ " << count << endl;
            gFileLogInfo.Unlock();
            count = count + 1;
            if (bufferSend_.isPushGreaterThanPop())
            {
                int nSize = bufferSend_.getPushedLength();
                if (nSize < 1)
                {
                    gFileLogInfo.Lock();
                    gFileLogInfo << "AAA \xba\xec\xc1\xa4\xc3\xfb send queue \xb9\xdf\xc3\xfd nSize=" << nSize << endl;
                    gFileLogInfo.Unlock();
                    postDisconnected(0xc);
                    return;
                }
                if (pSock_ == NULL)
                {
                    return;
                }
                int nSent = pSock_->send(bufferSend_.peekPop(), nSize);
                if (nSent < 1)
                {
                    if (nSent == 0)
                    {
                        gFileLogInfo.Lock();
                        gFileLogInfo << "AAA send \xbd\xc7\xc6\xd4, \xbc\xd2\xc4\xc9 \xc0\xcc\xba\xa5\xc6\xae \xbf\xa9\xc0\xfc\xc8\xfe \xc1\xb8\xc0\xe7, \xb4\xd9\xc0\xfd \xbc\xbc\xc8\xb8\xbf\xa1 .. " << endl;
                        gFileLogInfo.Unlock();
                        return;
                    }
                    gFileLogInfo.Lock();
                    gFileLogInfo << "AAA send \xbd\xc7\xc6\xd4, disconnect.. count =" << count << endl;
                    gFileLogInfo.Unlock();
                    postDisconnected(0xb);
                    return;
                }
                if (bufferSend_.pop(nSent) < 0)
                {
                    postDisconnected(10);
                    gFileLogInfo.Lock();
                    gFileLogInfo << "AAA 2.pop \xc0\xd4 \xbd\xc7\xc6\xd4\xb8\xe9 \xb9\xae\xc1\xa2\xc0\xd4\xb4\xd9." << endl;
                    gFileLogInfo.Unlock();
                    return;
                }
                gFileLogInfo.Lock();
                gFileLogInfo << "1.AAA pop \xbc\xba\xb0\xf8, " << nSent << endl;
                gFileLogInfo.Unlock();
            }
            else
            {
                int nSize = bufferSend_.getPopLengthToEnd();
                if (nSize < 1)
                {
                    gFileLogInfo.Lock();
                    gFileLogInfo << "AAA \xba\xec\xc1\xa4\xc3\xfb send queue \xb9\xdf\xc3\xfd  nSize=" << nSize << endl;
                    gFileLogInfo.Unlock();
                    postDisconnected(0xf);
                    return;
                }
                if (pSock_ == NULL)
                {
                    return;
                }
                int nSent = pSock_->send(bufferSend_.peekPop(), nSize);
                if (nSent < 1)
                {
                    if (nSent == 0)
                    {
                        gFileLogInfo.Lock();
                        gFileLogInfo << "AAA send \xbd\xc7\xc6\xd4, \xbc\xd2\xc4\xc9 \xcc\xcc\xba\xa5\xc6\xae \xbf\xa9\xc0\xfc\xc8\xfe \xc1\xb8\xc0\xe7, \xb4\xd9\xc0\xfd \xbc\xbc\xc8\xb8\xbf\xa1 .. " << endl;
                        gFileLogInfo.Unlock();
                        return;
                    }
                    gFileLogInfo.Lock();
                    gFileLogInfo << "2.AAA send \xbd\xc7\xc6\xd4, disconnect.. count=" << count << endl;
                    gFileLogInfo.Unlock();
                    postDisconnected(0xe);
                    return;
                }
                if (bufferSend_.pop(nSent) < 0)
                {
                    gFileLogInfo.Lock();
                    gFileLogInfo << "AAA 3.pop \xc0\xd4 \xbd\xc7\xc6\xd4\xb8\xe9 \xb9\xae\xc1\xa2\xc0\xd4\xb4\xd9." << endl;
                    gFileLogInfo.Unlock();
                    postDisconnected(0xd);
                    return;
                }
                gFileLogInfo.Lock();
                gFileLogInfo << "AAA pop \xbc\xba\xb0\xf8, " << nSent << endl;
                gFileLogInfo.Unlock();
            }
            gFileLogInfo.Lock();
            gFileLogInfo << "send empty? =" << bufferSend_.isEmpty() << endl;
            gFileLogInfo.Unlock();
        } while (bufferSend_.isEmpty() == 0);
    }
    else
    {
        gFileLogInfo.Lock();
        gFileLogInfo << "onWrite_ : Error(4)" << endl;
        gFileLogInfo.Unlock();
    }
}

int ChannelServiceApp::TCPUser::onWrite2Buffer(CMsgCell* pMsgCell)
{
    if (isAboutToDisconnect() || isDisconnected())
    {
        gFileLogInfo.Lock();
        gFileLogInfo << "Send Queue Push error=-1" << endl;
        gFileLogInfo.Unlock();
        return -1;
    }
    else if (pSock_ == NULL)
    {
        gFileLogInfo.Lock();
        gFileLogInfo << "Send Queue Push error=-2" << endl;
        gFileLogInfo.Unlock();
        return -2;
    }
    else
    {
        SOCKET h = pSock_->getHandle();
        if (h < 0)
        {
            gFileLogInfo.Lock();
            gFileLogInfo << "Send Queue Push error=-3" << endl;
            gFileLogInfo.Unlock();
            return -3;
        }
        else
        {
            if (pMsgCell->GetSize() < 1)
            {
                gFileLogInfo.Lock();
                gFileLogInfo << "Send Queue Push error=-4" << endl;
                gFileLogInfo.Unlock();
                return -4;
            }
            else
            {
                TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockSend_);
                if (bufferSend_.getPushedLength() + pMsgCell->GetSize() < 0xa0000)
                {
                    int nRet = bufferSend_.push(pMsgCell->GetBuf(), pMsgCell->GetSize(), "TCPUser.cpp", 0x239);
                    if (nRet < 0)
                    {
                        gFileLogInfo.Lock();
                        gFileLogInfo << "Send Queue Push error=" << nRet << endl;
                        gFileLogInfo.Unlock();
                        return -6;
                    }
                }
                else
                {
                    gFileLogInfo.Lock();
                    gFileLogInfo << "Send Queue Push error=-5" << endl;
                    gFileLogInfo.Unlock();
                    return -5;
                }
            }
        }
    }
    return 0;
}

int ChannelServiceApp::TCPUser::send(CMsgCell* pMsgCell)
{
    if (pSock_ == NULL)
    {
        return -1;
    }
    if (pSock_->getHandle() > -1)
    {
        if (isAboutToDisconnect() || isDisconnected())
        {
            return -3;
        }
        if (pMsgCell->GetSize() < 1)
        {
            return -4;
        }
        TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockSend_);
        if (bufferSend_.getPushedLength() + pMsgCell->GetSize() < 0xa0000)
        {
            int nRet = bufferSend_.push(pMsgCell->GetBuf(), pMsgCell->GetSize(), "TCPUser.cpp", 0x20a);
            if (nRet < 0)
            {
                gFileLogInfo.Lock();
                gFileLogInfo << "Send Queue Push error=" << nRet << endl;
                gFileLogInfo.Unlock();
                return -6;
            }
            else
            {
                onWrite_();
                return 0;
            }
        }
        return -5;
    }
    return -2;
}

char* CMsgCell::GetBuf() const
{
    return m_bBuf;
}

int CMsgCell::GetSize() const
{
    return m_wSize;
}

template <unsigned int Size>
int TCircularQueueBuffer<Size>::push(char* in_block, int in_nSize, char* file, int line)
{
    if ((unsigned int)in_nSize < 0xa0000)
    {
        if (m_nPushIndex < m_nPopIndex)
        {
            if ((int)(m_nPopIndex - m_nPushIndex) <= in_nSize)
            {
                return -4;
            }
            memcpy(&m_buffer[m_nPushIndex], in_block, in_nSize);
            m_nPushIndex = m_nPushIndex + in_nSize;
        }
        else
        {
            unsigned int nFirstCutSize = 0xa0000 - m_nPushIndex;
            if (in_nSize < (int)nFirstCutSize)
            {
                memcpy(&m_buffer[m_nPushIndex], in_block, in_nSize);
                m_nPushIndex = m_nPushIndex + in_nSize;
            }
            else if (nFirstCutSize == (unsigned int)in_nSize)
            {
                if (m_nPopIndex == 0)
                {
                    return -2;
                }
                memcpy(&m_buffer[m_nPushIndex], in_block, in_nSize);
                m_nPushIndex = m_nPushIndex + in_nSize;
                if (m_nPushIndex == 0xa0000)
                {
                    m_nPushIndex = 0;
                }
            }
            else
            {
                if ((int)(nFirstCutSize + m_nPopIndex) <= in_nSize)
                {
                    return -3;
                }
                memcpy(&m_buffer[m_nPushIndex], in_block, nFirstCutSize);
                memcpy(&m_buffer[0], in_block + nFirstCutSize, in_nSize - nFirstCutSize);
                m_nPushIndex = in_nSize - nFirstCutSize;
            }
        }
        return 0;
    }
    return -1;
}

template <unsigned int Size>
int TCircularQueueBuffer<Size>::pop(int in_nSize)
{
    if (in_nSize < 1)
    {
        return -1;
    }
    if (m_nPushIndex >= m_nPopIndex)
    {
        int nLength = m_nPushIndex - m_nPopIndex;
        if (nLength < in_nSize)
        {
            return -2;
        }
        m_nPopIndex = m_nPopIndex + in_nSize;
        return 0;
    }
    else
    {
        int nFirstCut = 0xa0000 - m_nPopIndex;
        if (nFirstCut < in_nSize)
        {
            if (m_nPushIndex + nFirstCut < in_nSize)
            {
                return -3;
            }
            m_nPopIndex = in_nSize - nFirstCut;
            return 0;
        }
        m_nPopIndex = m_nPopIndex + in_nSize;
        if (m_nPopIndex == 0xa0000)
        {
            m_nPopIndex = 0;
        }
        return 0;
    }
}

template <unsigned int Size>
bool TCircularQueueBuffer<Size>::popCopy(int in_nSize, char* pCopyee)
{
    if (in_nSize < 1)
    {
        return false;
    }
    if (m_nPushIndex >= m_nPopIndex)
    {
        int nLength = m_nPushIndex - m_nPopIndex;
        if (nLength < in_nSize)
        {
            return false;
        }
        memcpy(pCopyee, &m_buffer[m_nPopIndex], in_nSize);
        m_nPopIndex = m_nPopIndex + in_nSize;
        return true;
    }
    else
    {
        int nFirstCut = 0xa0000 - m_nPopIndex;
        if (nFirstCut < in_nSize)
        {
            int nSecondCut = m_nPushIndex;
            if (m_nPushIndex + nFirstCut < in_nSize)
            {
                return false;
            }
            memcpy(pCopyee, &m_buffer[m_nPopIndex], nFirstCut);
            memcpy(pCopyee + nFirstCut, &m_buffer[0], in_nSize - nFirstCut);
            m_nPopIndex = m_nPopIndex + in_nSize;
            m_nPopIndex = in_nSize - nFirstCut;
            return true;
        }
        memcpy(pCopyee, &m_buffer[m_nPopIndex], in_nSize);
        m_nPopIndex = m_nPopIndex + in_nSize;
        if (m_nPopIndex == 0xa0000)
        {
            m_nPopIndex = 0;
        }
        return true;
    }
}

template <unsigned int Size>
bool TCircularQueueBuffer<Size>::peekCopy(int in_nSize, char* pCopyee)
{
    if (in_nSize < 1)
    {
        return false;
    }
    if (m_nPushIndex >= m_nPopIndex)
    {
        int nLength = m_nPushIndex - m_nPopIndex;
        if (nLength < in_nSize)
        {
            return false;
        }
        memcpy(pCopyee, &m_buffer[m_nPopIndex], in_nSize);
        return true;
    }
    else
    {
        int nFirstCut = 0xa0000 - m_nPopIndex;
        if (nFirstCut < in_nSize)
        {
            int nSecondCut = m_nPushIndex;
            if (m_nPushIndex + nFirstCut < in_nSize)
            {
                return false;
            }
            memcpy(pCopyee, &m_buffer[m_nPopIndex], nFirstCut);
            memcpy(pCopyee + nFirstCut, &m_buffer[0], in_nSize - nFirstCut);
            return true;
        }
        memcpy(pCopyee, &m_buffer[m_nPopIndex], in_nSize);
        return true;
    }
}

template <unsigned int Size>
bool TCircularQueueBuffer<Size>::isPopStraight(int in_nSize)
{
    if ((unsigned int)in_nSize <= 0x9ffff)
    {
        if (m_nPopIndex < m_nPushIndex)
        {
            int nSize = m_nPushIndex - m_nPopIndex;
            if (nSize < in_nSize)
            {
                return false;
            }
            return true;
        }
        else
        {
            unsigned int nFirstCutSize = 0xa0000 - m_nPopIndex;
            if (nFirstCutSize < in_nSize)
            {
                return false;
            }
            return true;
        }
    }
    return false;
}
