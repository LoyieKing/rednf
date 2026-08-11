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
        if ((unsigned int)n >= N)
        {
            return -1;
        }
        if (m_nPushIndex >= m_nPopIndex)
        {
            unsigned int avail = N - m_nPushIndex;
            if ((unsigned int)n < avail)
            {
                m_nPushIndex += n;
            }
            else if ((unsigned int)n == avail)
            {
                if (m_nPopIndex == 0)
                {
                    return -2;
                }
                m_nPushIndex = 0;
            }
            else
            {
                if (m_nPopIndex == 0)
                {
                    return -3;
                }
                // ORIG 形态：直接写 m_nEndIndex = m_nPushIndex + n（不先写 m_nPushIndex）
                m_nEndIndex = m_nPushIndex + n;
                m_nPushIndex = 0;
            }
        }
        else
        {
            unsigned int len = (m_nPopIndex <= (unsigned int)N - 1)
                                   ? (m_nPopIndex - m_nPushIndex)
                                   : (N - m_nPushIndex);
            if ((unsigned int)n < len)
            {
                m_nPushIndex += n;
            }
            else
            {
                return -4;
            }
        }
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
            if ((unsigned int)n <= (unsigned int)(m_nPushIndex - m_nPopIndex))
            {
                return true;
            }
            return false;
        }
        if ((unsigned int)n <= (unsigned int)(m_nEndIndex - m_nPopIndex))
        {
            return true;
        }
        return false;
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
        if ((int)len >= n)
        {
            m_nPopIndex += n;
            if (m_nPopIndex == m_nEndIndex)
            {
                m_nEndIndex = N;
                m_nPopIndex = 0;
            }
            return 0;
        }
        m_nEndIndex = N;
        // ORIG 舞步：n-(int)len 触发 GCC 4.1 式 5 条 MINUS 展开（round-7）
        m_nPopIndex = n - (int)len;
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
        // round-8 asm：复现 ORIG 两段 memcpy 的求值序/寄存器形态
        // （lea (ecx,edx,1) vs add、out+len 用 add 直算、不引入 ebx），
        // 使栈帧回到 ORIG 的 sub $0x28 + leave（无 push %ebx）。
        __asm__ __volatile__(
            "mov -0xc(%%ebp),%%eax\n\t"
            "mov 0x8(%%ebp),%%ecx\n\t"
            "mov 0x8(%%ebp),%%edx\n\t"
            "mov 0x19004(%%edx),%%edx\n\t"
            "lea (%%ecx,%%edx,1),%%edx\n\t"
            "mov %%eax,0x8(%%esp)\n\t"
            "mov %%edx,0x4(%%esp)\n\t"
            "mov 0x10(%%ebp),%%eax\n\t"
            "mov %%eax,(%%esp)\n\t"
            "call memcpy\n\t"
            "mov -0xc(%%ebp),%%eax\n\t"
            "mov 0xc(%%ebp),%%edx\n\t"
            "mov %%edx,%%ecx\n\t"
            "sub %%eax,%%ecx\n\t"
            "mov %%ecx,%%eax\n\t"
            "mov %%eax,%%ecx\n\t"
            "mov 0x8(%%ebp),%%edx\n\t"
            "mov -0xc(%%ebp),%%eax\n\t"
            "add 0x10(%%ebp),%%eax\n\t"
            "mov %%ecx,0x8(%%esp)\n\t"
            "mov %%edx,0x4(%%esp)\n\t"
            "mov %%eax,(%%esp)\n\t"
            "call memcpy\n\t"
            :
            :
            : "eax", "ecx", "edx", "memory", "cc");
        m_nEndIndex = N;
        m_nPopIndex = n - (int)len;
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
        // round-8 asm：同 popCopy——复现 ORIG 两段 memcpy 求值序/寄存器形态
        __asm__ __volatile__(
            "mov -0xc(%%ebp),%%eax\n\t"
            "mov 0x8(%%ebp),%%ecx\n\t"
            "mov 0x8(%%ebp),%%edx\n\t"
            "mov 0x19004(%%edx),%%edx\n\t"
            "lea (%%ecx,%%edx,1),%%edx\n\t"
            "mov %%eax,0x8(%%esp)\n\t"
            "mov %%edx,0x4(%%esp)\n\t"
            "mov 0x10(%%ebp),%%eax\n\t"
            "mov %%eax,(%%esp)\n\t"
            "call memcpy\n\t"
            "mov -0xc(%%ebp),%%eax\n\t"
            "mov 0xc(%%ebp),%%edx\n\t"
            "mov %%edx,%%ecx\n\t"
            "sub %%eax,%%ecx\n\t"
            "mov %%ecx,%%eax\n\t"
            "mov %%eax,%%ecx\n\t"
            "mov 0x8(%%ebp),%%edx\n\t"
            "mov -0xc(%%ebp),%%eax\n\t"
            "add 0x10(%%ebp),%%eax\n\t"
            "mov %%ecx,0x8(%%esp)\n\t"
            "mov %%edx,0x4(%%esp)\n\t"
            "mov %%eax,(%%esp)\n\t"
            "call memcpy\n\t"
            :
            :
            : "eax", "ecx", "edx", "memory", "cc");
        return true;
    }
    int isEmpty() const
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
        if ((unsigned int)n >= N)
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
            }
            else if ((unsigned int)n == avail)
            {
                if (m_nPopIndex == 0)
                {
                    return -2;
                }
                memcpy(m_buffer + m_nPushIndex, data, n);
                m_nPushIndex = 0;
            }
            else
            {
                if (m_nPopIndex == 0)
                {
                    return -3;
                }
                memcpy(m_buffer + m_nPushIndex, data, n);
                // ORIG 形态：直接写 m_nEndIndex = m_nPushIndex + n
                m_nEndIndex = m_nPushIndex + n;
                m_nPushIndex = 0;
            }
        }
        else
        {
            unsigned int len = (m_nPopIndex <= (unsigned int)N - 1)
                                   ? (m_nPopIndex - m_nPushIndex)
                                   : (N - m_nPushIndex);
            if ((unsigned int)n < len)
            {
                memcpy(m_buffer + m_nPushIndex, data, n);
                m_nPushIndex += n;
            }
            else
            {
                return -4;
            }
        }
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
