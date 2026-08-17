// df_monitor_r — Peer（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "Peer.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFPacketBuffer.h"
#include "DNFPacketTranslater.h"

void CPeer::operator delete(void* p, unsigned int size) { ::operator delete(p); }

int CPeer::recv_packet()
{
    if (getHandle() < 0)
    {
        return 0;
    }
    errno = 0;
    int remaining = m_data - m_buf + 0x1800;
    if (remaining == 0)
    {
        m_buf = m_data;
        m_remainLen = 0;
        remaining = 0x1800;
    }
    int n = read(getHandle(), m_buf, remaining);
    if ((char)((unsigned int)n >> 31))
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN || errno == 0)
        {
            return 0;
        }
        printf("RECV ERROR DISCONNNECT NOW FD[%d] : %d(%s)", getHandle(), errno, strerror(errno));
        return -1;
    }
    if (n == 0)
    {
        DNF_LOG_SCOPE_LINE(0xa4,"./log/TcpRecv", "Recv ERROR = 0 (%d) : %s, MaxRead(%d) nRead(%d)", errno,
            strerror(errno), remaining, n);
        return -1;
    }
    return n;
}

void CPeer::DisConnSig()
{
    Packet_InnerPakcet_Logout pkt;
    pkt.m_connNo = (unsigned int)getHandle();  // ORIG：fd 写入 pkt+6（reversed2），无独立局部
    CTcpRecvBuffer* buf;
    {
        CGuard<CMutex> guard((CMutex*)m_bLock);
        buf = new CTcpRecvBuffer;
    }
    memcpy(buf, &pkt, pkt.packetSize);
    {
        CGuard<CMutex> guard((CMutex*)m_qLock);
        ((std::queue<CTcpRecvBuffer*>*)m_recvQ)->push(buf);
    }
}

unsigned int CPeer::get_remain_sendlen() { return m_sendRemain; }

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0131 | monitor | 与ORIG差异=DIFF | CPeer::send_packet | 详见 function_reports/monitor/_ZN5CPeer11send_packetEv.md
int CPeer::send_packet()
{
    int result = 0;
    if (m_sendRemain == 0)
    {
        return 1;
    }
    else
    {
        result = write(getHandle(), m_sendBuf, m_sendRemain);
        if (result < 1)
        {
            if (errno == EAGAIN || errno == EINTR || errno == EAGAIN || errno == 0)
            {
                return 1;
            }
            else
            {
                printf("SEND ERROR DISCONNNECT NOW FD[%d] : %d(%s)", getHandle(), errno,
                       strerror(errno));
                return 1;
            }
        }
        if (0 < result)
        {
            if (result < m_sendRemain)
            {
                m_sendPtr = m_sendBuf + result;
                m_sendRemain = m_sendRemain - result;
                if ((unsigned int)m_sendRemain < 0x96001)
                {
                    memmove(m_sendBuf, m_sendPtr, m_sendRemain);
                    m_sendPtr = m_sendBuf + m_sendRemain;
                }
                else
                {
                    DNF_LOG_SCOPE_LINE(0x17e,"./log/TcpErr",
                        "m_remain_sendlen < MAX_PACKET_SIZE_UDP :  m_remain_sendlen:%d]",
                        m_sendRemain);
                    m_sendPtr = m_sendBuf;
                    m_sendRemain = 0;
                    return 1;
                }
            }
            else if (m_sendRemain < result)
            {
                printf("offset error[Remain_Data: %d Send:%d]", m_sendRemain, result);
                result = -1;
            }
            else
            {
                m_sendPtr = m_sendBuf;
                m_sendRemain = 0;
            }
        }
    }
    return result;
}

// [DNF-NONIDENTICAL] DNF-MON-NEAR-0012 | monitor | 与ORIG差异=NEAR | CPeer::send_packet | 详见 function_reports/monitor/_ZN5CPeer11send_packetEPci.md
int CPeer::send_packet(char* buf, int len)
{
    if (getHandle() < 0)
    {
        return -1;
    }
    if (len <= 0)
    {
        printf("!!!Send Packet[(%d,%d) Size(%d) Error\n", *buf, buf[1], len);
        return -1;
    }
    errno = 0;
    m_sendRemain = m_sendRemain + len;
    if ((unsigned int)m_sendRemain > 0x96000)
    {
        DNF_LOG_SCOPE_LINE(0x133,"./log/TcpErr", "!!!Send Packet Overflow P_TYPE[%d] Size:Remain[%d] Last[%d]",
            (char)buf[1], m_sendRemain, len);
        m_sendPtr = m_sendBuf;
        m_sendRemain = 0;
        return -1;
    }
    else
    {
        if (m_sendPtr < m_sendBuf || m_sendPtr >= m_sendBuf + 0x96000)
        {
            DNF_LOG_SCOPE_LINE(0x13b,"./log/TcpErr",
                "!!!Send Packet Buffer critical error P_TYPE[%d] Size:Remain[%d] Last[%d]",
                (char)buf[1], m_sendRemain, len);
            m_sendPtr = m_sendBuf;
            m_sendRemain = 0;
            return -1;
        }
        memcpy(m_sendPtr, buf, len);
        m_sendPtr = m_sendPtr + len;
        return send_packet();
    }
}

TCPSocket* CPeer::GetTcpSocket() { return this; }

void CPeer::InitPeer(std::queue<CTcpRecvBuffer*>* recvQ, CMutex* recvQLock, CMutex* recvBLock)
{
    m_recvQ = recvQ;
    m_qLock = recvQLock;
    m_bLock = recvBLock;
    m_buf = m_data;
    m_alreadyRead = 0;
    m_remainLen = 0;
    m_sendPtr = m_sendBuf;
    m_sendRemain = 0;
}

int CPeer::RecvPacket()
{
    int n = recv_packet();
    if (n > 0)
    {
        if (!parsing(n))
        {
            DNF_LOG_SCOPE_LINE(0x4d, "./log/TcpRecv", "CPeer::Recv (false == parsing( size:%d ) )", n);
            printf("CPeer::Recv (false == parsing( size:%d ) )\n", n);
            return 1;
        }
        return 1;
    }
    if (n < 0)
    {
        DNF_LOG_SCOPE_LINE(0x59,"./log/TcpRecv",
            "Maybe Peer is disconnect!(%d), socket no(%d), addr(%s), port(%d)", n,
            GetTcpSocket()->getHandle(), GetTcpSocket()->getPeerAdrs(),
            (unsigned int)GetTcpSocket()->getPeerPort() & 0xffff);
        printf("CPeer::Recv (size(%d) < 0)\n", n);
        return 0;
    }
    DNF_LOG_SCOPE_LINE(99, "./log/TcpRecv", "Maybe Peer is disconnect!(size == 0)");
    puts("CPeer::Recv (size == 0)");
    return 1;
}

void CPeer::ConnSig()
{
    Packet_InnerPakcet_Login pkt;
    pkt.m_connNo = (unsigned int)getHandle();  // ORIG：fd 写入 pkt+6（reversed2），无独立局部
    CTcpRecvBuffer* buf;
    {
        CGuard<CMutex> guard((CMutex*)m_bLock);
        buf = new CTcpRecvBuffer;
    }
    memcpy(buf, &pkt, pkt.packetSize);
    {
        CGuard<CMutex> guard((CMutex*)m_qLock);
        ((std::queue<CTcpRecvBuffer*>*)m_recvQ)->push(buf);
    }
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0132 | monitor | 与ORIG差异=DIFF | CPeer::parsing | 详见 function_reports/monitor/_ZN5CPeer7parsingEi.md
bool CPeer::parsing(int recvLen)
{
    PacketHeader header(0, 0);
    int totalLen = m_remainLen + recvLen;
    int headerSize = 10;
    if (totalLen < 10)
    {
        m_remainLen = m_remainLen + recvLen;
        m_buf = m_buf + recvLen;
        DNF_LOG_SCOPE_LINE(0xbb,"./log/TcpRecv", "(offset:%x - buf:%x) = remainlen:%d, Recv Size[%d] ",
            m_buf, m_data, m_remainLen, recvLen);
    }
    else
    {
        do
        {
            if (m_remainLen != 0)
            {
                m_buf = m_buf - m_remainLen;
            }
            memcpy(&header, m_buf, 10);
            unsigned int pktSize = (unsigned int)header.packetSize;
            if (pktSize < 10 || 0x1800 < pktSize)
            {
                DNF_LOG_SCOPE_LINE(0xd0,"./log/TcpRecv",
                    "Recv Size[%d], Parsing Packet Size[%d] is Too Large, offset:%x, buf:%x, alreadyRead:%d",
                    recvLen, pktSize, m_buf, m_data, m_alreadyRead);
                m_buf = m_data;
                m_remainLen = 0;
                return 0;
            }
            if ((unsigned int)totalLen < pktSize)
            {
                DNF_LOG_SCOPE_LINE(0x100,"./log/TcpRecv",
                    "need more data (packetsize > (unsigned int)parsinglength): body=%d !!",
                    totalLen);
                goto LAB_51773;
            }
            CTcpRecvBuffer* buf;
            {
                CGuard<CMutex> guard((CMutex*)m_bLock);
                buf = new CTcpRecvBuffer;
            }
            memcpy(buf, m_buf, pktSize);
            ((PacketHeader*)buf)->m_connNo = (unsigned int)getHandle();
            {
                CGuard<CMutex> guard((CMutex*)m_qLock);
                ((std::queue<CTcpRecvBuffer*>*)m_recvQ)->push(buf);
                int qsize = ((std::queue<CTcpRecvBuffer*>*)m_recvQ)->size();
            }
            totalLen = totalLen - pktSize;
            m_buf = m_buf + pktSize;
            m_remainLen = 0;
            if (totalLen == 0)
            {
                m_buf = m_data;
                goto LAB_51773;
            }
        } while (9 < (int)totalLen);
        {
            DNF_LOG_SCOPE_LINE(0xf8,"./log/TcpRecv", "need more data (parsinglength < HEADER_SIZE): body=%d !!",
                totalLen);
        }
LAB_51773:
        if (0 < (int)totalLen)
        {
            if (0x1800 < totalLen)
            {
                // ORIG：try 包日志块，catch 是 printf("[PARSING EXCEPTION]...")+return 0；
                // memmove 在 try 外。
                try
                {
                    DNF_LOG_SCOPE_LINE(0x10e,"./log/TcpRecv",
                        "[PARSING LENGTH EXCEPTION] parsinglength > MAX_RECV_BUF , memmove : parsinglength = %d",
                        totalLen);
                }
                catch (...)
                {
                    printf("[PARSING EXCEPTION] memmove : parsinglength = %d", totalLen);
                    return 0;
                }
                return 0;
            }
            memmove(m_data, m_buf, totalLen);
            m_remainLen = (int)totalLen;
            m_buf = m_data + totalLen;
        }
    }
    return 1;
}

CPeer::CPeer()
{
    m_buf = 0;
    m_remainLen = 0;
    m_alreadyRead = 0;
    m_recvQ = 0;
    m_bLock = 0;
    m_qLock = 0;
    m_sendRemain = 0;
    m_sendPtr = 0;
}

CPeer::~CPeer()
{
    m_buf = m_data;
    m_alreadyRead = 0;
    m_remainLen = 0;
    m_sendPtr = m_sendBuf;
    m_sendRemain = 0;
}

void* CPeer::operator new(unsigned int size) { return ::operator new(size); }

void CPeer::operator delete(void* p) { ::operator delete(p); }
