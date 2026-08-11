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
    *(void**)((char*)this + 0x181c) = 0;
    *(int*)((char*)this + 0x1820) = 0;
    *(int*)((char*)this + 0x1824) = 0;
    *(void**)((char*)this + 0x1828) = 0;
    *(void**)((char*)this + 0x182c) = 0;
    *(void**)((char*)this + 0x1830) = 0;
    *(int*)((char*)this + 0x1834) = 0;
    *(void**)((char*)this + 0x1838) = 0;
}

CPeer::~CPeer()
{
}

void CPeer::InitPeer(std::queue<CTcpRecvBuffer*>* q, CMutex* lock1, CMutex* lock2)
{
    *(void**)((char*)this + 0x1828) = q;
    *(void**)((char*)this + 0x1830) = lock1;
    *(void**)((char*)this + 0x182c) = lock2;
    *(void**)((char*)this + 0x181c) = (char*)this + 0x1c;
    *(int*)((char*)this + 0x1824) = 0;
    *(int*)((char*)this + 0x1820) = 0;
    *(void**)((char*)this + 0x1838) = (char*)this + 0x183c;
    *(int*)((char*)this + 0x1834) = 0;
}

int CPeer::RecvPacket()
{
    int ret = recv_packet();
    if (ret > 0)
    {
        if (!parsing(ret))
        {
            DNF_LOG_SCOPE_LINE(0x4d, "./log/TcpRecv",
                "CPeer::Recv (false == parsing( size:%d ) )\n", ret);
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
    int remaining = ((char*)this + 0x1c + 0x1800) - *(char**)((char*)this + 0x181c);
    if (remaining == 0)
    {
        *(char**)((char*)this + 0x181c) = (char*)this + 0x1c;
        *(int*)((char*)this + 0x1820) = 0;
        remaining = 0x1800;
    }
    int n = read(getHandle(), *(void**)((char*)this + 0x181c), remaining);
    if (n < 0)
    {
        if (errno == EAGAIN || errno == EINTR)
            return 0;
        if (errno != 0)
        {
            printf("RECV ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
                   getHandle(), errno, strerror(errno));
            return -1;
        }
        return 0;
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

int CPeer::parsing(int len)
{
    int parsinglength = *(int*)((char*)this + 0x1820) + len;
    if (parsinglength <= 9)
    {
        *(int*)((char*)this + 0x1820) += len;
        *(char**)((char*)this + 0x181c) += len;
        DNF_LOG_SCOPE_LINE(0xbb, "./log/TcpRecv",
            "(offset:%x - buf:%x) = remainlen:%d, Recv Size[%d] ",
            (char*)this + 0x1c, *(char**)((char*)this + 0x181c),
            *(int*)((char*)this + 0x1820), len);
        return 1;
    }
    for (;;)
    {
        if (*(int*)((char*)this + 0x1820) != 0)
            *(char**)((char*)this + 0x181c) -= *(int*)((char*)this + 0x1820);
        PacketHeader hdr(0, 0);
        memcpy(&hdr, *(void**)((char*)this + 0x181c), 10);
        int size = hdr.packetSize;
        if (size <= 9 || size > 0x1800)
        {
            DNF_LOG_SCOPE_LINE(0xd0, "./log/TcpRecv",
                "Recv Size[%d], Parsing Packet Size[%d] is Too Large, offset:%x, buf:%x, alreadyRead:%d",
                len, size, *(char**)((char*)this + 0x181c), (char*)this + 0x1c,
                *(int*)((char*)this + 0x1824));
            *(char**)((char*)this + 0x181c) = (char*)this + 0x1c;
            *(int*)((char*)this + 0x1820) = 0;
            return 0;
        }
        if (parsinglength < size)
        {
            DNF_LOG_SCOPE_LINE(0x100, "./log/TcpRecv",
                "need more data (packetsize > (unsigned int)parsinglength): body=%d !!",
                parsinglength);
            break;
        }
        CTcpRecvBuffer* buf;
        {
            CGuard<CMutex> guard(*(CMutex**)((char*)this + 0x182c));
            buf = new CTcpRecvBuffer;
        }
        memcpy(buf, *(void**)((char*)this + 0x181c), size);
        *(int*)((char*)buf + 6) = getHandle();
        {
            CGuard<CMutex> guard(*(CMutex**)((char*)this + 0x1830));
            (*(std::queue<CTcpRecvBuffer*>**)((char*)this + 0x1828))->push(buf);
        }
        parsinglength -= size;
        *(char**)((char*)this + 0x181c) += size;
        *(int*)((char*)this + 0x1820) = 0;
        if (parsinglength == 0)
        {
            *(char**)((char*)this + 0x181c) = (char*)this + 0x1c;
            break;
        }
        if (parsinglength <= 9)
        {
            DNF_LOG_SCOPE_LINE(0xf8, "./log/TcpRecv",
                "need more data (parsinglength < HEADER_SIZE): body=%d !!",
                parsinglength);
            break;
        }
    }
    if (parsinglength > 0)
    {
        if (parsinglength > 0x1800)
        {
            DNF_LOG_SCOPE_LINE(0x10e, "./log/TcpRecv",
                "[PARSING LENGTH EXCEPTION] parsinglength > MAX_RECV_BUF , memmove : parsinglength = %d",
                parsinglength);
            return 0;
        }
        memmove((char*)this + 0x1c, *(void**)((char*)this + 0x181c), parsinglength);
        *(int*)((char*)this + 0x1820) = parsinglength;
        *(char**)((char*)this + 0x181c) = (char*)this + 0x1c + parsinglength;
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
    *(int*)((char*)this + 0x1834) += len;
    if (*(unsigned int*)((char*)this + 0x1834) < 0x96001)
    {
        if (*(CPeer**)((char*)this + 0x1838) < (CPeer*)((char*)this + 0x183c) ||
            (CPeer*)((char*)this + 0x9783c) <= *(CPeer**)((char*)this + 0x1838))
        {
            int remain = *(int*)((char*)this + 0x1834);
            DNF_LOG_SCOPE_LINE(0x13b,"./log/TcpErr",
                "!!!Send Packet Buffer critical error P_TYPE[%d] Size:Remain[%d] Last[%d]",
                (int)buf[1], remain, len);
            *(CPeer**)((char*)this + 0x1838) = (CPeer*)((char*)this + 0x183c);
            *(int*)((char*)this + 0x1834) = 0;
            return -1;
        }
        memcpy(*(void**)((char*)this + 0x1838), buf, (size_t)len);
        *(int*)((char*)this + 0x1838) += len;
        return send_packet();
    }
    int remain = *(int*)((char*)this + 0x1834);
    DNF_LOG_SCOPE_LINE(0x133,"./log/TcpErr", "!!!Send Packet Overflow P_TYPE[%d] Size:Remain[%d] Last[%d]",
        (int)buf[1], remain, len);
    *(CPeer**)((char*)this + 0x1838) = (CPeer*)((char*)this + 0x183c);
    *(int*)((char*)this + 0x1834) = 0;
    return -1;
}

int CPeer::send_packet()
{
    ssize_t r = 0;
    if (*(int*)((char*)this + 0x1834) == 0)
    {
        r = 1;
    }
    else
    {
        size_t n = *(size_t*)((char*)this + 0x1834);
        int fd = getHandle();
        r = write(fd, (char*)this + 0x183c, n);
        if (r < 1)
        {
            if (errno == EAGAIN || errno == EINTR || errno == 0)
            {
                r = 1;
            }
            else
            {
                printf("SEND ERROR DISCONNNECT NOW FD[%d] : %d(%s)", fd, errno, strerror(errno));
                r = 1;
            }
        }
        else if (r > 0)
        {
            if (r < *(int*)((char*)this + 0x1834))
            {
                *(CPeer**)((char*)this + 0x1838) = (CPeer*)((char*)this + 0x183c + r);
                *(int*)((char*)this + 0x1834) -= (int)r;
                if (*(unsigned int*)((char*)this + 0x1834) < 0x96001)
                {
                    memmove((char*)this + 0x183c, *(void**)((char*)this + 0x1838),
                            *(size_t*)((char*)this + 0x1834));
                    *(CPeer**)((char*)this + 0x1838) = (CPeer*)((char*)this + 0x183c);
                    *(int*)((char*)this + 0x1834) = 0;
                    r = 1;
                }
                else
                {
                    CMyFileLog log("send_packet", 0x17e);
                    log("./log/TcpErr",
                        "m_remain_sendlen < MAX_PACKET_SIZE_UDP :  m_remain_sendlen:%d]",
                        *(int*)((char*)this + 0x1834));
                }
            }
            else if (*(int*)((char*)this + 0x1834) < r)
            {
                printf("offset error[Remain_Data: %d Send:%d]",
                       *(int*)((char*)this + 0x1834), (int)r);
                r = -1;
            }
            else
            {
                *(CPeer**)((char*)this + 0x1838) = (CPeer*)((char*)this + 0x183c);
                *(int*)((char*)this + 0x1834) = 0;
            }
        }
    }
    return (int)r;
}

void CPeer::DisConnSig()
{
    Packet_InnerPakcet_Logout pkt;
    int fd = getHandle();
    CTcpRecvBuffer* buf;
    {
        CGuard<CMutex> guard(*(CMutex**)((char*)this + 0x182c));
        buf = new CTcpRecvBuffer;
    }
    memcpy(buf, &pkt, pkt.packetSize);
    {
        CGuard<CMutex> guard(*(CMutex**)((char*)this + 0x1830));
        (*(std::queue<CTcpRecvBuffer*>**)((char*)this + 0x1828))->push(buf);
    }
}

void CPeer::ConnSig()
{
    Packet_InnerPakcet_Login pkt;
    int fd = getHandle();
    CTcpRecvBuffer* buf;
    {
        CGuard<CMutex> guard(*(CMutex**)((char*)this + 0x182c));
        buf = new CTcpRecvBuffer;
    }
    memcpy(buf, &pkt, pkt.packetSize);
    {
        CGuard<CMutex> guard(*(CMutex**)((char*)this + 0x1830));
        (*(std::queue<CTcpRecvBuffer*>**)((char*)this + 0x1828))->push(buf);
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
    return m_remainSendLen;
}
