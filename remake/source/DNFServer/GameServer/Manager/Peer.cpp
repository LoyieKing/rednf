// df_manager_r — CPeer（ORIG Peer.cpp）
#include "Peer.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "DNFFileLog.h"
#include "DNFTcpSocket.h"
#include "PacketHeader.h"
#include "Thread.h"

// ORIG 二进制证据：ConnSig/DisConnSig 的 pkt 局部槽为 6 字节（packetId+packetSize+
// reversed1），而 ctor 仍写满 10 字节（尾部 4 字节越界到 fd 槽，随后被 fd 覆盖）。
// 共享头文件中 Packet_InnerPakcet_* 为 10 字节内联 ctor（禁止改头），故 cpp 内
// 以 6 字节局部结构 + 直接声明调用原版 ctor 符号（extern "C" 保持符号不重修饰）。
// 原版二进制含弱符号 _ZN24Packet_InnerPakcet_LoginC1Ev / _ZN25Packet_InnerPakcet_LogoutC1Ev
// （地址 0x805772a / 0x805774e，W），本 TU 出库定义同形 ctor 供链接解析；
// 这两个符号不在 compare scope（targets.txt 无），不会新增 md。
extern "C" void _ZN24Packet_InnerPakcet_LoginC1Ev(void*);
extern "C" void _ZN25Packet_InnerPakcet_LogoutC1Ev(void*);

class Packet_InnerPakcet_Login : public PacketHeader
{
public:
    Packet_InnerPakcet_Login();
} __attribute__((packed));

class Packet_InnerPakcet_Logout : public PacketHeader
{
public:
    Packet_InnerPakcet_Logout();
} __attribute__((packed));

Packet_InnerPakcet_Login::Packet_InnerPakcet_Login() : PacketHeader(0xfa0, 0xa) {}
Packet_InnerPakcet_Logout::Packet_InnerPakcet_Logout() : PacketHeader(0xfa1, 0xa) {}

void* CPeer::operator new(unsigned int size) { return g_peerPool.alloc(); }
void CPeer::operator delete(void* ptr) { g_peerPool.free(ptr); }
void CPeer::operator delete(void* ptr, unsigned int size) { g_peerPool.free(ptr, size); }

CPeer::CPeer()
{
    m_sendBuf = 0;
    m_recvLen = 0;
    m_sendLen = 0;
    m_recvQ = 0;
    m_sendBLock = 0;
    m_sendQLock = 0;
    m_remainSendLen = 0;
    m_recvBuf = 0;
}

CPeer::~CPeer()
{
    m_sendBuf = m_sendData;
    m_sendLen = 0;
    m_recvLen = 0;
    m_recvBuf = m_data183c;
    m_remainSendLen = 0;
}

int CPeer::recv_packet()
{
    if (getHandle() < 0)
        return 0;
    errno = 0;
    int remaining = m_sendData - m_sendBuf + 0x1800;
    if (remaining == 0)
    {
        m_sendBuf = m_sendData;
        m_recvLen = 0;
        remaining = 0x1800;
    }
    int n;
    if ((n = read(getHandle(), m_sendBuf, remaining)) < 0)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN || errno == 0)
            return 0;
        printf("RECV ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
               getHandle(), errno, strerror(errno));
        return -1;
    }
    if (n == 0)
    {
        DNF_LOG_SCOPE_LINE(0xa4, "./log/TcpRecv", "Recv ERROR = 0 (%d) : %s, MaxRead(%d) nRead(%d)",
            errno, strerror(errno), remaining, n);
        return -1;
    }
    return n;
}

int CPeer::send_packet()
{
    int ret = 0;
    if (m_remainSendLen == 0)
        return 1;
    if ((ret = write(getHandle(), m_data183c, m_remainSendLen)) <= 0)
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
        if ((int)m_remainSendLen > ret)
        {
            m_recvBuf = m_data183c + ret;
            m_remainSendLen -= ret;
            if (m_remainSendLen > 0x96000u)
            {
                register unsigned int remain = m_remainSendLen;
                CMyFileLog log(__FUNCTION__, 0x17e);
                log("./log/TcpErr",
                    "m_remain_sendlen < MAX_PACKET_SIZE_UDP :  m_remain_sendlen:%d]",
                    remain);
                char pad[16];
                m_recvBuf = m_data183c;
                m_remainSendLen = 0;
                return 1;
            }
            memmove(m_data183c, m_recvBuf, m_remainSendLen);
            m_recvBuf = m_data183c + m_remainSendLen;
        }
        else if ((int)m_remainSendLen < ret)
        {
            printf("offset error[Remain_Data: %d Send:%d]", m_remainSendLen, ret);
            return -1;
        }
        else
        {
            m_recvBuf = m_data183c;
            m_remainSendLen = 0;
        }
    }
    return ret;
}

int CPeer::send_packet(char* buf, int len)
{
    if (getHandle() < 0)
        return -1;
    if (len <= 0)
    {
        printf("!!!Send Packet[(%d,%d) Size(%d) Error\n", buf[0], buf[1], len);
        return -1;
    }
    errno = 0;
    m_remainSendLen += len;
    if ((unsigned int)m_remainSendLen > 0x96000)
    {
        DNF_LOG_SCOPE_LINE(0x133, "./log/TcpErr",
            "!!!Send Packet Overflow P_TYPE[%d] Size:Remain[%d] Last[%d]",
            buf[1], m_remainSendLen, len);
        m_recvBuf = m_data183c;
        m_remainSendLen = 0;
        return -1;
    }
    if (m_recvBuf < m_data183c ||
        (unsigned int)m_recvBuf >= (unsigned int)(m_data183c) + 0x96000u)
    {
        DNF_LOG_SCOPE_LINE(0x13b, "./log/TcpErr",
            "!!!Send Packet Buffer critical error P_TYPE[%d] Size:Remain[%d] Last[%d]",
            buf[1], m_remainSendLen, len);
        m_recvBuf = m_data183c;
        m_remainSendLen = 0;
        return -1;
    }
    memcpy(m_recvBuf, buf, len);
    m_recvBuf += len;
    return send_packet();
}
void CPeer::InitPeer(TcpRecvQueue* recvQ, CMutex* qLock, CMutex* bLock)
{
    m_recvQ = recvQ;
    m_sendQLock = qLock;
    m_sendBLock = bLock;
    m_sendBuf = m_sendData;
    m_sendLen = 0;
    m_recvLen = 0;
    m_recvBuf = m_data183c;
    m_remainSendLen = 0;
}
bool CPeer::parsing(int len)
{
    // R40：栈槽声明顺序按 ORIG 布局（qsize -0x28 / parsinglength -0x24 / size -0x20 / headerSize -0x1c；
    // GCC 4.4 按反声明序分配简单局部，hdr 为带 ctor 的 12 字节对象独立落在 -0x5a）。
    PacketHeader hdr(0, 0);
    int qsize;
    int parsinglength = m_recvLen + len;
    unsigned int size;
    int headerSize = 10;
    if (parsinglength <= 9)
    {
        m_recvLen = m_recvLen + len;
        m_sendBuf += len;
        DNF_LOG_SCOPE_LINE(0xbb, "./log/TcpRecv",
            "(offset:%x - buf:%x) = remainlen:%d, Recv Size[%d] ",
            m_sendBuf, m_sendData, m_recvLen, len);
        return 1;
    }
    // R40：ORIG 控制流形态——主路径/need_more 为 if-else 结构（jb 入 else），
    // f8 日志在循环底部 continue 之后，else 的 goto out 冗余跳转被删留下 out 前空块 nop。
    for (;;)
    {
        if (m_recvLen != 0)
            m_sendBuf -= m_recvLen;
        memcpy(&hdr, m_sendBuf, 10);
        size = hdr.packetSize;
        if (size <= 9 || size > 0x1800)
        {
            DNF_LOG_SCOPE_LINE(0xd0, "./log/TcpRecv",
                "Recv Size[%d], Parsing Packet Size[%d] is Too Large, offset:%x, buf:%x, alreadyRead:%d",
                len, size, m_sendBuf, m_sendData, m_sendLen);
            m_sendBuf = m_sendData;
            m_recvLen = 0;
            return 0;
        }
        if (parsinglength >= size)
        {
            CTcpRecvBuffer* buf;
            {
                CGuard<CMutex> guard(m_sendBLock);
                buf = new CTcpRecvBuffer;
            }
            memcpy(buf, m_sendBuf, size);
            struct RAInt6 { char p[6]; int v; } __attribute__((packed));
            ((RAInt6*)buf)->v = getHandle();
            {
                CGuard<CMutex> guard(m_sendQLock);
                m_recvQ->push(buf);
                qsize = m_recvQ->size();
            }
            parsinglength -= size;
            m_sendBuf += size;
            m_recvLen = 0;
            if (parsinglength == 0)
            {
                m_sendBuf = m_sendData;
                goto out;
            }
            if (parsinglength > 9)
                continue;
            DNF_LOG_SCOPE_LINE(0xf8, "./log/TcpRecv",
                "need more data (parsinglength < HEADER_SIZE): body=%d !!",
                parsinglength);
            goto out;
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x100, "./log/TcpRecv",
                "need more data (packetsize > (unsigned int)parsinglength): body=%d !!",
                parsinglength);
            goto out;
        }
    }
out:
    if (parsinglength > 0)
    {
        try
        {
            if ((unsigned int)parsinglength > 0x1800)
            {
                DNF_LOG_SCOPE_LINE(0x10e, "./log/TcpRecv",
                    "[PARSING LENGTH EXCEPTION] parsinglength > MAX_RECV_BUF , memmove : parsinglength = %d",
                    parsinglength);
                return 0;
            }
            memmove(m_sendData, m_sendBuf, parsinglength);
            m_recvLen = parsinglength;
            m_sendBuf = m_sendData + parsinglength;
        }
        catch (...)
        {
            printf("[PARSING EXCEPTION] memmove : parsinglength = %d", parsinglength);
            return 0;
        }
    }
    return 1;
}
void CPeer::ConnSig()
{
    // ORIG 二进制证据：pkt 局部槽 6 字节（packetId+packetSize+reversed1），
    // ctor 仍写满 10 字节（尾部 4 字节越界到 fd 槽，随后被 fd 覆盖，与 ORIG 一致）。
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
        CGuard<CMutex> guard(m_sendBLock);
        buf = new CTcpRecvBuffer;
    }
    memcpy(buf, &pkt, pkt.packetSize);
    {
        CGuard<CMutex> guard(m_sendQLock);
        m_recvQ->push(buf);
    }
}
void CPeer::DisConnSig()
{
    // ORIG 二进制证据：pkt 局部槽 6 字节（packetId+packetSize+reversed1），
    // ctor 仍写满 10 字节（尾部 4 字节越界到 fd 槽，随后被 fd 覆盖，与 ORIG 一致）。
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
        CGuard<CMutex> guard(m_sendBLock);
        buf = new CTcpRecvBuffer;
    }
    memcpy(buf, &pkt, pkt.packetSize);
    {
        CGuard<CMutex> guard(m_sendQLock);
        m_recvQ->push(buf);
    }
}
bool CPeer::RecvPacket()
{
    int ret = recv_packet();
    if (ret > 0)
    {
        if (!parsing(ret))
        {
            // ORIG 实测：日志文案无结尾 \n（printf 有 \n）。
            DNF_LOG_SCOPE_LINE(0x4d, "./log/TcpRecv", "CPeer::Recv (false == parsing( size:%d ) )", ret);
            printf("CPeer::Recv (false == parsing( size:%d ) )\n", ret);
            return 1;
        }
        return 1;
    }
    if (ret < 0)
    {
        // ORIG：三个 getter 在 CMyFileLog 构造前求值（callee-saved 预装载），
        // 求值顺序 port -> adrs -> handle；port 以 int 形式入栈槽。
        register int p = GetTcpSocket()->getPeerPort();
        register char* a = GetTcpSocket()->getPeerAdrs();
        register int h = GetTcpSocket()->getHandle();
        DNF_LOG_SCOPE_LINE(0x59, "./log/TcpRecv",
            "Maybe Peer is disconnect!(%d), socket no(%d), addr(%s), port(%d)",
            ret, h, a, p);
        printf("CPeer::Recv (size(%d) < 0)\n", ret);
        return 0;
    }
    DNF_LOG_SCOPE_LINE(0x63, "./log/TcpRecv", "Maybe Peer is disconnect!(size == 0)");
    puts("CPeer::Recv (size == 0)");
    return 1;
}

TCPSocket* CPeer::GetTcpSocket() { return this; }
int CPeer::get_remain_sendlen() { return m_remainSendLen; }
