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
    m_buf = m_data;
    m_alreadyRead = 0;
    m_remainLen = 0;
    m_sendPtr = m_sendBuf;
    m_sendRemain = 0;
}

// ORIG 证据（guild df_guild_r）：`_ZN5CPeer7ConnSigEv` T @0x08051c06、
// `_ZN5CPeer10DisConnSigEv` T @0x08051b1c、`_ZN5CPeer12GetTcpSocketEv` W @0x08051da0、
// `_ZN24Packet_InnerPakcet_LoginC1Ev` / `_ZN25Packet_InnerPakcet_LogoutC1Ev` W @0x08051de6。
// ConnSig/DisConnSig 的 pkt 局部槽为 6 字节（packetId+packetSize+reversed1），ctor 仍写满
// 10 字节（尾部越界到 fd 槽后被覆盖）——与 DBMW/Manager 同源（那边已 identical）。
extern "C" void _ZN24Packet_InnerPakcet_LoginC1Ev(void*);
extern "C" void _ZN25Packet_InnerPakcet_LogoutC1Ev(void*);

Packet_InnerPakcet_Login::Packet_InnerPakcet_Login()
    : PacketHeader(0xfa0, 0xa)
{
}

Packet_InnerPakcet_Logout::Packet_InnerPakcet_Logout()
    : PacketHeader(0xfa1, 0xa)
{
}

void CPeer::ConnSig()
{
    struct PktLocal {
        unsigned short packetId;
        unsigned short packetSize;
        unsigned short reversed1;
    } __attribute__((packed));
    PktLocal pkt;
    _ZN24Packet_InnerPakcet_LoginC1Ev(&pkt);
    int fd = getHandle();
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

void CPeer::DisConnSig()
{
    struct PktLocal {
        unsigned short packetId;
        unsigned short packetSize;
        unsigned short reversed1;
    } __attribute__((packed));
    PktLocal pkt;
    _ZN25Packet_InnerPakcet_LogoutC1Ev(&pkt);
    int fd = getHandle();
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

TCPSocket* CPeer::GetTcpSocket()
{
    return this;
}

void CPeer::InitPeer(std::queue<CTcpRecvBuffer*>* q, CMutex* lock1, CMutex* lock2)
{
    m_recvQ = q;
    m_qLock = lock1;
    m_bLock = lock2;
    m_buf = m_data;
    m_alreadyRead = 0;
    m_remainLen = 0;
    m_sendPtr = m_sendBuf;
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0106 已消除） | guild | CPeer::recv_packet | 详见 function_reports/guild/_ZN5CPeer11recv_packetEv.md
int CPeer::recv_packet()
{
    if (getHandle() < 0)
        return 0;
    errno = 0;
    int remaining = m_data - m_buf + 0x1800;
    if (remaining == 0)
    {
        m_buf = m_data;
        m_remainLen = 0;
        remaining = 0x1800;
    }
    int n;
    if ((n = read(getHandle(), m_buf, remaining)) < 0)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN || errno == 0)
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

// [2026-09-15 W3-GuildRest 机理 N 命中] 交换 `int size` / `unsigned int packetSize` 声明序
//   （packetSize 先）⇒ 槽位对齐 ORIG（size@-0x1c、packetSize@-0x20；原我方 size@-0x20）
//   ⇒ ndiff 44→**35** ✓ / 长度仍 288 vs 287。残余 = 尾部 `mov $0x0,%ebx; jmp` 块布局族
//   （`packetSize<=9` 与 `>0x1800` 两出口的块合并方式）⇒ 块尾/条件布局族，未 identical。
// [DNF-NONIDENTICAL] DNF-GLD-DIFF-0108 | guild | 与ORIG差异=DIFF | CPeer::parsing | 详见 function_reports/guild/_ZN5CPeer7parsingEi.md
bool CPeer::parsing(int len)
{
    int qsize;
    PacketHeader hdr(0, 0);
    int parsinglength = m_remainLen + len;
    // [机理 N] ORIG 槽序：size@-0x1c、packetSize@-0x20 ⇒ packetSize 先声明
    unsigned int packetSize;
    int size = 10;
    if (parsinglength <= 9)
    {
        m_remainLen = m_remainLen + len;
        m_buf += len;
        DNF_LOG_SCOPE_LINE(0xbb, "./log/TcpRecv",
            "(offset:%x - buf:%x) = remainlen:%d, Recv Size[%d] ",
            m_buf, m_data, m_remainLen, len);
        return 1;
    }
parsing_loop:
    {
        if (m_remainLen != 0)
            m_buf -= m_remainLen;
        memcpy(&hdr, m_buf, 10);
        packetSize = hdr.packetSize;
        if (packetSize <= 9 || packetSize > 0x1800)
        {
            DNF_LOG_SCOPE_LINE(0xd0, "./log/TcpRecv",
                "Recv Size[%d], Parsing Packet Size[%d] is Too Large, offset:%x, buf:%x, alreadyRead:%d",
                len, packetSize, m_buf, m_data, m_alreadyRead);
            m_buf = m_data;
            m_remainLen = 0;
            return 0;
        }
        if ((unsigned int)parsinglength < packetSize)
            goto log_need_more;
        CTcpRecvBuffer* buf;
        {
            CGuard<CMutex> guard((CMutex*)m_bLock);
            buf = new CTcpRecvBuffer;
        }
        memcpy(buf, m_buf, packetSize);
        ((PacketHeader*)buf)->m_connNo = getHandle();
        {
            CGuard<CMutex> guard((CMutex*)m_qLock);
            ((std::queue<CTcpRecvBuffer*>*)m_recvQ)->push(buf);
            qsize = (int)((std::queue<CTcpRecvBuffer*>*)m_recvQ)->size();
        }
        parsinglength -= packetSize;
        m_buf += packetSize;
        m_remainLen = 0;
        if (parsinglength == 0)
        {
            m_buf = m_data;
            // ORIG 0x805153b：`m_buf = m_data` 之后 **jmp +0x39f**，也就是**跳过**
            // 0xf8 那条 "need more data (parsinglength < HEADER_SIZE): body=%d"。
            // 语义：buffer 里的包**正好整批消费完**（body=0）属于正常情形，不该报
            // need-more-data；只有真的剩下 1..9 字节的半截头才报。
            // 我们原来写成 break，会落到 0xf8 判断块里，于是每轮多打 body=0 的日志
            // （整机 A/B 实测：guild/log/TcpRecv 比 ORIG 多 3 行）。
            goto parsing_after_header_size_log;
        }
    }
    if (parsinglength > 9) goto parsing_loop;
    DNF_LOG_SCOPE_LINE(0xf8, "./log/TcpRecv",
        "need more data (parsinglength < HEADER_SIZE): body=%d !!",
        parsinglength);
    goto parsing_after_header_size_log;
log_need_more:
    DNF_LOG_SCOPE_LINE(0x100, "./log/TcpRecv",
        "need more data (packetsize > (unsigned int)parsinglength): body=%d !!",
        parsinglength);
parsing_after_header_size_log:
    // ORIG +0x39f：上面两个 break 分支真正落地的地方（跳过 0xf8 日志块）。
    if (parsinglength > 0)
    {
        if ((unsigned int)parsinglength <= 0x1800)
        {
            memmove(m_data, m_buf, parsinglength);
            m_remainLen = parsinglength;
            m_buf = m_data + parsinglength;
        }
        else
        {
            // ORIG：try 包日志块（CMyFileLog 构造/调用），catch 是
            // printf("[PARSING EXCEPTION] memmove...") + return 0。
            try
            {
                DNF_LOG_SCOPE_LINE(0x10e, "./log/TcpRecv",
                    "[PARSING LENGTH EXCEPTION] parsinglength > MAX_RECV_BUF , memmove : parsinglength = %d",
                    parsinglength);
            }
            catch (...)
            {
                printf("[PARSING EXCEPTION] memmove : parsinglength = %d", parsinglength);
                return 0;
            }
            return 0;
        }
    }
    return 1;
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-NEAR-0011 已消除） | guild | CPeer::send_packet | 详见 function_reports/guild/_ZN5CPeer11send_packetEPci.md
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
        m_sendPtr = m_sendBuf;
        m_sendRemain = 0;
        return -1;
    }
    if (m_sendPtr < m_sendBuf || m_sendPtr >= m_sendBuf + 0x96000)
    {
        DNF_LOG_SCOPE_LINE(0x13b, "./log/TcpErr",
            "!!!Send Packet Buffer critical error P_TYPE[%d] Size:Remain[%d] Last[%d]",
            (int)buf[1], m_sendRemain, len);
        m_sendPtr = m_sendBuf;
        m_sendRemain = 0;
        return -1;
    }
    memcpy(m_sendPtr, buf, (size_t)len);
    m_sendPtr += len;
    return send_packet();
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0107 已消除） | guild | CPeer::send_packet | 详见 function_reports/guild/_ZN5CPeer11send_packetEv.md
int CPeer::send_packet()
{
    int ret = 0;
    char pad[0x10];
    if (m_sendRemain == 0)
        return 1;
    if ((ret = write(getHandle(), m_sendBuf, m_sendRemain)) <= 0)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN || errno == 0)
            return 1;
        else
        {
            printf("SEND ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
                   getHandle(), errno, strerror(errno));
            return 1;
        }
    }
    if (ret > 0)
    {
        if ((int)m_sendRemain > ret)
        {
            m_sendPtr = m_sendBuf + ret;
            m_sendRemain -= ret;
            if (m_sendRemain > 0x96000u)
            {
                DNF_LOG_SCOPE_LINE(0x17e, "./log/TcpErr",
                    "m_remain_sendlen < MAX_PACKET_SIZE_UDP :  m_remain_sendlen:%d]",
                    m_sendRemain);
                m_sendPtr = m_sendBuf;
                m_sendRemain = 0;
                return 1;
            }
            memmove(m_sendBuf, m_sendPtr, m_sendRemain);
            m_sendPtr = m_sendBuf + m_sendRemain;
        }
        else if ((int)m_sendRemain < ret)
        {
            printf("offset error[Remain_Data: %d Send:%d]", m_sendRemain, ret);
            return -1;
        }
        else
        {
            m_sendPtr = m_sendBuf;
            m_sendRemain = 0;
        }
    }
    return ret;
}
int CPeer::get_remain_sendlen()
{
    return m_sendRemain;
}
