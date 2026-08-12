// df_dbmw_r - DNFUdpHandler (ORIG DNFUdpHandler.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

CUdpHandler::CUdpHandler()
{
    m_sock = -1;
    m_clientSock = -1;
}
int CUdpHandler::InitServerSocket(int port)
{
    m_sock = socket(AF_INET, SOCK_DGRAM, 0x11);
    if (m_sock == -1)
    {
        int e = getErrno();
        printf("Could not create a UDP socket : %d\n", e);
        return -1;
    }
    struct sockaddr_in addr;
    memset(&addr, 0, 0x10);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0);
    addr.sin_port = htons(port);
    if (::bind(m_sock, (struct sockaddr*)&addr, 0x10) != 0)
    {
        int e = getErrno();
        switch (e)
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
            if (e != 0)
                printf("Could not bind UDP receive port. Error= %d , strerror = %s\n",
                       e, strerror(e));
            break;
        }
        m_sock = -1;
    }
    int bufsize = 0xf4240;
    setsockopt(m_clientSock, SOL_SOCKET, SO_RCVBUF, &bufsize, 4);
    CMyFileLog(__FUNCTION__, 0x6e)("./log/Udp",
        "Opened port %d with fd %d, recv buf size %d\n", port, m_sock, bufsize);
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
    register int fd = m_clientSock;
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
        memset((char*)&addr, 0, 0x10);
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
            CMyFileLog(__FUNCTION__, 0x1b2)("./log/UdpErr",
                "Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", e);
            break;
        case 0x61:
            CMyFileLog(__FUNCTION__, 0x1b8)("./log/UdpErr",
                "Error( EAFNOSUPPORT ) in send = %d\n", e);
            break;
        default:
            CMyFileLog(__FUNCTION__, 0x1be)("./log/UdpErr",
                "err = %d , strerror = %s in send\n", e, strerror(e));
            break;
        }
        return 0;
    }
    if (n == 0)
    {
        CMyFileLog(__FUNCTION__, 0x1c7)("./log/UdpErr", "no data sent in send\n");
        return 0;
    }
    if (len != n)
    {
        CMyFileLog(__FUNCTION__, 0x1ce)("./log/UdpErr",
            "Only %d out of %d bytes sent\n", n, len);
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
            CMyFileLog(__FUNCTION__, 0x113)("./log/UdpErr",
                "Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", e);
            break;
        case 0x61:
            puts("err EAFNOSUPPORT in send");
            CMyFileLog(__FUNCTION__, 0x119)("./log/UdpErr",
                "Error( EAFNOSUPPORT ) in send = %d\n", e);
            break;
        default:
            printf("err = %d , strerror = %s in send\n", e, strerror(e));
            CMyFileLog(__FUNCTION__, 0x11f)("./log/UdpErr",
                "err = %d , strerror = %s in send\n", e, strerror(e));
            break;
        }
        return 0;
    }
    if (n == 0)
    {
        puts("no data sent in send");
        CMyFileLog(__FUNCTION__, 0x128)("./log/UdpErr", "no data sent in send\n");
        return 0;
    }
    if (len != n)
    {
        printf("Only %s out of %d bytes sent\n", (char*)n, len);
        CMyFileLog(__FUNCTION__, 0x133)("./log/UdpErr",
            "Only %s out of %d bytes sent\n", (char*)n, len);
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
    int fromLen = 0x10;
    *size = recvfrom(m_sock, buf, *size, 0, (struct sockaddr*)&from,
                     (socklen_t*)&fromLen);
    if (*size == -1)
    {
        int e = getErrno();
        switch (e)
        {
        case 0x58:
            puts("Error fd not a socket");
            CMyFileLog(__FUNCTION__, 0xaf)("./log/UdpErr", "Error fd not a socket\n");
            break;
        case 0x68:
            puts("Error connection reset - host not reachable");
            CMyFileLog(__FUNCTION__, 0xb6)("./log/UdpErr",
                "Error connection reset - host not reachable\n");
            break;
        default:
            printf("Hm! Time out Or Socket Error = %d\n", e);
            break;
        }
        return 0;
    }
    if (*size <= 0)
    {
        printf("Socket closed? Recv size = %d\n", *size);
        CMyFileLog(__FUNCTION__, 0xc6)("./log/UdpErr", "Socket closed? Recv size = %d\n", *size);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *addr = ntohl(from.sin_addr.s_addr);
    char* ipStr = inet_ntoa(from.sin_addr);
    char* pkt = buf;
    if (*(unsigned short*)pkt == 0x4c8 || *(unsigned short*)pkt == 0x4c9 ||
        *(unsigned short*)pkt == 0x44f || *(unsigned short*)pkt == 0x450)
    {
        CMyFileLog(__FUNCTION__, 0xd1)("./log/Udp",
            "PacketId(%d) Recv success! IP = %s, Port %d, Recv size = %d",
            *(unsigned short*)pkt, ipStr, *port, *size);
    }
    buf[*size] = 0;
    return 1;
}
char CUdpHandler::RecvFromServer(char* buf, int* size, unsigned int* addr,
                                 unsigned short* port) const
{
    if (m_clientSock == -1)
        return 0;
    struct sockaddr_in from;
    int fromLen = 0x10;
    *size = recvfrom(m_clientSock, buf, *size, 0, (struct sockaddr*)&from,
                     (socklen_t*)&fromLen);
    if (*size == -1)
    {
        int e = getErrno();
        switch (e)
        {
        case 0x58:
            puts("Error fd not a socket");
            CMyFileLog(__FUNCTION__, 0x156)("./log/UdpErr", "Error fd not a socket\n");
            break;
        case 0x68:
            puts("Error connection reset - host not reachable");
            CMyFileLog(__FUNCTION__, 0x15d)("./log/UdpErr",
                "Error connection reset - host not reachable\n");
            break;
        default:
            printf("Hm! Time out Or Socket Error = %d\n", e);
            break;
        }
        return 0;
    }
    if (*size <= 0)
    {
        printf("Socket closed? Recv size = %d\n", *size);
        CMyFileLog(__FUNCTION__, 0x16d)("./log/UdpErr", "Socket closed? Recv size = %d\n", *size);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *addr = ntohl(from.sin_addr.s_addr);
    char* ipStr = inet_ntoa(from.sin_addr);
    char* pkt = buf;
    if (*(unsigned short*)pkt == 0x4c8 || *(unsigned short*)pkt == 0x4c9)
    {
        CMyFileLog(__FUNCTION__, 0x179)("./log/Udp",
            "PacketId(%d) Recv success! IP = %s, Port %d, Recv size = %d",
            *(unsigned short*)pkt, ipStr, *port, *size);
    }
    buf[*size] = 0;
    return 1;
}
int CUdpHandler::GetServerSocket() { return m_sock; }
unsigned int CUdpHandler::InetAddr(const char* ip) const { return inet_addr(ip); }
