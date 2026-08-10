#ifndef TCPUSER_H_
#define TCPUSER_H_

#include "ThreadLock.h"
#include "Reactor.h"
#include "Socket.h"

// 双环形缓冲：m_buffer[2*N]@0 / m_nPushIndex@2N / m_nPopIndex@2N+4 / m_nEndIndex@2N+8
// relay 实例 N=51200，总 0x1900c
template <unsigned int N>
class TDoubleCircularQueueBuffer
{
public:
    TDoubleCircularQueueBuffer()
    {
        m_nPushIndex = 0;
        m_nPopIndex = 0;
        m_nEndIndex = N;
        memset(m_buffer, 0, sizeof(m_buffer));
    }
    ~TDoubleCircularQueueBuffer()
    {
    }
    void clear()
    {
        m_nPopIndex = 0;
        m_nPushIndex = m_nPopIndex;
        m_nEndIndex = N;
        memset(m_buffer, 0, sizeof(m_buffer));
    }
    char* peekPush()
    {
        return m_buffer + m_nPushIndex;
    }
    unsigned int getAvailableSpace() const
    {
        return N - getPushedLength();
    }
    int pushIndex(int n)
    {
        if (n > (int)N - 1)
        {
            return -1;
        }
        if (m_nPushIndex >= m_nPopIndex)
        {
            unsigned int avail = N - m_nPushIndex;
            if ((unsigned int)n < avail)
            {
                m_nPushIndex += n;
                return 0;
            }
            if ((unsigned int)n == avail)
            {
                if (m_nPopIndex != 0)
                {
                    m_nPushIndex = 0;
                    return 0;
                }
                return -2;
            }
            if (m_nPopIndex == 0)
            {
                return -3;
            }
            m_nPushIndex += n;
            m_nEndIndex = m_nPushIndex;
            m_nPushIndex = 0;
            return 0;
        }
        unsigned int len;
        if (m_nPopIndex <= (unsigned int)N - 1)
        {
            len = m_nPopIndex - m_nPushIndex;
        }
        else
        {
            len = N - m_nPushIndex;
        }
        if ((unsigned int)n >= len)
        {
            return -4;
        }
        m_nPushIndex += n;
        return 0;
    }
    bool isPopStraight(int n)
    {
        if ((unsigned int)n > (unsigned int)N - 1)
        {
            return false;
        }
        if (m_nPopIndex <= m_nPushIndex)
        {
            unsigned int len = m_nPushIndex - m_nPopIndex;
            if ((unsigned int)n > len)
            {
                return false;
            }
            return true;
        }
        unsigned int len = m_nEndIndex - m_nPopIndex;
        if ((unsigned int)n > len)
        {
            return false;
        }
        return true;
    }
    char* peekPop()
    {
        return m_buffer + m_nPopIndex;
    }
    int pop(int n)
    {
        if (n <= 0)
        {
            return -1;
        }
        if ((int)getPushedLength() < n)
        {
            return -2;
        }
        if (m_nPushIndex >= m_nPopIndex)
        {
            m_nPopIndex += n;
            return 0;
        }
        unsigned int len = m_nEndIndex - m_nPopIndex;
        if ((int)len < n)
        {
            m_nPopIndex = n - len;
            m_nEndIndex = N;
            return 0;
        }
        m_nPopIndex += n;
        if (m_nPopIndex == m_nEndIndex)
        {
            m_nEndIndex = N;
            m_nPopIndex = 0;
        }
        return 0;
    }
    unsigned int getPushedLength() const
    {
        if (m_nPushIndex >= m_nPopIndex)
        {
            return m_nPushIndex - m_nPopIndex;
        }
        return (m_nEndIndex - m_nPopIndex) + m_nPushIndex;
    }
    bool popCopy(int n, char* out)
    {
        if (!(n > 0 && (int)getPushedLength() >= n))
        {
            return false;
        }
        if (m_nPushIndex >= m_nPopIndex)
        {
            memcpy(out, m_buffer + m_nPopIndex, n);
            m_nPopIndex += n;
            return true;
        }
        unsigned int len = m_nEndIndex - m_nPopIndex;
        if ((int)len >= n)
        {
            memcpy(out, m_buffer + m_nPopIndex, n);
            m_nPopIndex += n;
            if (m_nPopIndex == m_nEndIndex)
            {
                m_nEndIndex = N;
                m_nPopIndex = 0;
            }
            return true;
        }
        memcpy(out, m_buffer + m_nPopIndex, len);
        memcpy(out + len, m_buffer, n - len);
        m_nEndIndex = N;
        m_nPopIndex = n - len;
        return true;
    }
    bool peekCopy(int n, char* out)
    {
        if (!(n > 0 && (int)getPushedLength() >= n))
        {
            return false;
        }
        if (m_nPushIndex >= m_nPopIndex)
        {
            memcpy(out, m_buffer + m_nPopIndex, n);
            return true;
        }
        unsigned int len = m_nEndIndex - m_nPopIndex;
        if ((int)len >= n)
        {
            memcpy(out, m_buffer + m_nPopIndex, n);
            return true;
        }
        memcpy(out, m_buffer + m_nPopIndex, len);
        memcpy(out + len, m_buffer, n - len);
        return true;
    }
    bool isEmpty() const
    {
        return getPushedLength() == 0;
    }
    bool isPushGreaterThanPop() const
    {
        return m_nPushIndex >= m_nPopIndex;
    }
    unsigned int getPopLengthToEnd() const
    {
        return m_nEndIndex - m_nPopIndex;
    }
    int push(char* data, int n)
    {
        if (n > (int)N - 1)
        {
            return -1;
        }
        if (m_nPushIndex >= m_nPopIndex)
        {
            unsigned int avail = N - m_nPushIndex;
            if ((unsigned int)n < avail)
            {
                memcpy(m_buffer + m_nPushIndex, data, n);
                m_nPushIndex += n;
                return 0;
            }
            if ((unsigned int)n == avail)
            {
                if (m_nPopIndex != 0)
                {
                    memcpy(m_buffer + m_nPushIndex, data, n);
                    m_nPushIndex = 0;
                    return 0;
                }
                return -2;
            }
            if (m_nPopIndex == 0)
            {
                return -3;
            }
            memcpy(m_buffer + m_nPushIndex, data, n);
            m_nPushIndex += n;
            m_nEndIndex = m_nPushIndex;
            m_nPushIndex = 0;
            return 0;
        }
        unsigned int len;
        if (m_nPopIndex <= (unsigned int)N - 1)
        {
            len = m_nPopIndex - m_nPushIndex;
        }
        else
        {
            len = N - m_nPushIndex;
        }
        if ((unsigned int)n >= len)
        {
            return -4;
        }
        memcpy(m_buffer + m_nPushIndex, data, n);
        m_nPushIndex += n;
        return 0;
    }

private:
    char m_buffer[2 * N];
    unsigned int m_nPushIndex;
    unsigned int m_nPopIndex;
    unsigned int m_nEndIndex;
};


namespace RelayServiceApp
{
struct PacketHeader;
class TCPUserStates
{
public:
    TCPUserStates()
    {
    }
};


class TCPUser : public TManager<RelayService>
{
public:
    TCPUser();
    ~TCPUser();
    unsigned int getACCID() const
    {
        return m_accId;
    }
    void setACCID(unsigned int acc_id);
    int getHandle();
    TCPSocket* getSocket();
    void setSocket(TCPSocket* sock);
    bool isAboutToDisconnect() const;
    bool isDisconnected() const;
    bool isIdle() const;
    bool isToWrite() const;
    void setLastAccessTime();
    void onRead();
    void onRead_();
    void onWrite();
    void onError();
    void onClose();
    void onAccept();
    void shutdown();
    void startupAfterSetSocket();
    void onPacketParse();
    int postSendPacket(char* buf);
    int send(PacketHeader* buf);
    void postDisconnected(int flag);
    void notifyCannotLoginByMaxUserCount();

    TCPUserStates m_states;                          // +4
    unsigned int m_accId;                            // +8
    int m_kind;                                      // +0xc = 4
    bool m_isDisconnected;                           // +0x10（onClose 置位）
    bool m_isAboutToDisconnect;                      // +0x11（postDisconnected 置位）
    long long m_lastAccessTime;                      // +0x14
    TCPSocket* m_sock;                               // +0x1c
    TDoubleCircularQueueBuffer<51200u> m_recvQueue;  // +0x20
    TDoubleCircularQueueBuffer<51200u> m_sendQueue;  // +0x1902c
};


} // namespace RelayServiceApp

#endif // TCPUSER_H_
