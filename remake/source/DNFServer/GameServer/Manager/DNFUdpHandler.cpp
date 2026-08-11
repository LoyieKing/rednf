// df_manager_r — CUdpHandler（ORIG DNFUdpHandler.cpp）
#include "DNFUdpHandler.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "DNFFileLog.h"

int getErrno()
{
    return *__errno_location();
}

CUdpHandler::CUdpHandler()
{
    m_sock = -1;
    m_clientSock = -1;
}
CUdpHandler::~CUdpHandler() {}

int CUdpHandler::InitServerSocket(int port)
{
    int err;
    m_sock = socket(AF_INET, SOCK_DGRAM, 0x11);
    if (m_sock == -1)
    {
        err = getErrno();
        printf("Could not create a UDP socket : %d\n", err);
        return -1;
    }
    struct sockaddr_in addr;
    memset((char*)&addr, 0, 0x10);  // (char*)：ORIG 求值顺序 lea 先行
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0);
    addr.sin_port = htons((unsigned short)port);
    if (::bind(m_sock, (struct sockaddr*)&addr, 0x10) != 0)
    {
        err = getErrno();
        switch (err)
        {
        case 0x62:
            printf("Port %d for receiving UDP is in use\n", port);
            break;
        case 0x63:
            puts("Cannot assign requested address");
            break;
        case 0:
            break;
        default:
            printf("Could not bind UDP receive port. Error= %d , strerror = %s\n",
                   err, strerror(err));
            break;
        }
        m_sock = -1;
    }
    int bufsize = 0xf4240;
    setsockopt(m_clientSock, SOL_SOCKET, SO_RCVBUF, &bufsize, 4);
    DNF_LOG_SCOPE_LINE(0x6e, "./log/Udp", "Opened port %d with fd %d, recv buf size %d\n",
                       port, m_sock, bufsize);
    return m_sock;
}

int CUdpHandler::InitClientSocket()
{
    m_clientSock = socket(AF_INET, SOCK_DGRAM, 0x11);
    if (m_clientSock == -1)
    {
        int e = getErrno();
        printf("Could not create a UDP socket : %d\n", e);
        return -1;
    }
    register int fd = m_clientSock;  // ORIG：CMyFileLog ctor 前读入 ebx，ctor 后直接传参
    CMyFileLog log(__FUNCTION__, 0x8f);
    log("./log/Udp", "Opened port with fd %d\n", fd);
    return m_clientSock;
}

int CUdpHandler::SendToServer(char* buf, int len, unsigned short port, const char* ip) const
{
    if (m_clientSock == -1)
        return 0;
    int n;
    if (port == 0)
    {
        if (ip == 0)
        {
            n = send(m_clientSock, buf, len, 0);
        }
    }
    else
    {
        struct sockaddr_in addr;
        memset((char*)&addr, 0, 0x10);  // (char*)：ORIG 求值顺序 lea 先行
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip);
        n = sendto(m_clientSock, buf, len, 0, (struct sockaddr*)&addr, 0x10);
    }
    if (n == -1)
    {
        int e = getErrno();
        switch (e)
        {
        case 0x6f:
        case 0x70:
        case 0x71:
            DNF_LOG_SCOPE_LINE(0x1b2, "./log/UdpErr",
                "Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", e);
            break;
        case 0x61:
            DNF_LOG_SCOPE_LINE(0x1b8, "./log/UdpErr", "Error( EAFNOSUPPORT ) in send = %d\n", e);
            break;
        default:
            DNF_LOG_SCOPE_LINE(0x1be, "./log/UdpErr",
                "err = %d , strerror = %s in send\n", e, strerror(e));
            break;
        }
        return 0;
    }
    if (n == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1c7, "./log/UdpErr", "no data sent in send\n");
        return 0;
    }
    if (len != n)
    {
        DNF_LOG_SCOPE_LINE(0x1ce, "./log/UdpErr", "Only %d out of %d bytes sent\n", n, len);
        return 0;
    }
    return 1;
}

int CUdpHandler::SendToClient(char* buf, int len, unsigned short port, const char* ip,
                              unsigned int addr) const
{
    if (!ip && !addr)
        return 0;
    if (ip)
        addr = inet_addr(ip);
    if (m_sock == -1)
        return 0;
    int n;
    if (port == 0)
    {
        if (addr == 0)
        {
            n = send(m_sock, buf, len, 0);
        }
    }
    else
    {
        struct sockaddr_in to;
        memset(&to, 0, 0x10);
        to.sin_family = AF_INET;
        to.sin_port = htons(port);
        to.sin_addr.s_addr = ntohl(addr);
        n = sendto(m_sock, buf, len, 0, (struct sockaddr*)&to, 0x10);
    }
    if (n == -1)
    {
        int e = getErrno();
        switch (e)
        {
        case 0x6f:
        case 0x70:
        case 0x71:
            printf("Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", e);
            DNF_LOG_SCOPE_LINE(0x113, "./log/UdpErr",
                "Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", e);
            break;
        case 0x61:
            puts("err EAFNOSUPPORT in send");
            DNF_LOG_SCOPE_LINE(0x119, "./log/UdpErr", "Error( EAFNOSUPPORT ) in send = %d\n", e);
            break;
        default:
            printf("err = %d , strerror = %s in send\n", e, strerror(e));
            DNF_LOG_SCOPE_LINE(0x11f, "./log/UdpErr", "err = %d , strerror = %s in send\n",
                               e, strerror(e));
            break;
        }
        return 0;
    }
    if (n == 0)
    {
        puts("no data sent in send");
        DNF_LOG_SCOPE_LINE(0x128, "./log/UdpErr", "no data sent in send\n");
        return 0;
    }
    if (len != n)
    {
        // ORIG 实测：printf 与 log 都保留 %s bug（n 按指针打印）。
        printf("Only %s out of %d bytes sent\n", n, len);
        DNF_LOG_SCOPE_LINE(0x133, "./log/UdpErr", "Only %s out of %d bytes sent\n", n, len);
        return 0;
    }
    return 1;
}

bool CUdpHandler::RecvFromClient(char* buf, int* size, unsigned int* addr,
                                 unsigned short* port) const
{
    if (m_sock == -1)
        return 0;
    struct sockaddr_in from;
    int slen = 0x10;
    *size = recvfrom(m_sock, buf, *size, 0, (struct sockaddr*)&from, (socklen_t*)&slen);
    if (*size == -1)
    {
        int e = getErrno();
        switch (e)
        {
        case 0x58:
        {
            puts("Error fd not a socket");
            DNF_LOG_SCOPE_AT(__FUNCTION__, 0xaf, "./log/UdpErr", "Error fd not a socket\n");
            break;
        }
        case 0x68:
        {
            puts("Error connection reset - host not reachable");
            DNF_LOG_SCOPE_AT(__FUNCTION__, 0xb6, "./log/UdpErr",
                "Error connection reset - host not reachable\n");
            break;
        }
        default:
            printf("Hm! Time out Or Socket Error = %d\n", e);
            break;
        }
        return 0;
    }
    if (*size < 1)
    {
        printf("Socket closed? Recv size = %d\n", *size);
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0xc6, "./log/UdpErr",
            "Socket closed? Recv size = %d\n", *size);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *addr = ntohl(from.sin_addr.s_addr);
    char* ip = inet_ntoa(from.sin_addr);
    char* unused_buf = buf;  // ORIG：包 ID 判断与日志实参均经该局部（-0x20(%ebp)）
    (void)unused_buf;
    if (*(unsigned short*)unused_buf == 0x4c8 || *(unsigned short*)unused_buf == 0x4c9 ||
        *(unsigned short*)unused_buf == 0x44f || *(unsigned short*)unused_buf == 0x450)
    {
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0xd1, "./log/Udp",
            "PacketId(%d) Recv success! IP = %s, Port %d, Recv size = %d",
            *(unsigned short*)unused_buf, ip, *port, *size);
    }
    buf[*size] = 0;  // ORIG：无条件执行（包 ID 只控制日志），随后恒 return 1
    return 1;
}

char CUdpHandler::RecvFromServer(char* buf, int* size, unsigned int* addr,
                                 unsigned short* port) const
{
    if (m_clientSock == -1)
        return 0;
    struct sockaddr_in from;
    int slen = 0x10;
    *size = recvfrom(m_clientSock, buf, *size, 0, (struct sockaddr*)&from, (socklen_t*)&slen);
    if (*size == -1)
    {
        int e = getErrno();
        switch (e)
        {
        case 0x58:
            puts("Error fd not a socket");
            DNF_LOG_SCOPE_AT(__FUNCTION__, 0x156, "./log/UdpErr", "Error fd not a socket\n");
            break;
        case 0x68:
            puts("Error connection reset - host not reachable");
            DNF_LOG_SCOPE_AT(__FUNCTION__, 0x15d, "./log/UdpErr",
                "Error connection reset - host not reachable\n");
            break;
        default:
            printf("Hm! Time out Or Socket Error = %d\n", e);
            break;
        }
        return 0;
    }
    if (*size < 1)
    {
        printf("Socket closed? Recv size = %d\n", *size);
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x16d, "./log/UdpErr",
            "Socket closed? Recv size = %d\n", *size);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *addr = ntohl(from.sin_addr.s_addr);
    char* ip = inet_ntoa(from.sin_addr);
    char* unused_buf = buf;  // ORIG：包 ID 判断与日志实参均经该局部（-0x20(%ebp)）
    (void)unused_buf;
    if (*(unsigned short*)unused_buf == 0x4c8 || *(unsigned short*)unused_buf == 0x4c9)
    {
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x179, "./log/Udp",
            "PacketId(%d) Recv success! IP = %s, Port %d, Recv size = %d",
            *(unsigned short*)unused_buf, ip, *port, *size);
    }
    buf[*size] = 0;  // ORIG：无条件执行，随后恒 return 1
    return 1;
}

int CUdpHandler::GetServerSocket() { return m_sock; }

unsigned int CUdpHandler::InetAddr(const char* ip) const
{
    return inet_addr(ip);
}
