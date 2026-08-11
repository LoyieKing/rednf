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
CUdpHandler::~CUdpHandler() {}
int CUdpHandler::InitServerSocket(int port)
{
    m_sock = socket(AF_INET, SOCK_DGRAM, 0x11);
    if (m_sock == -1)
    {
        printf("Could not create a UDP socket : %d\n", getErrno());
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
        if (e == 0x62)
            printf("Port %d for receiving UDP is in use\n", port);
        else if (e == 0x63)
            puts("Cannot assign requested address");
        else if (e != 0)
            printf("Could not bind UDP receive port. Error= %d , strerror = %s\n",
                   e, strerror(e));
        m_sock = -1;
    }
    int bufsize = 0xf4240;
    setsockopt(m_clientSock, SOL_SOCKET, SO_RCVBUF, &bufsize, 4);
    CMyFileLog log(__FUNCTION__, 0x6e);
    log("./log/Udp", "Opened port %d with fd %d, recv buf size %d\n", port, m_sock, bufsize);
    return m_sock;
}
int CUdpHandler::InitClientSocket()
{
    m_clientSock = socket(AF_INET, SOCK_DGRAM, 0x11);
    if (m_clientSock == -1)
    {
        printf("udp client socket error : %d", getErrno());
        return -1;
    }
    CMyFileLog log(__FUNCTION__, 0x8f);
    log("./log/UdpClient", "udp client socket = %d", m_clientSock);
    return m_clientSock;
}
int CUdpHandler::SendToServer(char* buf, int len, unsigned short port, const char* ip) const
{
    if (m_clientSock == -1)
        return 0;
    int n;
    if (port == 0 && ip == 0)
    {
        n = send(m_clientSock, buf, len, 0);
    }
    else
    {
        struct sockaddr_in addr;
        memset(&addr, 0, 0x10);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip);
        n = sendto(m_clientSock, buf, len, 0, (struct sockaddr*)&addr, 0x10);
    }
    if (n == -1)
    {
        int e = getErrno();
        if (e == 0x61)
        {
            CMyFileLog log(__FUNCTION__, 0x1b8);
            log("./log/UdpErr", "Error( EAFNOSUPPORT ) in send = %d\n", e);
        }
        else if (e >= 0x6f && e <= 0x71)
        {
            CMyFileLog log(__FUNCTION__, 0x1b2);
            log("./log/UdpErr", "Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", e);
        }
        else
        {
            CMyFileLog log(__FUNCTION__, 0x1be);
            log("./log/UdpErr", "err = %d , strerror = %s in send\n", e, strerror(e));
        }
        return 0;
    }
    if (n == 0)
    {
        CMyFileLog log(__FUNCTION__, 0x1c7);
        log("./log/UdpErr", "no data sent in send\n");
        return 0;
    }
    if (n != len)
    {
        CMyFileLog log(__FUNCTION__, 0x1ce);
        log("./log/UdpErr", "Only %d out of %d bytes sent\n", n, len);
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
    if (port == 0 && addr == 0)
    {
        n = send(m_sock, buf, len, 0);
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
        if (e == 0x61)
        {
            puts("err EAFNOSUPPORT in send");
            CMyFileLog log(__FUNCTION__, 0x119);
            log("./log/UdpErr", "Error( EAFNOSUPPORT ) in send = %d\n", e);
        }
        else if (e >= 0x6f && e <= 0x71)
        {
            printf("Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", e);
            CMyFileLog log(__FUNCTION__, 0x113);
            log("./log/UdpErr", "Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", e);
        }
        else
        {
            printf("err = %d , strerror = %s in send\n", e, strerror(e));
            CMyFileLog log(__FUNCTION__, 0x11f);
            log("./log/UdpErr", "err = %d , strerror = %s in send\n", e, strerror(e));
        }
        return 0;
    }
    if (n == 0)
    {
        puts("no data sent in send");
        CMyFileLog log(__FUNCTION__, 0x128);
        log("./log/UdpErr", "no data sent in send\n");
        return 0;
    }
    if (n != len)
    {
        printf("Only %s out of %d bytes sent\n", (const char*)n, len);
        CMyFileLog log(__FUNCTION__, 0x133);
        log("./log/UdpErr", "Only %d out of %d bytes sent\n", n, len);
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
    socklen_t len = 0x10;
    *size = recvfrom(m_sock, buf, *size, 0, (struct sockaddr*)&from, &len);
    if (*size == -1)
    {
        int e = getErrno();
        if (e == 0x58)
        {
            puts("Error fd not a socket");
            CMyFileLog log(__FUNCTION__, 0xaf);
            log("./log/UdpErr", "Error fd not a socket\n");
        }
        else if (e == 0x68)
        {
            puts("Error connection reset - host not reachable");
            CMyFileLog log(__FUNCTION__, 0xb6);
            log("./log/UdpErr", "Error connection reset - host not reachable\n");
        }
        else
        {
            printf("Hm! Time out Or Socket Error = %d\n", e);
        }
        return 0;
    }
    if (*size <= 0)
    {
        printf("Socket closed? Recv size = %d\n", *size);
        CMyFileLog log(__FUNCTION__, 0xc6);
        log("./log/UdpErr", "Socket closed? Recv size = %d\n", *size);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *addr = ntohl(from.sin_addr.s_addr);
    if (*(unsigned short*)buf == 0x4c8 || *(unsigned short*)buf == 0x4c9 ||
        *(unsigned short*)buf == 0x44f || *(unsigned short*)buf == 0x450)
    {
        CMyFileLog log(__FUNCTION__, 0xd1);
        log("./log/Udp", "PacketId(%d) Recv success! IP = %s, Port %d, Recv size = %d",
            *(unsigned short*)buf, inet_ntoa(from.sin_addr), *port, *size);
        buf[*size] = 0;
        return 1;
    }
    return 0;
}
char CUdpHandler::RecvFromServer(char* buf, int* size, unsigned int* addr,
                                 unsigned short* port) const
{
    if (m_clientSock == -1)
        return 0;
    struct sockaddr_in from;
    socklen_t len = 0x10;
    *size = recvfrom(m_clientSock, buf, *size, 0, (struct sockaddr*)&from, &len);
    if (*size == -1)
    {
        int e = getErrno();
        if (e == 0x58)
        {
            puts("Error fd not a socket");
            CMyFileLog log(__FUNCTION__, 0x156);
            log("./log/UdpErr", "Error fd not a socket\n");
        }
        else if (e == 0x68)
        {
            puts("Error connection reset - host not reachable");
            CMyFileLog log(__FUNCTION__, 0x15d);
            log("./log/UdpErr", "Error connection reset - host not reachable\n");
        }
        else
        {
            printf("Hm! Time out Or Socket Error = %d\n", e);
        }
        return 0;
    }
    if (*size <= 0)
    {
        printf("Socket closed? Recv size = %d\n", *size);
        CMyFileLog log(__FUNCTION__, 0x16d);
        log("./log/UdpErr", "Socket closed? Recv size = %d\n", *size);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *addr = ntohl(from.sin_addr.s_addr);
    if (*(unsigned short*)buf == 0x4c8 || *(unsigned short*)buf == 0x4c9 ||
        *(unsigned short*)buf == 0x44f || *(unsigned short*)buf == 0x450)
    {
        CMyFileLog log(__FUNCTION__, 0x179);
        log("./log/Udp", "PacketId(%d) Recv success! IP = %s, Port %d, Recv size = %d",
            *(unsigned short*)buf, inet_ntoa(from.sin_addr), *port, *size);
        buf[*size] = 0;
        return 1;
    }
    return 0;
}
int CUdpHandler::GetServerSocket() { return m_sock; }
unsigned int CUdpHandler::InetAddr(const char* ip) const { return inet_addr(ip); }
