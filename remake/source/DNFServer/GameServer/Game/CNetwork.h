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
    static void dispatchPacket(char* p);
    static void dispatchAuctionPacket(char* p);
    static void dispatchPointPacket(char* p);   // ORIG 0847c1fe
};

template <int N1, int N2>
class CNetwork
{
public:
    CNetwork();
    ~CNetwork();

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
    int CreateListenSocket(int& sock, int port, const char* ip);
    bool Parsing_ServerPacket(int recvLen);
    bool Parsing_AuctionServerPacket(int recvLen);

    // ---- P2P/NAT 接口（<4096,450000> 实例使用）----
    char* GetPeerIP2(char* ip, int size);
    char* get_str_ip();
    unsigned char get_nat_type();
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
    long long m_field04;      // +0x04（8 字节，disconnect 整体清零）
    int m_field0c;            // +0x0c
    int m_field10;            // +0x10（ctor 清零）
    char m_field14;           // +0x14（ctor 清零）
    unsigned int m_recvOffset;// +0x18（接收缓冲当前偏移）
    int m_field1c;            // +0x1c
    int m_field20;            // +0x20
    int m_field24;            // +0x24
    char m_field28;           // +0x28
    char m_field29;           // +0x29（ctor/recv 清零）
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
    char m_padTail[4];        // +0xa0+N1+N2+0x18（ORIG 尺寸 = 0xb8+N1+N2）
};

template <int N1, int N2>
CNetwork<N1, N2>::CNetwork()
    : m_sock(-1), m_field10(0), m_field14(0), m_recvOffset(0), m_field29(0),
      m_sendSize(0), m_natType(0), m_ip(0), m_port(0), m_innerIp(0), m_mutex()
{
    m_recvBuf = m_recv;
    m_sendBuf = m_send;
}

template <int N1, int N2>
CNetwork<N1, N2>::~CNetwork()
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
    m_field04 = 0;
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
        LogManager::logFormat(1, "CNetwork.h", "send_packet(char*, int)", 0x25b,
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
        LogManager::logFormat(1, "CNetwork.h", "send_packet(char*, int)", 0x26e,
            "send_packet(char*, int) error, Send buffer over flow : %d %d %d",
            data[1], m_sendSize, len);
        send_unlock();
        return -1;
    }
    if (!(m_sendBuf >= m_send && m_sendBuf < m_send + sizeof(m_send)))
    {
        m_sendSize -= len;
        LogManager::logFormat(1, "CNetwork.h", "send_packet(char*, int)", 0x277,
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
        m_field29 = 0;
    }

    int ret;
    if ((ret = read(m_sock, m_recvBuf, remain)) < 0)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK ||
            errno == 0 || errno == EINPROGRESS)
        {
            return 0;
        }
        cMyTrace("recv_packet", 0x210, 0)("recv_packet() error(%d:%s)",
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
        cMyTrace("send_packet()", 0x2af, 0)("send_packet() error(%d:%s)",
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
                LogManager::logFormat(1, "CNetwork.h", "send_packet()", 0x2d1,
                    "send_packet() error, Send buffer over flow : %d", m_sendSize);
                return -1;
            }
            memmove(m_send, m_sendBuf, m_sendSize);
            m_sendBuf = m_send + m_sendSize;
        }
        else if ((int)m_sendSize < ret)
        {
            LogManager::logFormat(1, "CNetwork.h", "send_packet()", 0x2db,
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
int CNetwork<N1, N2>::CreateListenSocket(int& sock, int port, const char* ip)
{
    sockaddr_in addr;
    int opt = 0xea60;
    int ret = 0;

    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock < 0)
    {
        return 0;
    }

    memset(&addr, 0, 0x10);
    addr.sin_family = AF_INET;
    if (port != 0)
    {
        addr.sin_addr.s_addr = inet_addr((const char*)port);
    }
    else
    {
        addr.sin_addr.s_addr = htonl(0);
    }
    addr.sin_port = htons((unsigned short)sock);

    ret = setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &opt, 4);
    if (ret < 0)
    {
        return 0;
    }
    if (bind(m_sock, (sockaddr*)&addr, 0x10) < 0)
    {
        return 0;
    }
    if (listen(m_sock, 0x400) < 0)
    {
        return 0;
    }
    if (!SetNonBlock(m_sock))
    {
        return 0;
    }
    return 1;
}

template <int N1, int N2>
bool CNetwork<N1, N2>::Parsing_ServerPacket(int recvLen)
{
    PacketHeader header(0, 0);
    int remain = m_recvOffset + recvLen;
    unsigned int packetSize;
    int headerSize = 0xa;
    if (remain <= 9)
    {
        m_recvOffset += recvLen;
        m_recvBuf += recvLen;
        LogManager::logFormat(1, "CNetwork.h", "Parsing_ServerPacket(int)", 0x31d,
            "Packet size(%d:%d:%d) < HEADER_SIZE", m_recvBuf, m_recv,
            m_recvOffset, recvLen);
        return 1;
    }

    while (true)
    {
        if (m_recvOffset != 0)
        {
            m_recvBuf -= m_recvOffset;
            memcpy(&header, m_recvBuf, 0xa);
        }
        packetSize = header.packetSize;
        if (packetSize > 9)
        {
            if (packetSize > 0x1800)
            {
                LogManager::logFormat(1, "CNetwork.h", "Parsing_ServerPacket(int)",
                    0x32d, "Packet size(%d:%d:%d) ERROR", recvLen, packetSize,
                    m_recvBuf);
                m_recvBuf = m_recv;
                m_recvOffset = 0;
                return 0;
            }
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
            continue;
        }
        if (remain > 9)
        {
            continue;
        }
        break;
    }

    if (remain > 0)
    {
        if ((unsigned int)remain > (unsigned int)sizeof(m_recv))
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
    nsl::PACKET_HEADER header;
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
        if (m_recvOffset != 0)
        {
            m_recvBuf -= m_recvOffset;
            memcpy(&header, m_recvBuf, 0x12);
        }
        packetSize = header.getSize();
        if (packetSize > 0x11)
        {
            if (packetSize > 0x5000)
            {
                int packetID = header.getPacketID();
                int category = header.getCategory();
                LogManager::logFormat(1, "CNetwork.h",
                    "Parsing_AuctionServerPacket(int)", 0x402,
                    "Packet size(%d:%d:%d:%d) ERROR", recvLen, packetSize,
                    category, packetID);
                m_recvBuf = m_recv;
                m_recvOffset = 0;
                return 0;
            }
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
            continue;
        }
        if (remain > 0x11)
        {
            continue;
        }
        LogManager::logFormat(1, "CNetwork.h", "Parsing_AuctionServerPacket(int)",
            0x41c, "Packet size(%d) < HEADER_SIZE", remain);
        break;
    }

    if (remain > 0)
    {
        if ((unsigned int)remain > (unsigned int)sizeof(m_recv))
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
unsigned char CNetwork<N1, N2>::get_nat_type()
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
extern template class CNetwork<6144, 100000>;
extern template class CNetwork<95000, 450000>;

#endif  // DNF_GAME_CNETWORK_H_
