// df_game_r 还原 —— CUdpHandler（UDP 句柄，ORIG 0x8483518 族，8 函数）。
// 参照 docs/class_func_reports/CUdpHandler.md；getErrno 同 TU（ORIG 0x8483e75）。
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "CUdpHandler.h"
#include "GameTypes.h"

int getErrno()
{
    return errno;
}

CUdpHandler::CUdpHandler()
{
    m_serverSocket = -1;
    m_clientSocket = -1;
}

unsigned int CUdpHandler::InetAddr(const char* ip) const
{
    return inet_addr(ip);
}

int CUdpHandler::InitClientSocket()
{
    timeval rcvtimeo;
    int err;
    m_clientSocket = socket(AF_INET, SOCK_DGRAM, 0x11);
    if (m_clientSocket == -1)
    {
        err = getErrno();
        printf("Could not create a UDP socket : %d\n", err);
        return -1;
    }
    rcvtimeo.tv_sec = 10;
    rcvtimeo.tv_usec = 0;
    setsockopt(m_clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&rcvtimeo,
               sizeof(rcvtimeo));
    return m_clientSocket;
}

int CUdpHandler::InitServerSocket(int port)
{
    int err;
    sockaddr_in addr;
    timeval rcvtimeo;
    m_serverSocket = socket(AF_INET, SOCK_DGRAM, 0x11);
    if (m_serverSocket == -1)
    {
        err = getErrno();
        printf("Could not create a UDP socket : %d\n", err);
        return -1;
    }
    memset((char*)&addr, 0, 0x10);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0);
    addr.sin_port = htons((unsigned short)port);
    if (bind(m_serverSocket, (sockaddr*)&addr, 0x10) != 0)
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
        default:
            if (err != 0)
            {
                printf("Could not bind UDP receive port. Error= %d , strerror = %s\n",
                       err, strerror(err));
            }
            break;
        }
        m_serverSocket = -1;
    }
    rcvtimeo.tv_sec = 10;
    rcvtimeo.tv_usec = 0;
    setsockopt(m_serverSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&rcvtimeo,
               sizeof(rcvtimeo));
    return m_serverSocket;
}

bool CUdpHandler::RecvFromClient(char* buf, int* size, unsigned int* addr,
                                unsigned short* port) const
{
    int err;
    sockaddr_in from;
    socklen_t len;
    if (m_serverSocket == -1)
    {
        return 0;
    }
    len = 0x10;
    *size = recvfrom(m_serverSocket, buf, *size, 0, (sockaddr*)&from, &len);
    if (*size == -1)
    {
        err = getErrno();
        switch (err)
        {
        case 0x58:
            puts("Error fd not a socket");
            break;
        case 0x68:
            puts("Error connection reset - host not reachable");
            break;
        default:
            printf("Hm! Time out Or Socket Error = %d\n", err);
            break;
        }
        return 0;
    }
    if (*size < 1)
    {
        printf("Socket closed? Recv size = %d\n", *size);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *addr = ntohl(from.sin_addr.s_addr);
    buf[*size] = '\0';
    return 1;
}

bool CUdpHandler::RecvFromServer(char* buf, int* size, unsigned int* addr,
                                unsigned short* port) const
{
    int err;
    sockaddr_in from;
    socklen_t len;
    if (m_clientSocket == -1)
    {
        return 0;
    }
    len = 0x10;
    *size = recvfrom(m_clientSocket, buf, *size, 0, (sockaddr*)&from, &len);
    if (*size == -1)
    {
        err = getErrno();
        switch (err)
        {
        case 0x58:
            puts("Error fd not a socket");
            break;
        case 0x68:
            puts("Error connection reset - host not reachable");
            break;
        default:
            printf("Hm! Time out Or Socket Error = %d\n", err);
            break;
        }
        return 0;
    }
    if (*size < 1)
    {
        printf("Socket closed? Recv size = %d\n", *size);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *addr = ntohl(from.sin_addr.s_addr);
    buf[*size] = '\0';
    return 1;
}

bool CUdpHandler::SendToClient(char* buf, int len, unsigned short port,
                              const char* ip, unsigned int addr) const
{
    int sent;
    int err;
    if (m_clientSocket == -1)
    {
        cMyTrace trace(__FUNCTION__, 0xe1, 5);
        trace("[%s][%d]", __FUNCTION__, 0xe1);
        return 0;
    }
    if (ip == 0 && addr == 0)
    {
        return 0;
    }
    if (ip != 0)
    {
        addr = inet_addr(ip);
    }
    if (m_serverSocket == -1)
    {
        return 0;
    }
    if (port == 0)
    {
        if (addr == 0)
        {
            sent = send(m_serverSocket, buf, len, 0);
        }
    }
    else
    {
        sockaddr_in to;
        memset(&to, 0, 0x10);
        to.sin_family = AF_INET;
        to.sin_port = htons(port);
        to.sin_addr.s_addr = ntohl(addr);
        sent = sendto(m_serverSocket, buf, len, 0, (sockaddr*)&to, 0x10);
    }
    if (sent == -1)
    {
        err = getErrno();
        switch (err)
        {
        case 0x6f:
        case 0x70:
        case 0x71:
            printf("Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
            break;
        case 0x61:
            puts("err EAFNOSUPPORT in send");
            break;
        default:
            printf("err = %d , strerror = %s in send\n", err, strerror(err));
            break;
        }
        return 0;
    }
    if (sent == 0)
    {
        puts("no data sent in send");
        return 0;
    }
    if (len != sent)
    {
        printf("Only %s out of %d bytes sent\n", sent, len);
        return 0;
    }
    else
    {
        return 1;
    }
}

bool CUdpHandler::SendToServer(char* buf, int len, unsigned short port,
                              const char* ip) const
{
    sockaddr_in to;
    int sent;
    int err;
    if (m_clientSocket == -1)
    {
        cMyTrace trace(__FUNCTION__, 0x194, 5);
        trace("[%s][%d]", __FUNCTION__, 0x194);
        return 0;
    }
    else if (m_clientSocket == -1)
    {
        return 0;
    }
    if (port == 0)
    {
        if (ip == 0)
        {
            sent = send(m_clientSocket, buf, len, 0);
        }
    }
    else
    {
        memset(&to, 0, 0x10);
        to.sin_family = AF_INET;
        to.sin_port = htons(port);
        to.sin_addr.s_addr = inet_addr(ip);
        sent = sendto(m_clientSocket, buf, len, 0, (sockaddr*)&to, 0x10);
    }
    if (sent == -1)
    {
        err = getErrno();
        switch (err)
        {
        case 0x6f:
        case 0x70:
        case 0x71:
        {
            cMyTrace trace(__FUNCTION__, 0x1ca, 5);
            trace("CUdpHandler::SendToServer Error: EHOSTUNREACH");
            break;
        }
        case 0x61:
        {
            cMyTrace trace(__FUNCTION__, 0x1d1, 5);
            trace("CUdpHandler::SendToServer Error: EAFNOSUPPORT");
            break;
        }
        default:
        {
            cMyTrace trace(__FUNCTION__, 0x1d8, 5);
            trace("CUdpHandler::SendToServer Error : other err");
            break;
        }
        }
        return 0;
    }
    if (sent == 0)
    {
        cMyTrace trace(__FUNCTION__, 0x1e2, 5);
        trace("CUdpHandler::SendToServer Error: 0 == s");
        return 0;
    }
    if (len != sent)
    {
        cMyTrace trace(__FUNCTION__, 0x1ea, 5);
        trace("CUdpHandler::SendToServer Error : len != s");
        return 0;
    }
    else
    {
        return 1;
    }
}
