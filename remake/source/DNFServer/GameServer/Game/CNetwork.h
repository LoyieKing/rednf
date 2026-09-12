#ifndef DNF_GAME_CNETWORK_H_
#define DNF_GAME_CNETWORK_H_

// ============================================================================
// df_game_r 还原 —— CNetwork<T,R> 网络缓冲模板（ORIG Network.h）。
// 布局（T=接收缓冲字节数，R=发送缓冲字节数）：
//   +0x00 m_sock | +0x04/+0x08 连接标志 | +0x10..+0x29 杂项 |
//   +0x2c m_recvBuf -> +0x30 m_recv[T] | +0x30+T m_sendSize |
//   +0x34+T m_sendBuf -> +0x38+T m_send[R] |
//   +0x38+T+R m_natType | m_ip | m_ipStr[16] | m_port | m_mtu | m_innerIp |
//   m_macAddr[0x44] | m_mutex。
// 实例：<4096,450000>（CUser 客户端）、<100000,100000>（BaseServerProxy）、
//   <6144,100000>（社区）、<95000,450000>（exchange/监视）。
// 显式实例化见 CNetwork.cpp（extern template 抑制其它 TU 隐式实例化）。
// ============================================================================

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "GameBase.h"
#include "PacketHeader.h"
#include "PacketDesign.h"

class CDispatchServerPacket
{
public:
    static bool dispatchPacket(char* p);   // ORIG 0x8473660：末尾 mov $1,%ebx → 恒返 true（调用侧 test %al；bool 由 ORIG __PRETTY_FUNCTION__ "static bool ..." 实证）
    static bool dispatchAuctionPacket(char* p);  // ORIG 0847ac3c（恒返 1；__PRETTY_FUNCTION__ 实证 static bool）
    static bool dispatchPointPacket(char* p);    // ORIG 0847c1fe（恒返 1；__PRETTY_FUNCTION__ 实证 static bool）
};

template <int N1, int N2>
class CNetwork
{
public:
    // P216（b28-L11）：throw() 消调用者 4.4 冗余 landing pad（ORIG 4.1 CSession::C1
    // dead-temp 形 20 条无 EH；-fno-enforce-eh-specs 下本 W 体尺寸不变、无 guard）。
    CNetwork() throw();
    ~CNetwork() throw();

    int get_socket();
    void disconnect();
    int send_packet(char* data, int len);
    int recv_packet();
    int send_packet();
    static bool SetNonBlock(int sock);
    void send_lock();
    void send_unlock();
    int GetRemainSendSize();
    static int CreateConnectionSocket(int& sock, const char* ip, int port);
    static int CreateConnectionBlockSocket(int& sock, const char* ip, int port);
    static bool CreateListenSocket(int& sock, int port, const char* ip);
    bool Parsing_ServerPacket(int recvLen);
    bool Parsing_AuctionServerPacket(int recvLen);

    // ---- P2P/NAT 接口（<4096,450000> 实例使用）----
    char* GetPeerIP2(char* ip, int size);
    char* get_str_ip();
    char get_nat_type();
    void set_nat(char natType, unsigned int innerIp, unsigned int ip,
                 unsigned short port, unsigned int mtu);
    void set_nat(char natType, unsigned int innerIp, unsigned int ip,
                 unsigned short port, const char* macAddr);
    char* get_mac_addr() const;
    unsigned int get_inner_ip();
    unsigned int get_ip();
    unsigned short get_port();
    int get_mtu();
    bool set_socket(int sock, bool nonBlock);
    void initNetworkBuffer();

    // ---- 对象布局（全部 public，ORIG 直访成员）----
    int m_sock;               // +0x00
    // +0x04..+0x0b（ORIG disconnect 0x82fdf61 以 8 字节整清：单基址 + 两 store；
    // Lane6 探针实证：u64 标量视图赋 0 单表达式复现，匿名 struct 保有两具名成员，
    // 布局/偏移/sizeof 不变，ExchangeServer_Session/W7_UserOnlyC 拆分写不受影响）
    union {
        unsigned long long m_lastRecvGapPair;   // u64 整清视图
        struct {
            unsigned int m_lastRecvTick;        // +0x04（Parsing 记录最后收包 tick；ORIG CUser+0xe4）
            unsigned int m_recvGapTick;         // +0x08（recvGap；ORIG CUser+0xe8）
        };
    };
    int m_field0c;            // +0x0c
    int m_field10;            // +0x10（ctor 清零）
    char m_field14;           // +0x14（ctor 清零）
    unsigned int m_recvOffset;// +0x18（接收缓冲当前偏移）
    int m_field1c;            // +0x1c
    int m_field20;            // +0x20
    int m_field24;            // +0x24
    char m_field28;           // +0x28
    char m_bBufOffsetTrace;   // +0x29（ctor/recv 清零；Parsing BUFOFFSET TRACE 标志）
    char m_pad[2];            // +0x2a
    char* m_recvBuf;          // +0x2c -> m_recv
    char m_recv[N1];          // +0x30
    unsigned int m_sendSize;  // +0x30+N1
    char* m_sendBuf;          // +0x34+N1 -> m_send
    char m_send[N2];          // +0x38+N1
    char m_natType;           // +0x38+N1+N2
    unsigned int m_ip;        // +0x3c+N1+N2
    char m_ipStr[16];         // +0x40+N1+N2
    unsigned short m_port;    // +0x50+N1+N2
    unsigned int m_mtu;       // +0x54+N1+N2
    unsigned int m_innerIp;   // +0x58+N1+N2
    char m_macAddr[0x44];     // +0x5c+N1+N2
    Mutex m_mutex;            // +0xa0+N1+N2
};

template <int N1, int N2>
CNetwork<N1, N2>::CNetwork() throw()
    : m_sock(-1), m_field10(0), m_field14(0), m_recvOffset(0), m_bBufOffsetTrace(0),
      m_sendSize(0), m_natType(0), m_ip(0), m_port(0), m_innerIp(0), m_mutex()
{
    m_recvBuf = m_recv;
    m_sendBuf = m_send;
}

template <int N1, int N2>
CNetwork<N1, N2>::~CNetwork() throw()
{
}

template <int N1, int N2>
int CNetwork<N1, N2>::get_socket()
{
    return m_sock;
}

template <int N1, int N2>
void CNetwork<N1, N2>::disconnect()
{
    // ORIG 0x82fdf61 族：+4/+8 以 8 字节整清，单基址 eax 复用两 store ——
    // 源形即 u64 标量视图单表达式赋 0（探针 /tmp/l6/probe.cpp disconnectA 逐条一致）。
    m_lastRecvGapPair = 0;
    shutdown(m_sock, 0);
    close(m_sock);
    m_sock = -1;
    m_natType = 0;
    m_ip = 0;
    m_port = 0;
    m_innerIp = 0;
}

template <int N1, int N2>
int CNetwork<N1, N2>::send_packet(char* data, int len)
{
    if (m_sock < 0)
    {
        return -1;
    }
    if (len <= 0)
    {
        // [Fn28Gap 2026-09-11] ORIG 0x082fdfcc+0xf..0x27 段：0x8(%esp) 装载
        // _ZZN8CNetworkILi100000ELi100000EE11send_packetEPciE12__FUNCTION__
        // @0x08c21a10（r,12B="send_packet\0"）⇒ 原码为宏 __FUNCTION__（非手写）。
        LogManager::logFormat(1, "CNetwork.h", __FUNCTION__, 0x25b,
            "send_packet(char*, int) error, len <= 0 : %d %d %d",
            data[0], data[1], len);
        return -1;
    }

    int ret = 0;
    errno = 0;
    send_lock();
    m_sendSize += len;
    if ((unsigned int)m_sendSize > (unsigned int)sizeof(m_send))
    {
        m_sendSize -= len;
        LogManager::logFormat(1, "CNetwork.h", __FUNCTION__, 0x26e,
            "send_packet(char*, int) error, Send buffer over flow : %d %d %d",
            data[1], m_sendSize, len);
        send_unlock();
        return -1;
    }
    if (!(m_sendBuf >= m_send && m_sendBuf < m_send + sizeof(m_send)))
    {
        m_sendSize -= len;
        LogManager::logFormat(1, "CNetwork.h", __FUNCTION__, 0x277,
            "send_packet(char*, int) error, Send buffer over flow : %d %d %d",
            data[1], m_sendSize, len);
        send_unlock();
        return -1;
    }
    else
    {
        memcpy(m_sendBuf, data, len);
        m_sendBuf += len;
        send_unlock();
        return send_packet();
    }
}

template <int N1, int N2>
int CNetwork<N1, N2>::recv_packet()
{
    if (m_sock < 0)
    {
        return 0;
    }
    errno = 0;
    int remain = sizeof(m_recv) - m_recvOffset;
    if (remain == 0)
    {
        m_recvBuf = m_recv;
        m_recvOffset = 0;
        remain = N1;
        m_bBufOffsetTrace = 0;
    }

    int ret;
    if ((ret = read(m_sock, m_recvBuf, remain)) < 0)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK ||
            errno == 0 || errno == EINPROGRESS)
        {
            return 0;
        }
        // [Fn28Gap 2026-09-11] ORIG 0x0818e277: movl $0x8b96721,0x4(%esp) =
        // _ZZN8CNetworkILi6144ELi100000EE11recv_packetEvE12__FUNCTION__
        // （r,12B="recv_packet\0"）⇒ cMyTrace 首参为宏 __FUNCTION__。
        cMyTrace(__FUNCTION__, 0x210, 0)("recv_packet() error(%d:%s)",
            m_sock, errno, strerror(errno));
        return -1;
    }
    if (ret == 0)
    {
        return -1;
    }
    return ret;
}

template <int N1, int N2>
int CNetwork<N1, N2>::send_packet()
{
    int ret = 0;
    if (m_sendSize == 0)
    {
        return 0;
    }

    send_lock();
    if ((ret = write(m_sock, m_send, m_sendSize)) <= 0)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK ||
            errno == 0 || errno == EINPROGRESS)
        {
            send_unlock();
            return 0;
        }
        // [Fn28Gap 2026-09-11] ORIG 0x0818e5dc: movl $0x8b96710,0x4(%esp) =
        // _ZZN8CNetworkILi6144ELi100000EE11send_packetEvE12__FUNCTION__
        // （r,12B="send_packet\0"）⇒ 原码 __FUNCTION__（旧手写 "send_packet()"
        // 串内容亦与 ORIG 不同）。
        cMyTrace(__FUNCTION__, 0x2af, 0)("send_packet() error(%d:%s)",
            m_sock, errno, strerror(errno));
        send_unlock();
        return -1;
    }
    if (ret > 0)
    {
        if ((int)m_sendSize > ret)
        {
            m_sendBuf = m_send + ret;
            m_sendSize -= ret;
            if ((unsigned int)m_sendSize > (unsigned int)sizeof(m_send))
            {
                // [Fn28Gap 2026-09-11] ORIG 0x0818e69a: 0x8(%esp)=__FUNCTION__。
                LogManager::logFormat(1, "CNetwork.h", __FUNCTION__, 0x2d1,
                    "send_packet() error, Send buffer over flow : %d", m_sendSize);
                return -1;
            }
            memmove(m_send, m_sendBuf, m_sendSize);
            m_sendBuf = m_send + m_sendSize;
        }
        else if ((int)m_sendSize < ret)
        {
            // [Fn28Gap 2026-09-11] ORIG 0x0818e73c: 0x8(%esp)=__FUNCTION__。
            LogManager::logFormat(1, "CNetwork.h", __FUNCTION__, 0x2db,
                "send_packet() error : %d %d", m_sendSize, ret);
            send_unlock();
            return -1;
        }
        else
        {
            m_sendBuf = m_send;
            m_sendSize = 0;
        }
    }
    send_unlock();
    return ret;
}

template <int N1, int N2>
bool CNetwork<N1, N2>::SetNonBlock(int sock)
{
    int flags = fcntl(sock, 3, 0);
    int ret = fcntl(sock, 4, flags | 0x800);
    if (ret < 0)
    {
        return 0;
    }
    return 1;
}

template <int N1, int N2>
void CNetwork<N1, N2>::send_lock()
{
    m_mutex.lock();
}

template <int N1, int N2>
void CNetwork<N1, N2>::send_unlock()
{
    m_mutex.unlock();
}

template <int N1, int N2>
int CNetwork<N1, N2>::GetRemainSendSize()
{
    return m_sendSize;
}

template <int N1, int N2>
int CNetwork<N1, N2>::CreateConnectionSocket(int& sock, const char* ip, int port)
{
    int ret = 0;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        return 1;
    }
    if (!SetNonBlock(sock))
    {
        return 2;
    }

    sockaddr_in addr;
    memset(&addr, 0, 0x10);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons((unsigned short)port);

    ret = connect(sock, (sockaddr*)&addr, 0x10);
    if (ret < 0 && errno != EINPROGRESS && errno != EAGAIN &&
        errno != EINTR && errno != 0)
    {
        return 3;
    }
    return 0;
}

template <int N1, int N2>
int CNetwork<N1, N2>::CreateConnectionBlockSocket(int& sock, const char* ip, int port)
{
    int ret = 0;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        return 1;
    }

    sockaddr_in addr;
    memset(&addr, 0, 0x10);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons((unsigned short)port);

    ret = connect(sock, (sockaddr*)&addr, 0x10);
    if (ret < 0)
    {
        return 3;
    }
    return 0;
}

template <int N1, int N2>
bool CNetwork<N1, N2>::CreateListenSocket(int& sock, int port, const char* ip)
{
    // ORIG 槽向 addr@-0x20 / opt@-0x10；4.4 聚合优先低槽（N82附），opt 未初始化前置试翻。
    int opt;
    sockaddr_in addr;
    opt = 0xea60;
    int ret = 0;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        return false;
    }

    memset(&addr, 0, 0x10);
    addr.sin_family = AF_INET;
    if (ip != 0)
    {
        addr.sin_addr.s_addr = inet_addr(ip);
    }
    else
    {
        addr.sin_addr.s_addr = htonl(0);
    }
    addr.sin_port = htons((unsigned short)port);

    ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, 4);
    if (ret < 0)
    {
        return false;
    }
    if (bind(sock, (sockaddr*)&addr, 0x10) < 0)
    {
        return false;
    }
    if (listen(sock, 0x400) < 0)
    {
        return false;
    }
    if (!SetNonBlock(sock))
    {
        return false;
    }
    return true;
}

template <int N1, int N2>
bool CNetwork<N1, N2>::Parsing_ServerPacket(int recvLen)
{
    // [b28d-solo-CNetwork] AE（ORIG 0x0846d050，155=155 条；p167 6/6 CLEAR）。
    // 修两处真语义 bug：①packetSize 守卫为负极性短路 `ps<=9 || ps>0x1800` 走 log
    // （ORIG 0x0846d153 `jbe <log>` / 0x0846d159 `jbe <dispatch>`；旧嵌套
    // if(ps>9){if(ps>0x1800)} 在 4.4 下把 ps<=9 误送 dispatch）；
    // ②dispatch 后 `>9 continue` 属 if 块内部、0<remain<=9 落块外 break
    // （ORIG 0x0846d204 `jne` / 0x0846d214 `cmpl $0x9;jg`）；旧版块内无条件
    // continue ⇒ 半包回循环。残余仅 .o 未解析重定位（call 目标/rodata 地址）
    // ⇒ AE 口径（P264/N124）。
    PacketHeader header(0, 0);
    int remain = m_recvOffset + recvLen;
    unsigned int packetSize;
    int headerSize = 0xa;
    if (remain <= 9)
    {
        m_recvOffset += recvLen;
        m_recvBuf += recvLen;
        // [Fn28Gap 2026-09-11] ORIG 0x0818e35d: 0x8(%esp) =
        // _ZZN8CNetworkILi6144ELi100000EE20Parsing_ServerPacketEiE12__FUNCTION__
        // （r,21B="Parsing_ServerPacket\0"）⇒ 原码 __FUNCTION__。
        LogManager::logFormat(1, "CNetwork.h", __FUNCTION__, 0x31d,
            "Packet size(%d:%d:%d) < HEADER_SIZE", m_recvBuf, m_recv,
            m_recvOffset, recvLen);
        return 1;
    }

    while (true)
    {
        if (m_recvOffset != 0)
        {
            m_recvBuf -= m_recvOffset;
        }
        // [SemB12 2026-09-12 真语义差异修复] ORIG 0x0846d11c 的 `je +0xdc` 落点 =
        //   memcpy 段起点（idx61）⇒ offset==0 时**仍执行** memcpy（否则 header 未
        //   初始化即被读）；旧写法把 memcpy 放块内，我方 `je` 落 idx68 跳过它。
        memcpy(&header, m_recvBuf, 0xa);
        packetSize = header.packetSize;
        // [b28d-solo-CNetwork] ORIG 0x0846d153 段实证：`cmpl $0x9; jbe <log>; cmpl
        // $0x1800; jbe <dispatch>` ⇒ 原码是负极性短路 `ps<=9 || ps>0x1800` 走 log；
        // 旧嵌套 if(ps>9){if(ps>0x1800)} 在 4.4 下把 ps<=9 误送 dispatch（真语义 bug）。
        if (packetSize <= 9 || packetSize > 0x1800)
        {
            // [Fn28Gap 2026-09-11] ORIG 0x0818e402: 0x8(%esp)=__FUNCTION__。
            LogManager::logFormat(1, "CNetwork.h", __FUNCTION__,
                0x32d, "Packet size(%d:%d:%d) ERROR", recvLen, packetSize,
                m_recvBuf);
            m_recvBuf = m_recv;
            m_recvOffset = 0;
            return 0;
        }
        if (remain >= packetSize)
        {
            CDispatchServerPacket::dispatchPacket(m_recvBuf);
            remain -= packetSize;
            m_recvBuf += packetSize;
            m_recvOffset = 0;
            if (remain == 0)
            {
                m_recvBuf = m_recv;
                break;
            }
            // [b28d-solo-CNetwork] ORIG 0x0846d204/0x0846d214 段形：
            //   jne +0x10; <赋值>; jmp exit; +0x10: cmpl $0x9,remain; jg <loop>;
            //   jmp exit; nop; exit:  ⇒ 该 `>9 continue` 属 dispatch 块内部；
            // remain<=9 落到块外 break。旧版把它放在 if 块外且块内无条件
            // continue ⇒ dispatch 后 0<remain<=9 也回循环（真语义 bug）。
            if (remain > 9)
            {
                continue;
            }
        }
        break;
    }

    if (remain > 0)
    {
        if ((unsigned int)remain > (unsigned int)N2)
        {
            return 0;
        }
        memmove(m_recv, m_recvBuf, remain);
        m_recvBuf = m_recv + remain;
        m_recvOffset = remain;
    }
    return 1;
}

template <int N1, int N2>
bool CNetwork<N1, N2>::Parsing_AuctionServerPacket(int recvLen)
{
    // [b28d-solo-CNetwork] AE（ORIG 0x082f7ee2，148=148 条；p167 8/8 CLEAR）。
    // 修三处：①packetSize 守卫负极性短路 `ps<=0x11 || ps>0x5000` 走 log
    // （ORIG 0x082f7f7d `jbe <log>` / 0x082f7f83 `jbe <dispatch>`）；
    // ②dispatch 后 `>0x11 continue` 与 log(0x41c) 归 if 块内、remain<packetSize
    // 落块外 break（ORIG 0x082f8044 段）；③header 用线格式字节缓冲 + 具型视图
    // 复现 ORIG 无 nsl::PACKET_HEADER::C1 调用形（P215；ORIG 0x082f7f52 段为
    // -0x26 处 0x12 字节栈缓冲直 call getSize）。旧「N82 封顶」判词作废。
    // [b28d-solo-CNetwork] ORIG 0x082f7f52 段：header 为 -0x26 处 0x12 字节栈
    // 缓冲，且**无** nsl::PACKET_HEADER::C1 调用（4.1 前端把空默认 ctor 展开为
    // 0 条，P215）。用线格式字节缓冲 + 具型视图（P80/P95/P156）复现，消除 4.4
    // 的 out-of-line 空 ctor call（本件唯一残差 3 条）。
    char headerBuf[0x12];
    int remain = m_recvOffset + recvLen;
    unsigned int packetSize;
    int headerSize = 0x12;
    if (remain <= 0x11)
    {
        m_recvOffset += recvLen;
        m_recvBuf += recvLen;
        return 1;
    }

    while (true)
    {
        // [SemB00 2026-09-12] 真语义差异修复（ORIG 0x082f7ee2 段实证）：
        // ORIG `je`（m_recvOffset==0，0x082f7f1f）落点 = 0x082f7f52（**memcpy 段
        // 起点**）⇒ 原码为 `if (off != 0) { m_recvBuf -= off; }` 后 **无条件**
        // memcpy；旧写法把 memcpy 关进 if 内，m_recvOffset==0 时 headerBuf 未
        // 初始化即 getSize（真语义偏差）。
        if (m_recvOffset != 0)
        {
            m_recvBuf -= m_recvOffset;
        }
        memcpy(headerBuf, m_recvBuf, 0x12);
        packetSize = reinterpret_cast<nsl::PACKET_HEADER*>(headerBuf)->getSize();
        // [b28d-solo-CNetwork] ORIG 0x082f7f7d 段实证：`cmpl $0x11; jbe <log>;
        // cmpl $0x5000; jbe <dispatch>` ⇒ 负极性短路 `ps<=0x11 || ps>0x5000`
        // 走 log；旧嵌套 if 在 4.4 下把 ps<=0x11 误送 dispatch（真语义 bug）。
        if (packetSize <= 0x11 || packetSize > 0x5000)
        {
            // ORIG 0x082f7f92..0x082f7fa8：packetID 先求值存 %ebx 跨 getCategory
            // 调用 ⇒ 无具名局部，实参内联（P44/P226）。
            // [Fn28Gap 2026-09-11] ORIG 0x082f7fca: 0x8(%esp) =
            // _ZZN8CNetworkILi100000ELi100000EE27Parsing_AuctionServerPacketEiE12__FUNCTION__
            // @0x08c1ebd4（r,28B="Parsing_AuctionServerPacket\0"）。
            LogManager::logFormat(1, "CNetwork.h",
                __FUNCTION__, 0x402,
                "Packet size(%d:%d:%d:%d) ERROR", recvLen, packetSize,
                reinterpret_cast<nsl::PACKET_HEADER*>(headerBuf)->getCategory(),
                reinterpret_cast<nsl::PACKET_HEADER*>(headerBuf)->getPacketID());
            m_recvBuf = m_recv;
            m_recvOffset = 0;
            return 0;
        }
        if (remain >= packetSize)
        {
            CDispatchServerPacket::dispatchAuctionPacket(m_recvBuf);
            remain -= packetSize;
            m_recvBuf += packetSize;
            m_recvOffset = 0;
            if (remain == 0)
            {
                m_recvBuf = m_recv;
                break;
            }
            // [b28d-solo-CNetwork] ORIG 0x082f8044 段形：jne +0x10; <赋值>; jmp exit;
            // +0x10: cmpl $0x11,remain; jg <loop>; <log 0x41c>; jmp exit; nop; exit:
            // ⇒ `>0x11 continue` 与 log(0x41c) 均属 dispatch 块内部；remain<packetSize
            // 落到块外 break。旧版块内无条件 continue + 块外守卫（真语义 bug）。
            if (remain > 0x11)
            {
                continue;
            }
            // [Fn28Gap 2026-09-11] ORIG 0x082f8069 段：0x8(%esp)=__FUNCTION__。
            LogManager::logFormat(1, "CNetwork.h", __FUNCTION__,
                0x41c, "Packet size(%d) < HEADER_SIZE", remain);
            break;
        }
        break;
    }

    if (remain > 0)
    {
        if ((unsigned int)remain > (unsigned int)N2)
        {
            return 0;
        }
        memmove(m_recv, m_recvBuf, remain);
        m_recvBuf = m_recv + remain;
        m_recvOffset = remain;
    }
    return 1;
}

// ---- <4096,450000> P2P/NAT 专用 ----

template <int N1, int N2>
char* CNetwork<N1, N2>::GetPeerIP2(char* ip, int size)
{
    socklen_t addrLen = 0x10;
    memset(ip, 0, size);
    if (size <= 0xf)
    {
        return "0.0.0.0";
    }

    sockaddr_in addr;
    if (getpeername(m_sock, (sockaddr*)&addr, &addrLen) != 0)
    {
        return "0.0.0.0";
    }
    strncpy(ip, inet_ntoa(addr.sin_addr), 0x10);
    return ip;
}

template <int N1, int N2>
char* CNetwork<N1, N2>::get_str_ip()
{
    return m_ipStr;
}

template <int N1, int N2>
char CNetwork<N1, N2>::get_nat_type()
{
    return m_natType;
}

template <int N1, int N2>
void CNetwork<N1, N2>::set_nat(char natType, unsigned int innerIp,
                               unsigned int ip, unsigned short port, unsigned int mtu)
{
    in_addr addr;
    m_natType = natType;
    m_innerIp = innerIp;
    m_ip = ip;
    m_port = port;
    m_mtu = mtu;
    addr.s_addr = m_ip;
    strncpy(m_ipStr, inet_ntoa(addr), 0x10);
}

template <int N1, int N2>
void CNetwork<N1, N2>::set_nat(char natType, unsigned int innerIp,
                               unsigned int ip, unsigned short port,
                               const char* macAddr)
{
    in_addr addr;
    strncpy(m_macAddr, macAddr, 0x41);
    m_natType = natType;
    m_innerIp = innerIp;
    m_ip = ip;
    m_port = port;
    addr.s_addr = m_ip;
    strncpy(m_ipStr, inet_ntoa(addr), 0x10);
}

template <int N1, int N2>
char* CNetwork<N1, N2>::get_mac_addr() const
{
    return (char*)m_macAddr;
}

template <int N1, int N2>
unsigned int CNetwork<N1, N2>::get_inner_ip()
{
    return m_innerIp;
}

template <int N1, int N2>
unsigned int CNetwork<N1, N2>::get_ip()
{
    return m_ip;
}

template <int N1, int N2>
unsigned short CNetwork<N1, N2>::get_port()
{
    return m_port;
}

template <int N1, int N2>
int CNetwork<N1, N2>::get_mtu()
{
    return m_mtu;
}

template <int N1, int N2>
bool CNetwork<N1, N2>::set_socket(int sock, bool nonBlock)
{
    m_sock = sock;
    if (!nonBlock)
    {
        SetNonBlock(m_sock);
    }
    memset(m_ipStr, 0, 0x10);
    return 1;
}

template <int N1, int N2>
void CNetwork<N1, N2>::initNetworkBuffer()
{
    m_recvOffset = 0;
    m_recvBuf = m_recv;
    memset(m_recv, 0, N1);
    m_sendSize = 0;
    m_sendBuf = m_send;
    memset(m_send, 0, N2);
}

extern template class CNetwork<4096, 450000>;
extern template class CNetwork<100000, 100000>;
extern template class CNetwork<95000, 450000>;

#endif  // DNF_GAME_CNETWORK_H_
