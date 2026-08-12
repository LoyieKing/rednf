// df_guild_r — Peer (split from source/guild per ORIG layout)
#include <string.h>
#include <time.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include "tinyxml.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel_Next.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel_Next.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include <stdio.h>
#include <utility>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>

#include "Peer.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFGameServer.h"
#include "DNFGuild.h"
#include "DNFGuildManager.h"
#include "DNFGuildServerMain.h"
#include "DNFGuildWar.h"
#include "DNFInnerMsgHandler.h"
#include "DNFKillUserConfig.h"
#include "DNFManagerServer.h"
#include "DNFMonitorServer.h"
#include "DNFNetworkThread.h"
#include "DNFPacketBuffer.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTracer.h"
#include "DNFPacketTranslater.h"
#include "DNFPowerWarConfig.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFSignal.h"
#include "DNFSignalTranslator.h"
#include "DNFSystemFailSig.h"
#include "DNFTableBase.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "DNFTerminateSig.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"
#include "DNFUserManager.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "GuildCargo.h"
#include "MemPool.h"
#include "MemoryCashManager.h"
#include "PacketCounter.h"
#include "Power.h"
#include "PowerManager.h"
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

void* CPeer::operator new(unsigned int size)
{
    return m_PeerMemPool_.alloc();
}

void CPeer::operator delete(void* p)
{
    m_PeerMemPool_.free(p);
}

void CPeer::operator delete(void* p, unsigned int size)
{
    m_PeerMemPool_.free(p, size);
}

MemPool<CPeer> m_PeerMemPool_(1000);

__attribute__((weak)) int getErrno()
{
    return errno;
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
    m_buf = (char*)this + 0x1c;
    m_alreadyRead = 0;
    m_remainLen = 0;
    m_sendPtr = (char*)this + 0x183c;
    m_sendRemain = 0;
}

void CPeer::InitPeer(std::queue<CTcpRecvBuffer*>* q, CMutex* lock1, CMutex* lock2)
{
    m_recvQ = q;
    m_qLock = lock1;
    m_bLock = lock2;
    m_buf = (char*)this + 0x1c;
    m_alreadyRead = 0;
    m_remainLen = 0;
    m_sendPtr = (char*)this + 0x183c;
    m_sendRemain = 0;
}

bool CPeer::RecvPacket()
{
    int ret = recv_packet();
    if (ret > 0)
    {
        if (!parsing(ret))
        {
            DNF_LOG_SCOPE_LINE(0x4d, "./log/TcpRecv",
                "CPeer::Recv (false == parsing( size:%d ) )", ret);
            printf("CPeer::Recv (false == parsing( size:%d ) )\n", ret);
            return 1;
        }
        return 1;
    }
    if (ret < 0)
    {
        DNF_LOG_SCOPE_LINE(0x59, "./log/TcpRecv",
            "Maybe Peer is disconnect!(%d), socket no(%d), addr(%s), port(%d)",
            ret, GetTcpSocket()->getHandle(), GetTcpSocket()->getPeerAdrs(),
            GetTcpSocket()->getPeerPort());
        printf("CPeer::Recv (size(%d) < 0)\n", ret);
        return 0;
    }
    DNF_LOG_SCOPE_LINE(0x63, "./log/TcpRecv", "Maybe Peer is disconnect!(size == 0)");
    puts("CPeer::Recv (size == 0)");
    return 1;
}

int CPeer::recv_packet()
{
    if (getHandle() < 0)
        return 0;
    errno = 0;
    int remaining = ((char*)this + 0x1c) - m_buf + 0x1800;
    if (remaining == 0)
    {
        m_buf = (char*)this + 0x1c;
        m_remainLen = 0;
        remaining = 0x1800;
    }
    int n = read(getHandle(), m_buf, remaining);
    if (n < 0)
    {
        if (errno == EAGAIN)
            return 0;
        if (errno == EINTR)
            return 0;
        if (errno == EAGAIN)
            return 0;
        if (errno == 0)
            return 0;
        printf("RECV ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
               getHandle(), errno, strerror(errno));
        return -1;
    }
    if (n == 0)
    {
        DNF_LOG_SCOPE_LINE(0xa4, "./log/TcpRecv",
            "Recv ERROR = 0 (%d) : %s, MaxRead(%d) nRead(%d)",
            errno, strerror(errno), remaining, n);
        return -1;
    }
    return n;
}

bool CPeer::parsing(int len)
{
    PacketHeader hdr(0, 0);
    int parsinglength = m_remainLen + len;
    int size = 10;
    if (parsinglength <= 9)
    {
        m_remainLen += len;
        m_buf += len;
        DNF_LOG_SCOPE_LINE(0xbb, "./log/TcpRecv",
            "(offset:%x - buf:%x) = remainlen:%d, Recv Size[%d] ",
            m_buf, (char*)this + 0x1c, m_remainLen, len);
        return 1;
    }
    do
    {
        if (m_remainLen != 0)
            m_buf -= m_remainLen;
        memcpy(&hdr, m_buf, 10);
        unsigned int packetSize = hdr.packetSize;
        if (packetSize <= 9 || packetSize > 0x1800)
        {
            DNF_LOG_SCOPE_LINE(0xd0, "./log/TcpRecv",
                "Recv Size[%d], Parsing Packet Size[%d] is Too Large, offset:%x, buf:%x, alreadyRead:%d",
                len, packetSize, m_buf, (char*)this + 0x1c, m_alreadyRead);
            m_buf = (char*)this + 0x1c;
            m_remainLen = 0;
            return 0;
        }
        if ((unsigned int)parsinglength < packetSize)
        {
            DNF_LOG_SCOPE_LINE(0x100, "./log/TcpRecv",
                "need more data (packetsize > (unsigned int)parsinglength): body=%d !!",
                parsinglength);
            break;
        }
        CTcpRecvBuffer* buf;
        {
            CGuard<CMutex> guard(*(CMutex**)&m_bLock);
            buf = new CTcpRecvBuffer;
        }
        memcpy(buf, m_buf, packetSize);
        *(int*)((char*)buf + 6) = getHandle();
        {
            CGuard<CMutex> guard(*(CMutex**)&m_qLock);
            ((std::queue<CTcpRecvBuffer*>*)m_recvQ)->push(buf);
            int qsize = (int)((std::queue<CTcpRecvBuffer*>*)m_recvQ)->size();
        }
        parsinglength -= packetSize;
        m_buf += packetSize;
        m_remainLen = 0;
        if (parsinglength == 0)
        {
            m_buf = (char*)this + 0x1c;
            break;
        }
    } while (parsinglength > 9);
    if (parsinglength <= 9)
    {
        DNF_LOG_SCOPE_LINE(0xf8, "./log/TcpRecv",
            "need more data (parsinglength < HEADER_SIZE): body=%d !!",
            parsinglength);
    }
    if (parsinglength > 0)
    {
        if ((unsigned int)parsinglength > 0x1800)
        {
            DNF_LOG_SCOPE_LINE(0x10e, "./log/TcpRecv",
                "[PARSING LENGTH EXCEPTION] parsinglength > MAX_RECV_BUF , memmove : parsinglength = %d",
                parsinglength);
            return 0;
        }
        try
        {
            memmove((char*)this + 0x1c, m_buf, parsinglength);
            m_remainLen = parsinglength;
            m_buf = (char*)this + 0x1c + parsinglength;
        }
        catch (...)
        {
            printf("[PARSING EXCEPTION] memmove : parsinglength = %d", parsinglength);
            return 0;
        }
    }
    return 1;
}

int CPeer::send_packet(char* buf, int len)
{
    if (getHandle() < 0)
    {
        return -1;
    }
    if (len < 1)
    {
        printf("!!!Send Packet[(%d,%d) Size(%d) Error\n", (int)buf[0], (int)buf[1], len);
        return -1;
    }
    errno = 0;
    m_sendRemain += len;
    if ((unsigned int)m_sendRemain > 0x96000)
    {
        DNF_LOG_SCOPE_LINE(0x133, "./log/TcpErr",
            "!!!Send Packet Overflow P_TYPE[%d] Size:Remain[%d] Last[%d]",
            (int)buf[1], m_sendRemain, len);
        m_sendPtr = (char*)this + 0x183c;
        m_sendRemain = 0;
        return -1;
    }
    if (m_sendPtr < m_sendBuf || m_sendPtr >= m_sendBuf + 0x96000)
    {
        DNF_LOG_SCOPE_LINE(0x13b, "./log/TcpErr",
            "!!!Send Packet Buffer critical error P_TYPE[%d] Size:Remain[%d] Last[%d]",
            (int)buf[1], m_sendRemain, len);
        m_sendPtr = (char*)this + 0x183c;
        m_sendRemain = 0;
        return -1;
    }
    memcpy(m_sendPtr, buf, (size_t)len);
    m_sendPtr += len;
    return send_packet();
}

int CPeer::send_packet()
{
    ssize_t ret = 0;
    if (m_sendRemain == 0)
        return 1;
    ret = write(getHandle(), (char*)this + 0x183c, m_sendRemain);
    if (ret < 1)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN || errno == 0)
            return 1;
        printf("SEND ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
               getHandle(), errno, strerror(errno));
        return 1;
    }
    if (ret > 0)
    {
        if (ret < m_sendRemain)
        {
            m_sendPtr = (char*)this + 0x183c + ret;
            m_sendRemain -= (int)ret;
            if ((unsigned int)m_sendRemain <= 0x96000)
            {
                memmove((char*)this + 0x183c, m_sendPtr, m_sendRemain);
                m_sendPtr = (char*)this + 0x183c + m_sendRemain;
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x17e, "./log/TcpErr",
                    "m_remain_sendlen < MAX_PACKET_SIZE_UDP :  m_remain_sendlen:%d]",
                    m_sendRemain);
                m_sendPtr = (char*)this + 0x183c;
                m_sendRemain = 0;
                return 1;
            }
        }
        else if (m_sendRemain < ret)
        {
            printf("offset error[Remain_Data: %d Send:%d]", m_sendRemain, (int)ret);
            return -1;
        }
        else
        {
            m_sendPtr = (char*)this + 0x183c;
            m_sendRemain = 0;
        }
    }
    return (int)ret;
}

void CPeer::DisConnSig()
{
    int fd = getHandle();
    Packet_InnerPakcet_Logout pkt;
    CTcpRecvBuffer* buf;
    {
        CGuard<CMutex> guard(*(CMutex**)&m_bLock);
        buf = new CTcpRecvBuffer;
    }
    memcpy(buf, &pkt, pkt.packetSize);
    {
        CGuard<CMutex> guard(*(CMutex**)&m_qLock);
        ((std::queue<CTcpRecvBuffer*>*)m_recvQ)->push(buf);
    }
}

void CPeer::ConnSig()
{
    int fd = getHandle();
    Packet_InnerPakcet_Login pkt;
    CTcpRecvBuffer* buf;
    {
        CGuard<CMutex> guard(*(CMutex**)&m_bLock);
        buf = new CTcpRecvBuffer;
    }
    memcpy(buf, &pkt, pkt.packetSize);
    {
        CGuard<CMutex> guard(*(CMutex**)&m_qLock);
        ((std::queue<CTcpRecvBuffer*>*)m_recvQ)->push(buf);
    }
}

TCPSocket* CPeer::GetTcpSocket()
{
    return this;
}

Packet_InnerPakcet_Login::Packet_InnerPakcet_Login()
    : PacketHeader(0xfa0, 0xa)
{
    (void)0;
}

Packet_InnerPakcet_Logout::Packet_InnerPakcet_Logout()
    : PacketHeader(0xfa1, 0xa)
{
    (void)0;
}

int CPeer::get_remain_sendlen()
{
    return m_sendRemain;
}
