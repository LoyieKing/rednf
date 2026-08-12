// df_statics_r — DNFUdpHandler（ORIG DNFUdpHandler.o 拆分）
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "DNFUdpHandler.h"
#include "DNFFileLog.h"

bool SetNonBlock(int fd)
{
    unsigned int flags = fcntl(fd, 3, 0);
    int r = fcntl(fd, 4, flags | 0x800);
    if (r < 0) return 0;
    return 1;
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
        case 0x63:
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
    DNF_LOG_SCOPE_LINE(0x80, "./log/Udp.log", "Opened port %d with fd %d, recv buf size %d\n", port, m_sock, bufsize);
    SetNonBlock(m_sock);
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
    SetNonBlock(m_clientSock);
    DNF_LOG_SCOPE_LINE(0xa6, "./log/Udp", "Opened port with fd %d\n", m_clientSock);
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
    *len = recvfrom(m_sock, buf, *len, 0, (sockaddr*)&from, (socklen_t*)&slen);
    if (*len == -1)
    {
        int err = getErrno();
        switch (err)
        {
        case 0x58:
            puts("Error fd not a socket");
            DNF_LOG_SCOPE_LINE(0xc6, "./log/UdpErr", "Error fd not a socket\n");
            break;
        case 0x68:
            puts("Error connection reset - host not reachable");
            DNF_LOG_SCOPE_LINE(0xcd, "./log/UdpErr", "Error connection reset - host not reachable\n");
            break;
        }
        return 0;
    }
    if (*len < 1)
    {
        printf("Socket closed? Recv size = %d\n", *len);
        DNF_LOG_SCOPE_LINE(0xdd, "./log/UdpErr", "Socket closed? Recv size = %d\n", *len);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *ip = ntohl(from.sin_addr.s_addr);
    buf[*len] = '\0';
    return 1;
}
int CUdpHandler::SendToClient(char* buf, int len, unsigned short port, char const* ip,
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
        switch (err)
        {
        case 0x6f ... 0x71:
            printf("Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
            DNF_LOG_SCOPE_LINE(0x12c, "./log/UdpErr", "Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
            break;
        case 0x61:
            puts("err EAFNOSUPPORT in send");
            DNF_LOG_SCOPE_LINE(0x132, "./log/UdpErr", "Error( EAFNOSUPPORT ) in send = %d\n", err);
            break;
        default:
            printf("err = %d , strerror = %s in send\n", err, strerror(err));
            DNF_LOG_SCOPE_LINE(0x138, "./log/UdpErr", "err = %d , strerror = %s in send\n", err, strerror(err));
            break;
        }
        return 0;
    }
    if (n == 0)
    {
        puts("no data sent in send");
        DNF_LOG_SCOPE_LINE(0x141, "./log/UdpErr", "no data sent in send\n");
        return 0;
    }
    if (len != n)
    {
        printf("Only %d out of %d bytes sent\n", n, len);
        DNF_LOG_SCOPE_LINE(0x148, "./log/UdpErr", "Only %d out of %d bytes sent\n", n, len);
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
    *len = recvfrom(m_clientSock, buf, *len, 0, (sockaddr*)&from, (socklen_t*)&slen);
    if (*len == -1)
    {
        int err = getErrno();
        switch (err)
        {
        case 0x58:
            puts("Error fd not a socket");
            DNF_LOG_SCOPE_LINE(0x16d, "./log/UdpErr", "Error fd not a socket\n");
            break;
        case 0x68:
            puts("Error connection reset - host not reachable");
            DNF_LOG_SCOPE_LINE(0x174, "./log/UdpErr", "Error connection reset - host not reachable\n");
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
        DNF_LOG_SCOPE_LINE(0x184, "./log/UdpErr", "Socket closed? Recv size = %d\n", *len);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *ip = ntohl(from.sin_addr.s_addr);
    buf[*len] = '\0';
    return 1;
}
int CUdpHandler::SendToServer(char* buf, int len, unsigned short port, char const* ip) const
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
        switch (err)
        {
        case 0x6f ... 0x71:
            DNF_LOG_SCOPE_LINE(0x1cb, "./log/UdpErr", "Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
            break;
        case 0x61:
            DNF_LOG_SCOPE_LINE(0x1d1, "./log/UdpErr", "Error( EAFNOSUPPORT ) in send = %d\n", err);
            break;
        default:
            DNF_LOG_SCOPE_LINE(0x1d7, "./log/UdpErr", "err = %d , strerror = %s in send\n", err, strerror(err));
            break;
        }
        return 0;
    }
    if (n == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1e0, "./log/UdpErr", "no data sent in send\n");
        return 0;
    }
    if (len != n)
    {
        DNF_LOG_SCOPE_LINE(0x1e7, "./log/UdpErr", "Only %d out of %d bytes sent\n", n, len);
        return 0;
    }
    return 1;
}
void CUdpHandler::InetAddr(char const* ip) const
{
    inet_addr(ip);
}
__attribute__((weak)) int getErrno()
{
    return errno;
}
