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
    ChannelServiceApp::gFileLogInfo.Lock();
    ChannelServiceApp::gFileLogInfo << "In postDisconnected =" << nReason << endl;
    ChannelServiceApp::gFileLogInfo.Unlock();
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
    if (tickLast_ == 0 || gap < 0 || (gap < 1 && (unsigned int)gap < 0x124f81))
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
    ChannelServiceApp::gFileLogInfo.Lock();
    ChannelServiceApp::gFileLogInfo << "call onClose from " << file << ", " << line << ", TCPUSER=" << getACCID()
                 << ", error=" << strerror(*__errno_location()) << endl;
    ChannelServiceApp::gFileLogInfo.Unlock();
    if (bDisconnected_ == false)
    {
        getManager()->UserPools::destroyTCPUser(this, "TCPUser.cpp", 0x6a);
        if (pSock_ != NULL)
        {
            pSock_->close();
        }
        bDisconnected_ = true;
    }
}

void ChannelServiceApp::TCPUser::onRead(char* file, int line)
{
    if (-1 < pSock_->getHandle())
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
    ChannelServiceApp::gFileLogInfo.Lock();
    ChannelServiceApp::gFileLogInfo << "TCPUser.cpp" << " read=" << nRead << endl;
    ChannelServiceApp::gFileLogInfo.Unlock();
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
        int nRet = bufferRecv_.push(buf, nRead, "TCPUser.cpp", 0x96);
        if (nRet < 0)
        {
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
                return;
            }
            tagPacketHeader hdr;
            if (!bufferRecv_.peekCopy(0xb, (char*)&hdr))
            {
                ChannelServiceApp::gFileLogInfo.Lock();
                ChannelServiceApp::gFileLogInfo << "3.recv critical error occure!" << endl;
                ChannelServiceApp::gFileLogInfo.Unlock();
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
                ChannelServiceApp::gFileLogInfo.Lock();
                ChannelServiceApp::gFileLogInfo << "2.recv critical error occure!" << endl;
                ChannelServiceApp::gFileLogInfo.Unlock();
                postDisconnected(8);
                return;
            }
            if (bufferRecv_.getPushedLength() < nMessageSize)
            {
                ChannelServiceApp::gFileLogInfo.Lock();
                ChannelServiceApp::gFileLogInfo << "2.need more body data : messagesize=" << nMessageSize << endl;
                ChannelServiceApp::gFileLogInfo.Unlock();
                return;
            }
            char szBuf[nMessageSize];
            if (!bufferRecv_.popCopy(nMessageSize, szBuf))
            {
                ChannelServiceApp::gFileLogInfo.Lock();
                ChannelServiceApp::gFileLogInfo << "1.\xbf\xa9\xb1\xe2\xbc\xad pop error \x20\xb0\xa1 \xb6\xb3\xbe\xee\xc1\xf6\xb8\xe9 \xbe\xc8\xb5\xc8\xb4\xd9." << endl;
                ChannelServiceApp::gFileLogInfo.Unlock();
                postDisconnected(7);
                return;
            }
            TManager<ChannelService>::getManager()->getTCPHandlerRelay()->dispatch(this, szBuf, nMessageSize, 0);
        }
        else
        {
            tagPacketHeader* hdr = (tagPacketHeader*)bufferRecv_.peekPop();
            int nMessageSize = hdr->getSize();
            if ((0xa0000 < nMessageSize) || (nMessageSize < 1))
            {
                postDisconnected(2);
                return;
            }
            if (nMessageSize < 1)
            {
                ChannelServiceApp::gFileLogInfo.Lock();
                ChannelServiceApp::gFileLogInfo << "recv critical error occure!" << endl;
                ChannelServiceApp::gFileLogInfo.Unlock();
                postDisconnected(5);
                return;
            }
            if (bufferRecv_.isPopStraight(nMessageSize))
            {
                TManager<ChannelService>::getManager()->getTCPHandlerRelay()->dispatch(this, (char*)hdr, nMessageSize, 0);
                if (bufferRecv_.pop(nMessageSize) < 0)
                {
                    ChannelServiceApp::gFileLogInfo.Lock();
                    gFileLogInfo << "1.\xbf\xa9\xb1\xe2\xbc\xad pop error \x20\xb0\xa1 \xb6\xb3\xbe\xee\xc1\xf6\xb8\xe9 \xbe\xc8\xb5\xc8\xb4\xd9." << endl;
                    ChannelServiceApp::gFileLogInfo.Unlock();
                    postDisconnected(3);
                    return;
                }
            }
            else
            {
            if (bufferRecv_.getPushedLength() < nMessageSize)
            {
                ChannelServiceApp::gFileLogInfo.Lock();
                ChannelServiceApp::gFileLogInfo << "0.need more body data : messagesize=" << nMessageSize << endl;
                ChannelServiceApp::gFileLogInfo.Unlock();
                return;
            }
            char szBuf[nMessageSize];
            if (!bufferRecv_.popCopy(nMessageSize, szBuf))
            {
                ChannelServiceApp::gFileLogInfo.Lock();
                ChannelServiceApp::gFileLogInfo << "1.\xbf\xa9\xb1\xe2\xbc\xad pop error \x20\xb0\xa1 \xb6\xb3\xbe\xee\xc1\xf6\xb8\xe9 \xbe\xc8\xb5\xc8\xb4\xd9." << endl;
                ChannelServiceApp::gFileLogInfo.Unlock();
                    postDisconnected(4);
                    return;
            }
            TManager<ChannelService>::getManager()->getTCPHandlerRelay()->dispatch(this, szBuf, nMessageSize, 0);
        }
    }
    if (bufferRecv_.isEmpty() != 0)
    {
        return;
    }
    } while (true);
}

void ChannelServiceApp::TCPUser::onWrite(char* file, int line)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockSend_);
    onWrite_();
}

void ChannelServiceApp::TCPUser::onWrite_()
{
    int count = 0;
    if (pSock_ == NULL)
    {
        ChannelServiceApp::gFileLogInfo.Lock();
        gFileLogInfo << "onWrite_ : Error(1)" << endl;
        ChannelServiceApp::gFileLogInfo.Unlock();
    }
    else if (0 > pSock_->getHandle())
    {
        ChannelServiceApp::gFileLogInfo.Lock();
        gFileLogInfo << "onWrite_ : Error(2)" << endl;
        ChannelServiceApp::gFileLogInfo.Unlock();
    }
    else if (isAboutToDisconnect() || isDisconnected())
    {
        ChannelServiceApp::gFileLogInfo.Lock();
        gFileLogInfo << "onWrite_ : Error(3)" << endl;
        ChannelServiceApp::gFileLogInfo.Unlock();
    }
    else if (bufferSend_.isEmpty() == 0)
    {
        do
        {
            ChannelServiceApp::gFileLogInfo.Lock();
            gFileLogInfo << "start~~ " << count << endl;
            ChannelServiceApp::gFileLogInfo.Unlock();
            count = count + 1;
            if (bufferSend_.isPushGreaterThanPop())
            {
                int nSize = bufferSend_.getPushedLength();
                if (nSize < 1)
                {
                    ChannelServiceApp::gFileLogInfo.Lock();
                    gFileLogInfo << "AAA \xba\xf1\xc1\xa4\xbb\xf3 send queue \xb9\xdf\xbb\xfd nSize=" << nSize << endl;
                    ChannelServiceApp::gFileLogInfo.Unlock();
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
                        ChannelServiceApp::gFileLogInfo.Lock();
                        gFileLogInfo << "AAA send \xbd\xc7\xc6\xd0, \xbc\xd2\xc4\xcf \xc0\xcc\xba\xa5\xc6\xae \xbf\xa9\xc0\xfc\xc8\xf7 \xc1\xb8\xc0\xe7, \xb4\xd9\xc0\xbd \xb1\xe2\xc8\xb8\xbf\xa1 .. " << endl;
                        ChannelServiceApp::gFileLogInfo.Unlock();
                        return;
                    }
                    ChannelServiceApp::gFileLogInfo.Lock();
                    gFileLogInfo << "AAA send \xbd\xc7\xc6\xd0, disconnect.. count =" << count << endl;
                    ChannelServiceApp::gFileLogInfo.Unlock();
                    postDisconnected(0xb);
                    return;
                }
                if (bufferSend_.pop(nSent) < 0)
                {
                    postDisconnected(10);
                    ChannelServiceApp::gFileLogInfo.Lock();
                    gFileLogInfo << "AAA 2.pop \xbf\xa1 \xbd\xc7\xc6\xd0\xb8\xe9 \xb9\xae\xc1\xa6\xc0\xd6\xb4\xd9." << endl;
                    ChannelServiceApp::gFileLogInfo.Unlock();
                    return;
                }
                ChannelServiceApp::gFileLogInfo.Lock();
                ChannelServiceApp::gFileLogInfo << "1.AAA pop \xbc\xba\xb0\xf8, " << nSent << endl;
                ChannelServiceApp::gFileLogInfo.Unlock();
            }
            else
            {
                int nSize = bufferSend_.getPopLengthToEnd();
                if (nSize < 1)
                {
                    ChannelServiceApp::gFileLogInfo.Lock();
                    gFileLogInfo << "AAA \xba\xf1\xc1\xa4\xbb\xf3 send queue \xb9\xdf\xbb\xfd  nSize=" << nSize << endl;
                    ChannelServiceApp::gFileLogInfo.Unlock();
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
                        ChannelServiceApp::gFileLogInfo.Lock();
                        gFileLogInfo << "AAA send \xbd\xc7\xc6\xd0, \xbc\xd2\xc4\xcf \xc0\xcc\xba\xa5\xc6\xae \xbf\xa9\xc0\xfc\xc8\xf7 \xc1\xb8\xc0\xe7, \xb4\xd9\xc0\xbd \xb1\xe2\xc8\xb8\xbf\xa1 .. " << endl;
                        ChannelServiceApp::gFileLogInfo.Unlock();
                        return;
                    }
                    ChannelServiceApp::gFileLogInfo.Lock();
                    gFileLogInfo << "2.AAA send \xbd\xc7\xc6\xd0, disconnect.. count=" << count << endl;
                    ChannelServiceApp::gFileLogInfo.Unlock();
                    postDisconnected(0xe);
                    return;
                }
                if (bufferSend_.pop(nSent) < 0)
                {
                    ChannelServiceApp::gFileLogInfo.Lock();
                    gFileLogInfo << "AAA 3.pop \xbf\xa1 \xbd\xc7\xc6\xd0\xb8\xe9 \xb9\xae\xc1\xa6\xc0\xd6\xb4\xd9." << endl;
                    ChannelServiceApp::gFileLogInfo.Unlock();
                    postDisconnected(0xd);
                    return;
                }
                ChannelServiceApp::gFileLogInfo.Lock();
                ChannelServiceApp::gFileLogInfo << "AAA pop \xbc\xba\xb0\xf8, " << nSent << endl;
                ChannelServiceApp::gFileLogInfo.Unlock();
            }
            ChannelServiceApp::gFileLogInfo.Lock();
            gFileLogInfo << "send empty? =" << bufferSend_.isEmpty() << endl;
            ChannelServiceApp::gFileLogInfo.Unlock();
        } while (bufferSend_.isEmpty() == 0);
    }
    else
    {
        ChannelServiceApp::gFileLogInfo.Lock();
        gFileLogInfo << "onWrite_ : Error(4)" << endl;
        ChannelServiceApp::gFileLogInfo.Unlock();
    }
}

int ChannelServiceApp::TCPUser::onWrite2Buffer(CMsgCell* pMsgCell)
{
    int iVar5;
    if (pSock_ == NULL)
    {
        ChannelServiceApp::gFileLogInfo.Lock();
        gFileLogInfo << "Send Queue Push error = -1" << endl;
        ChannelServiceApp::gFileLogInfo.Unlock();
        iVar5 = -1;
    }
    else
    {
        SOCKET h = pSock_->getHandle();
        if (h < 0)
        {
            ChannelServiceApp::gFileLogInfo.Lock();
            gFileLogInfo << "Send Queue Push error = -1" << endl;
            ChannelServiceApp::gFileLogInfo.Unlock();
            iVar5 = -2;
        }
        else if (isAboutToDisconnect() || isDisconnected())
        {
            ChannelServiceApp::gFileLogInfo.Lock();
            gFileLogInfo << "Send Queue Push error = -1" << endl;
            ChannelServiceApp::gFileLogInfo.Unlock();
            iVar5 = -3;
        }
        else if (pMsgCell->GetSize() < 1)
        {
            ChannelServiceApp::gFileLogInfo.Lock();
            gFileLogInfo << "Send Queue Push error = -1" << endl;
            ChannelServiceApp::gFileLogInfo.Unlock();
            iVar5 = -4;
        }
        else
        {
            TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockSend_);
            if (bufferSend_.getPushedLength() + pMsgCell->GetSize() < 0xa0000)
            {
                iVar5 = bufferSend_.push(pMsgCell->GetBuf(), pMsgCell->GetSize(), "TCPUser.cpp", 0x213);
                if (iVar5 < 0)
                {
                    ChannelServiceApp::gFileLogInfo.Lock();
                    gFileLogInfo << "Send Queue Push error=" << iVar5 << endl;
                    ChannelServiceApp::gFileLogInfo.Unlock();
                    iVar5 = -6;
                }
            }
            else
            {
                printf("bufferSend_.getPushedLength() : %d, pMsgCell->GetSize() : %d\n",
                       bufferSend_.getPushedLength(), pMsgCell->GetSize());
                iVar5 = -5;
            }
        }
    }
    return iVar5;
}

int ChannelServiceApp::TCPUser::send(CMsgCell* pMsgCell)
{
    if (pSock_ == NULL)
    {
        return -1;
    }
    if (-1 < pSock_->getHandle())
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
            int nRet = bufferSend_.push(pMsgCell->GetBuf(), pMsgCell->GetSize(), "TCPUser.cpp", 0x1e4);
            if (nRet < 0)
            {
                ChannelServiceApp::gFileLogInfo.Lock();
                ChannelServiceApp::gFileLogInfo << "Send Queue Push error=" << nRet << endl;
                ChannelServiceApp::gFileLogInfo.Unlock();
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
    if (in_nSize <= 0x9ffff)
    {
        if (m_nPushIndex < m_nPopIndex)
        {
            if (m_nPopIndex - m_nPushIndex <= (unsigned int)in_nSize)
            {
                return -4;
            }
            memcpy(&m_buffer[m_nPushIndex], in_block, in_nSize);
            m_nPushIndex = m_nPushIndex + in_nSize;
        }
        else
        {
            unsigned int nFirstCutSize = 0xa0000 - m_nPushIndex;
            if ((unsigned int)in_nSize < nFirstCutSize)
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
                if (nFirstCutSize + m_nPopIndex <= (unsigned int)in_nSize)
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
    if (m_nPushIndex < m_nPopIndex)
    {
        int nFirstCut = 0xa0000 - m_nPopIndex;
        if (nFirstCut < in_nSize)
        {
            if ((int)(m_nPushIndex + nFirstCut) < in_nSize)
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
    if ((int)(m_nPushIndex - m_nPopIndex) < in_nSize)
    {
        return -2;
    }
    m_nPopIndex = m_nPopIndex + in_nSize;
    return 0;
}

template <unsigned int Size>
bool TCircularQueueBuffer<Size>::popCopy(int in_nSize, char* pCopyee)
{
    if (in_nSize < 1)
    {
        return false;
    }
    if (m_nPushIndex < m_nPopIndex)
    {
        unsigned int nFirstCut = 0xa0000 - m_nPopIndex;
        if ((int)nFirstCut < in_nSize)
        {
            if ((int)(m_nPushIndex + nFirstCut) < in_nSize)
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
    if ((int)(m_nPushIndex - m_nPopIndex) < in_nSize)
    {
        return false;
    }
    memcpy(pCopyee, &m_buffer[m_nPopIndex], in_nSize);
    m_nPopIndex = m_nPopIndex + in_nSize;
    return true;
}

template <unsigned int Size>
bool TCircularQueueBuffer<Size>::peekCopy(int in_nSize, char* pCopyee)
{
    if (in_nSize < 1)
    {
        return false;
    }
    if (m_nPushIndex < m_nPopIndex)
    {
        unsigned int nFirstCut = 0xa0000 - m_nPopIndex;
        if ((int)nFirstCut < in_nSize)
        {
            if ((int)(m_nPushIndex + nFirstCut) < in_nSize)
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
    if ((int)(m_nPushIndex - m_nPopIndex) < in_nSize)
    {
        return false;
    }
    memcpy(pCopyee, &m_buffer[m_nPopIndex], in_nSize);
    return true;
}

template <unsigned int Size>
bool TCircularQueueBuffer<Size>::isPopStraight(int in_nSize)
{
    ChannelServiceApp::gFileLogInfo.Lock();
    ChannelServiceApp::gFileLogInfo << "In  isPopStraight" << endl;
    ChannelServiceApp::gFileLogInfo.Unlock();
    ChannelServiceApp::gFileLogInfo.Lock();
    ChannelServiceApp::gFileLogInfo << "in_nSize=" << in_nSize << endl;
    ChannelServiceApp::gFileLogInfo.Unlock();
    ChannelServiceApp::gFileLogInfo.Lock();
    ChannelServiceApp::gFileLogInfo << "m_nPopIndex=" << m_nPopIndex << endl;
    ChannelServiceApp::gFileLogInfo.Unlock();
    ChannelServiceApp::gFileLogInfo.Lock();
    ChannelServiceApp::gFileLogInfo << "m_nPushIndex=" << m_nPushIndex << endl;
    ChannelServiceApp::gFileLogInfo.Unlock();
    if (in_nSize <= 0x9ffff)
    {
        if (m_nPopIndex < m_nPushIndex)
        {
            int nSize = m_nPushIndex - m_nPopIndex;
            if (nSize < in_nSize)
            {
                ChannelServiceApp::gFileLogInfo.Lock();
                ChannelServiceApp::gFileLogInfo << "Out isPopStraight" << endl;
                ChannelServiceApp::gFileLogInfo.Unlock();
                return false;
            }
            return true;
        }
        else
        {
            unsigned int nFirstCutSize = 0xa0000 - m_nPopIndex;
            if (nFirstCutSize < (unsigned int)in_nSize)
            {
                return false;
            }
            return true;
        }
    }
    return false;
}
