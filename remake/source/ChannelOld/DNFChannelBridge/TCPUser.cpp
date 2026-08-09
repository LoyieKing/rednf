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
    GLOG(ChannelServiceApp::gFileLogInfo, "In postDisconnected =" << nReason);
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
    if (tickLast_ == 0 || gap <= 0x124f80)
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
    GLOG(ChannelServiceApp::gFileLogInfo, "call onClose from " << file << ", " << line << ", TCPUSER=" << getACCID()
        << ", error=" << strerror(*__errno_location()));
    if (bDisconnected_)
        return;
    getManager()->UserPools::destroyTCPUser(this, "TCPUser.cpp", 0x6a);
    if (pSock_ != NULL)
    {
        pSock_->close();
    }
    bDisconnected_ = true;
}

void ChannelServiceApp::TCPUser::onRead(char* file, int line)
{
    if (pSock_->getHandle() < 0)
        return;
    if (isAboutToDisconnect() || isDisconnected())
        return;
    TScopedLock<TThreadLock<ThreadLock_linux> > rlock(lockRecv_);
    onRead_();
}

void ChannelServiceApp::TCPUser::onRead_()
{
    char buf[0xa0000];
    int nRead = pSock_->recv(buf, 0xa0000);
    GLOG(ChannelServiceApp::gFileLogInfo, "TCPUser.cpp" << " read=" << nRead);
    if (nRead >= 1)
    {
        int nRet = bufferRecv_.push(buf, nRead, "TCPUser.cpp", 0x96);
        if (nRet < 0)
        {
            postDisconnected(0);
            return;
        }
    }
    else if (nRead < 0)
    {
        postDisconnected(1);
        return;
    }
    do
    {
        if ((unsigned int)bufferRecv_.getPopLengthToEnd() > 0xa)
        {
            char* s = (char*)bufferRecv_.peekPop();
            tagPacketHeader* hdr = (tagPacketHeader*)s;
            int nMessageSize = hdr->getSize();
            if ((0xa0000 < nMessageSize) || (nMessageSize < 1))
            {
                postDisconnected(2);
                return;
            }
            if (nMessageSize >= 1)
            {
                if (bufferRecv_.isPopStraight(nMessageSize))
                {
                    TManager<ChannelService>::getManager()->getTCPHandlerRelay()->dispatch(this, s, nMessageSize, 0);
                    if (bufferRecv_.pop(nMessageSize) < 0)
                    {
                        GLOG(ChannelServiceApp::gFileLogInfo, "1.\xbf\xa9\xb1\xe2\xbc\xad pop error \x20\xb0\xa1 \xb6\xb3\xbe\xee\xc1\xf6\xb8\xe9 \xbe\xc8\xb5\xc8\xb4\xd9.");
                        postDisconnected(3);
                        return;
                    }
                }
                else
                {
                    if (bufferRecv_.getPushedLength() >= nMessageSize)
                    {
                        char* szBuf = (char*)malloc(nMessageSize);
                        if (bufferRecv_.popCopy(nMessageSize, szBuf))
                        {
                            TManager<ChannelService>::getManager()->getTCPHandlerRelay()->dispatch(this, szBuf, nMessageSize, 0);
                            free(szBuf);
                        }
                        else
                        {
                            free(szBuf);
                            GLOG(ChannelServiceApp::gFileLogInfo, "1.\xbf\xa9\xb1\xe2\xbc\xad pop error \x20\xb0\xa1 \xb6\xb3\xbe\xee\xc1\xf6\xb8\xe9 \xbe\xc8\xb5\xc8\xb4\xd9.");
                            postDisconnected(4);
                            return;
                        }
                    }
                    else
                    {
                        GLOG(ChannelServiceApp::gFileLogInfo, "0.need more body data : messagesize=" << nMessageSize);
                        return;
                    }
                }
            }
            else
            {
                GLOG(ChannelServiceApp::gFileLogInfo, "recv critical error occure!");
                postDisconnected(5);
                return;
            }
        }
        else
        {
            if ((unsigned int)bufferRecv_.getPushedLength() < 0xb)
            {
                return;
            }
            char s[12];
            if (bufferRecv_.peekCopy(0xb, s))
            {
                tagPacketHeader* hdr = (tagPacketHeader*)s;
                int nMessageSize = hdr->getSize();
                if ((0xa0000 < nMessageSize) || (nMessageSize < 1))
                {
                    postDisconnected(6);
                    return;
                }
                if (nMessageSize >= 1)
                {
                    if (bufferRecv_.getPushedLength() >= nMessageSize)
                    {
                        char* szBuf = (char*)malloc(nMessageSize);
                        if (bufferRecv_.popCopy(nMessageSize, szBuf))
                        {
                            TManager<ChannelService>::getManager()->getTCPHandlerRelay()->dispatch(this, szBuf, nMessageSize, 0);
                            free(szBuf);
                        }
                        else
                        {
                            free(szBuf);
                            GLOG(ChannelServiceApp::gFileLogInfo, "1.\xbf\xa9\xb1\xe2\xbc\xad pop error \x20\xb0\xa1 \xb6\xb3\xbe\xee\xc1\xf6\xb8\xe9 \xbe\xc8\xb5\xc8\xb4\xd9.");
                            postDisconnected(7);
                            return;
                        }
                    }
                    else
                    {
                        GLOG(ChannelServiceApp::gFileLogInfo, "2.need more body data : messagesize=" << nMessageSize);
                        return;
                    }
                }
                else
                {
                    GLOG(ChannelServiceApp::gFileLogInfo, "2.recv critical error occure!");
                    postDisconnected(8);
                    return;
                }
            }
            else
            {
                GLOG(ChannelServiceApp::gFileLogInfo, "3.recv critical error occure!");
                postDisconnected(9);
                return;
            }
        }
    } while (!bufferRecv_.isEmpty());
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
        GLOG(ChannelServiceApp::gFileLogInfo, "onWrite_ : Error(1)");
    }
    else if (0 > pSock_->getHandle())
    {
        GLOG(ChannelServiceApp::gFileLogInfo, "onWrite_ : Error(2)");
    }
    else if (isAboutToDisconnect() || isDisconnected())
    {
        GLOG(ChannelServiceApp::gFileLogInfo, "onWrite_ : Error(3)");
    }
    else if (bufferSend_.isEmpty() != 0)
    {
        GLOG(ChannelServiceApp::gFileLogInfo, "onWrite_ : Error(4)");
    }
    else
    {
        do
        {
            GLOG(ChannelServiceApp::gFileLogInfo, "start~~ " << count);
            count = count + 1;
            if (bufferSend_.isPushGreaterThanPop())
            {
                int nSize = bufferSend_.getPushedLength();
                if (nSize >= 1)
                {
                    if (pSock_ == NULL)
                    {
                        return;
                    }
                    int nSent = pSock_->send(bufferSend_.peekPop(), nSize);
                    if (nSent >= 1)
                    {
                        if (bufferSend_.pop(nSent) < 0)
                        {
                            postDisconnected(10);
                            GLOG(ChannelServiceApp::gFileLogInfo, "AAA 2.pop \xbf\xa1 \xbd\xc7\xc6\xd0\xb8\xe9 \xb9\xae\xc1\xa6\xc0\xd6\xb4\xd9.");
                            return;
                        }
                        GLOG(ChannelServiceApp::gFileLogInfo, "1.AAA pop \xbc\xba\xb0\xf8, " << nSent);
                    }
                    else
                    {
                        if (nSent == 0)
                        {
                            GLOG(ChannelServiceApp::gFileLogInfo, "AAA send \xbd\xc7\xc6\xd0, \xbc\xd2\xc4\xcf \xc0\xcc\xba\xa5\xc6\xae \xbf\xa9\xc0\xfc\xc8\xf7 \xc1\xb8\xc0\xe7, \xb4\xd9\xc0\xbd \xb1\xe2\xc8\xb8\xbf\xa1 .. ");
                            return;
                        }
                        GLOG(ChannelServiceApp::gFileLogInfo, "AAA send \xbd\xc7\xc6\xd0, disconnect.. count =" << count);
                        postDisconnected(0xb);
                        return;
                    }
                }
                else
                {
                    GLOG(ChannelServiceApp::gFileLogInfo, "AAA \xba\xf1\xc1\xa4\xbb\xf3 send queue \xb9\xdf\xbb\xfd nSize=" << nSize);
                    postDisconnected(0xc);
                    return;
                }
            }
            else
            {
                int nSize = bufferSend_.getPopLengthToEnd();
                if (nSize >= 1)
                {
                    if (pSock_ == NULL)
                    {
                        return;
                    }
                    int nSent = pSock_->send(bufferSend_.peekPop(), nSize);
                    if (nSent >= 1)
                    {
                        if (bufferSend_.pop(nSent) < 0)
                        {
                            GLOG(ChannelServiceApp::gFileLogInfo, "AAA 3.pop \xbf\xa1 \xbd\xc7\xc6\xd0\xb8\xe9 \xb9\xae\xc1\xa6\xc0\xd6\xb4\xd9.");
                            postDisconnected(0xd);
                            return;
                        }
                        GLOG(ChannelServiceApp::gFileLogInfo, "AAA pop \xbc\xba\xb0\xf8, " << nSent);
                    }
                    else
                    {
                        if (nSent == 0)
                        {
                            GLOG(ChannelServiceApp::gFileLogInfo, "AAA send \xbd\xc7\xc6\xd0, \xbc\xd2\xc4\xcf \xc0\xcc\xba\xa5\xc6\xae \xbf\xa9\xc0\xfc\xc8\xf7 \xc1\xb8\xc0\xe7, \xb4\xd9\xc0\xbd \xb1\xe2\xc8\xb8\xbf\xa1 .. ");
                            return;
                        }
                        GLOG(ChannelServiceApp::gFileLogInfo, "2.AAA send \xbd\xc7\xc6\xd0, disconnect.. count=" << count);
                        postDisconnected(0xe);
                        return;
                    }
                }
                else
                {
                    GLOG(ChannelServiceApp::gFileLogInfo, "AAA \xba\xf1\xc1\xa4\xbb\xf3 send queue \xb9\xdf\xbb\xfd  nSize=" << nSize);
                    postDisconnected(0xf);
                    return;
                }
            }
            GLOG(ChannelServiceApp::gFileLogInfo, "send empty? =" << bufferSend_.isEmpty());
        } while (bufferSend_.isEmpty() == 0);
    }
}

int ChannelServiceApp::TCPUser::onWrite2Buffer(CMsgCell* pMsgCell)
{
    if (pSock_ == NULL)
    {
        GLOG(ChannelServiceApp::gFileLogInfo, "Send Queue Push error = -1");
        return -1;
    }
    if (pSock_->getHandle() < 0)
    {
        GLOG(ChannelServiceApp::gFileLogInfo, "Send Queue Push error = -1");
        return -2;
    }
    if (isAboutToDisconnect() || isDisconnected())
    {
        GLOG(ChannelServiceApp::gFileLogInfo, "Send Queue Push error = -1");
        return -3;
    }
    if (pMsgCell->GetSize() <= 0)
    {
        GLOG(ChannelServiceApp::gFileLogInfo, "Send Queue Push error = -1");
        return -4;
    }
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockSend_);
    if (bufferSend_.getPushedLength() + pMsgCell->GetSize() > 0x9ffff)
    {
        printf("bufferSend_.getPushedLength() : %d, pMsgCell->GetSize() : %d\n",
               bufferSend_.getPushedLength(), pMsgCell->GetSize());
        return -5;
    }
    int nRet = bufferSend_.push(pMsgCell->GetBuf(), pMsgCell->GetSize(), "TCPUser.cpp", 0x213);
    if (nRet < 0)
    {
        GLOG(ChannelServiceApp::gFileLogInfo, "Send Queue Push error=" << nRet);
        return -6;
    }
    return nRet;
}

int ChannelServiceApp::TCPUser::send(CMsgCell* pMsgCell)
{
    if (pSock_ == NULL)
    {
        return -1;
    }
    if (pSock_->getHandle() < 0)
    {
        return -2;
    }
    if (isAboutToDisconnect() || isDisconnected())
    {
        return -3;
    }
    if (pMsgCell->GetSize() <= 0)
    {
        return -4;
    }
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockSend_);
    if (bufferSend_.getPushedLength() + pMsgCell->GetSize() > 0x9ffff)
    {
        return -5;
    }
    int nRet = bufferSend_.push(pMsgCell->GetBuf(), pMsgCell->GetSize(), "TCPUser.cpp", 0x1e4);
    if (nRet < 0)
    {
        GLOG(ChannelServiceApp::gFileLogInfo, "Send Queue Push error=" << nRet);
        return -6;
    }
    else
    {
        onWrite_();
        return 0;
    }
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
    if ((unsigned int)in_nSize > 0x9ffff)
    {
        return -1;
    }
    if (m_nPushIndex >= m_nPopIndex)
    {
        unsigned int nFirstCutSize = 0xa0000 - m_nPushIndex;
        unsigned int nSecondCutSize = m_nPopIndex;
        if (in_nSize < nFirstCutSize)
        {
            memcpy(m_buffer + m_nPushIndex, in_block, in_nSize);
            m_nPushIndex = m_nPushIndex + in_nSize;
        }
        else if (in_nSize == nFirstCutSize)
        {
            if (m_nPopIndex == 0)
            {
                return -2;
            }
            memcpy(m_buffer + m_nPushIndex, in_block, in_nSize);
            m_nPushIndex = m_nPushIndex + in_nSize;
            if (m_nPushIndex == 0xa0000)
            {
                m_nPushIndex = 0;
            }
        }
        else
        {
            if ((unsigned int)in_nSize < nFirstCutSize + nSecondCutSize)
            {
                memcpy(m_buffer + m_nPushIndex, in_block, nFirstCutSize);
                memcpy(m_buffer, in_block + nFirstCutSize, in_nSize - nFirstCutSize);
                m_nPushIndex = in_nSize - nFirstCutSize;
            }
            else
            {
                return -3;
            }
        }
    }
    else
    {
        unsigned int nLength = m_nPopIndex - m_nPushIndex;
        if (nLength > in_nSize)
        {
            memcpy(m_buffer + m_nPushIndex, in_block, in_nSize);
            m_nPushIndex = m_nPushIndex + in_nSize;
        }
        else
        {
            return -4;
        }
    }
    return 0;
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
        int nLength = (int)(m_nPushIndex - m_nPopIndex);
        if (nLength >= in_nSize)
        {
            m_nPopIndex = m_nPopIndex + in_nSize;
            return 0;
        }
        return -2;
    }
    else
    {
        int nFirstCut = 0xa0000 - m_nPopIndex;
        if (nFirstCut >= in_nSize)
        {
            m_nPopIndex = m_nPopIndex + in_nSize;
            if (m_nPopIndex == 0xa0000)
            {
                m_nPopIndex = 0;
            }
            return 0;
        }
        int nPushIndexTemp = (int)m_nPushIndex;
        if (nPushIndexTemp + nFirstCut >= in_nSize)
        {
            m_nPopIndex = in_nSize - nFirstCut;
            return 0;
        }
        return -3;
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
        int nLength = (int)(m_nPushIndex - m_nPopIndex);
        if (nLength >= in_nSize)
        {
            memcpy(pCopyee, m_buffer + m_nPopIndex, in_nSize);
            m_nPopIndex = m_nPopIndex + in_nSize;
            return true;
        }
        return false;
    }
    else
    {
        int nFirstCut = 0xa0000 - m_nPopIndex;
        if (nFirstCut >= in_nSize)
        {
            memcpy(pCopyee, m_buffer + m_nPopIndex, in_nSize);
            m_nPopIndex = m_nPopIndex + in_nSize;
            if (m_nPopIndex == 0xa0000)
            {
                m_nPopIndex = 0;
            }
            return true;
        }
        int nSecondCut = m_nPushIndex;
        if (nSecondCut + nFirstCut >= in_nSize)
        {
            memcpy(pCopyee, m_buffer + m_nPopIndex, nFirstCut);
            memcpy(pCopyee + nFirstCut, m_buffer, in_nSize - nFirstCut);
            m_nPopIndex = m_nPopIndex + in_nSize;
            m_nPopIndex = in_nSize - nFirstCut;
            return true;
        }
        return false;
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
        int nLength = (int)(m_nPushIndex - m_nPopIndex);
        if (nLength >= in_nSize)
        {
            memcpy(pCopyee, m_buffer + m_nPopIndex, in_nSize);
            return true;
        }
        return false;
    }
    else
    {
        int nFirstCut = 0xa0000 - m_nPopIndex;
        if (nFirstCut >= in_nSize)
        {
            memcpy(pCopyee, m_buffer + m_nPopIndex, in_nSize);
            return true;
        }
        int nSecondCut = m_nPushIndex;
        if (nSecondCut + nFirstCut >= in_nSize)
        {
            memcpy(pCopyee, m_buffer + m_nPopIndex, nFirstCut);
            memcpy(pCopyee + nFirstCut, m_buffer, in_nSize - nFirstCut);
            return true;
        }
        return false;
    }
}

template <unsigned int Size>
bool TCircularQueueBuffer<Size>::isPopStraight(int in_nSize)
{
    GLOG(ChannelServiceApp::gFileLogInfo, "In  isPopStraight");
    GLOG(ChannelServiceApp::gFileLogInfo, "in_nSize=" << in_nSize);
    GLOG(ChannelServiceApp::gFileLogInfo, "m_nPopIndex=" << m_nPopIndex);
    GLOG(ChannelServiceApp::gFileLogInfo, "m_nPushIndex=" << m_nPushIndex);
    if ((unsigned int)in_nSize > 0x9ffff)
    {
        return false;
    }
    if (m_nPopIndex < m_nPushIndex)
    {
        int nSize = m_nPushIndex - m_nPopIndex;
        if (nSize >= in_nSize)
        {
            return true;
        }
    }
    else
    {
        unsigned int nFirstCutSize = 0xa0000 - m_nPopIndex;
        if (in_nSize <= nFirstCutSize)
        {
            return true;
        }
        return false;
    }
    GLOG(ChannelServiceApp::gFileLogInfo, "Out isPopStraight");
    return false;
}
