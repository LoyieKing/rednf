// df_coserver_r — CUdpHandler + 收发包结构（ORIG DNFUdpHandler.cpp）
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "DNFUdpHandler.h"
#include "DNFFileLog.h"

__attribute__((weak)) int getErrno()
{
    return errno;
}

CUdpHandler::CUdpHandler()
{
    m_sock = -1;
    m_clientSock = -1;
}

int CUdpHandler::InitServerSocket(int port)
{
    int err;
    m_sock = socket(2, 2, 0x11);
    if (m_sock == -1)
    {
        err = getErrno();
        printf("Could not create a UDP socket : %d\n", err);
        return -1;
    }
    sockaddr_in local;
    // ORIG 实测（0x804cc7c）：memset 首参地址先于常量装载（lea 先行）；
    // (char*) 显式转换可复现该求值顺序。
    memset((char*)&local, 0, 0x10);
    local.sin_family = 2;
    local.sin_addr.s_addr = htonl(0);
    local.sin_port = htons((unsigned short)port);
    if (bind(m_sock, (sockaddr*)&local, 0x10) != 0)
    {
        err = getErrno();
        switch (err)
        {
        case 0x62:
            printf("Port %d for receiving UDP is in use\n", port);
            break;
        case 99:
            puts("Cannot assign requested address");
            break;
        case 0:
            break;
        default:
            printf("Could not bind UDP receive port. Error= %d , strerror = %s\n", err,
                   strerror(err));
            break;
        }
        m_sock = -1;
    }
    int bufsize = 1000000;
    setsockopt(m_clientSock, 1, 8, &bufsize, 4);
    DNF_LOG_SCOPE_LINE(0x6e, "./log/Udp", "Opened port %d with fd %d, recv buf size %d\n", port, m_sock, bufsize);
    return m_sock;
}

int CUdpHandler::InitClientSocket()
{
    m_clientSock = socket(2, 2, 0x11);
    if (m_clientSock == -1)
    {
        int err = getErrno();
        printf("Could not create a UDP socket : %d\n", err);
        return -1;
    }
    // ORIG 实测第四参为 &_ZZN11CUdpHandler16InitClientSocketEvE12__FUNCTION__
    // （0x80b7118，__FUNCTION__ 符号），不得用字面量。
    CMyFileLog(__FUNCTION__, 0x8f)("./log/Udp", "Opened port with fd %d\n", m_clientSock);
    return m_clientSock;
}

bool CUdpHandler::RecvFromClient(char* buf, int* len, unsigned int* ip,
                                unsigned short* port) const
{
    if (m_sock == -1)
    {
        return 0;
    }
    sockaddr_in from;
    int slen = 0x10;
    // ORIG 实测：recvfrom 结果直写 *len，后续判断重读 *len（无 r 局部）
    *len = recvfrom(m_sock, buf, *len, 0, (sockaddr*)&from, (socklen_t*)&slen);
    if (*len == -1)
    {
        int err = getErrno();
        // ORIG 实测：err 分支为 switch 形态（cmp/je ×2 + jmp default，case 块后置）
        switch (err)
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
            DNF_LOG_SCOPE_AT(__FUNCTION__, 0xb6, "./log/UdpErr", "Error connection reset - host not reachable\n");
            break;
        }
        default:
        {
            printf("Hm! Time out Or Socket Error = %d\n", err);
            break;
        }
        }
        return 0;
    }
    if (*len < 1)
    {
        printf("Socket closed? Recv size = %d\n", *len);
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0xc6, "./log/UdpErr", "Socket closed? Recv size = %d\n", *len);
        return 0;
    }
    // ORIG 实测：sockaddr 端口在 from+2（-0x52(%ebp)）、地址在 from+4（-0x50）
    *port = ntohs(from.sin_port);
    *ip = ntohl(from.sin_addr.s_addr);
    char* ipstr = inet_ntoa(from.sin_addr);
    char* unused_buf = buf;
    (void)unused_buf;
    // ORIG 实测：包 ID 判断与日志实参均经 unused_buf 局部（-0x20(%ebp)）
    if (*(unsigned short*)unused_buf == 0x4c8 || *(unsigned short*)unused_buf == 0x4c9 ||
        *(unsigned short*)unused_buf == 0x44f || *(unsigned short*)unused_buf == 0x450)
    {
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0xd1,"./log/Udp", "PacketId(%d) Recv success! IP = %s, Port %d, Recv size = %d",
            *(unsigned short*)unused_buf, ipstr, *port, *len);
    }
    buf[*len] = '\0';
    return 1;
}

bool CUdpHandler::SendToClient(char* buf, int len, unsigned short port, char const* ip,
                              unsigned int ipaddr) const
{
    if (ip == 0 && ipaddr == 0)
    {
        return 0;
    }
    if (ip != 0)
    {
        ipaddr = inet_addr(ip);
    }
    if (m_sock == -1)
    {
        return 0;
    }
    int n;
    if (port == 0)
    {
        if (ipaddr == 0)
        {
            n = send(m_sock, buf, len, 0);
        }
    }
    else
    {
        sockaddr_in to;
        memset(&to, 0, 0x10);
        to.sin_family = 2;
        to.sin_port = htons(port);
        to.sin_addr.s_addr = ntohl(ipaddr);
        n = sendto(m_sock, buf, len, 0, (sockaddr*)&to, 0x10);
    }
    if (n == -1)
    {
        int err = getErrno();
        // ORIG 实测（0x804d20e）：err 分支为 switch 决策树形态
        // （cmp $0x61;je + cmp $0x61;jl + sub $0x6f;cmp $0x2;ja，default 尾置），
        // case 按 0x6f-0x71 → 0x61 → default 顺序书写可复现块布局。
        switch (err)
        {
        case 0x6f:
        case 0x70:
        case 0x71:
            printf("Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
            DNF_LOG_SCOPE_AT(__FUNCTION__, 0x113, "./log/UdpErr",
                "Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
            break;
        case 0x61:
            puts("err EAFNOSUPPORT in send");
            DNF_LOG_SCOPE_AT(__FUNCTION__, 0x119, "./log/UdpErr", "Error( EAFNOSUPPORT ) in send = %d\n", err);
            break;
        default:
            printf("err = %d , strerror = %s in send\n", err, strerror(err));
            DNF_LOG_SCOPE_AT(__FUNCTION__, 0x11f, "./log/UdpErr", "err = %d , strerror = %s in send\n", err, strerror(err));
            break;
        }
        return 0;
    }
    if (n == 0)
    {
        puts("no data sent in send");
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x128, "./log/UdpErr", "no data sent in send\n");
        return 0;
    }
    if (len != n)
    {
        printf("Only %s out of %d bytes sent\n", n, len);
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x133, "./log/UdpErr", "Only %s out of %d bytes sent\n", n, len);
        return 0;
    }
    return 1;
}

bool CUdpHandler::RecvFromServer(char* buf, int* len, unsigned int* ip,
                                unsigned short* port) const
{
    if (m_clientSock == -1)
    {
        return 0;
    }
    sockaddr_in from;
    int slen = 0x10;
    // ORIG 实测（0x804d457）：recvfrom 返回值直存 *len（mov %eax,(%edx)），
    // 后续 -1/<1 判断均从 *len 重载（mov (%eax),%eax; cmp $0xffffffff）。
    *len = recvfrom(m_clientSock, buf, *len, 0, (sockaddr*)&from, (socklen_t*)&slen);
    if (*len == -1)
    {
        int err = getErrno();
        // ORIG 实测（0x804d472）：err 载入 eax 后两次 cmp+je（switch 形态）。
        switch (err)
        {
        case 0x58:
            puts("Error fd not a socket");
            DNF_LOG_SCOPE_AT(__FUNCTION__, 0x156, "./log/UdpErr", "Error fd not a socket\n");
            break;
        case 0x68:
            puts("Error connection reset - host not reachable");
            DNF_LOG_SCOPE_AT(__FUNCTION__, 0x15d, "./log/UdpErr", "Error connection reset - host not reachable\n");
            break;
        default:
            printf("Hm! Time out Or Socket Error = %d\n", err);
            break;
        }
        return 0;
    }
    if (*len < 1)
    {
        printf("Socket closed? Recv size = %d\n", *len);
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x16d, "./log/UdpErr", "Socket closed? Recv size = %d\n", *len);
        return 0;
    }
    // ORIG 实测（0x804d590/0x804d5a5）：from 以固定槽位形态访问
    // （movzwl -0x52(%ebp) / mov -0x50(%ebp)），数组下标形式可复现。
    *port = ntohs(from.sin_port);
    *ip = ntohl(from.sin_addr.s_addr);
    char* ipstr = inet_ntoa(from.sin_addr);
    char* unused_buf = buf;
    (void)unused_buf;
    if (*(unsigned short*)unused_buf == 0x4c8 || *(unsigned short*)unused_buf == 0x4c9)
    {
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x179,"./log/Udp", "PacketId(%d) Recv success! IP = %s, Port %d, Recv size = %d",
            *(unsigned short*)unused_buf, ipstr, *port, *len);
    }
    buf[*len] = '\0';
    return 1;
}

bool CUdpHandler::SendToServer(char* buf, int len, unsigned short port, char const* ip) const
{
    if (m_clientSock == -1)
    {
        return 0;
    }
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
        sockaddr_in to;
        memset((char*)&to, 0, 0x10);
        to.sin_family = 2;
        to.sin_port = htons(port);
        to.sin_addr.s_addr = inet_addr(ip);
        n = sendto(m_clientSock, buf, len, 0, (sockaddr*)&to, 0x10);
    }
    if (n == -1)
    {
        int err = getErrno();
        // ORIG 实测（0x804d6ed）：同 SendToClient，switch 决策树形态，
        // case 0x6f-0x71 → 0x61 → default 顺序。
        switch (err)
        {
        case 0x6f:
        case 0x70:
        case 0x71:
            DNF_LOG_SCOPE_LINE(0x1b2, "./log/UdpErr",
                "Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
            break;
        case 0x61:
            DNF_LOG_SCOPE_LINE(0x1b8, "./log/UdpErr", "Error( EAFNOSUPPORT ) in send = %d\n", err);
            break;
        default:
            DNF_LOG_SCOPE_LINE(0x1be, "./log/UdpErr", "err = %d , strerror = %s in send\n", err, strerror(err));
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

void CUdpHandler::InetAddr(char const* ip) const
{
    inet_addr(ip);
}
