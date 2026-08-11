// df_monitor_r — DNFUdpHandler（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "DNFUdpHandler.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "DNFPacketBuffer.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

CUdpHandler::CUdpHandler()
{
    m_sock = -1;
    m_clientSock = -1;
}

CUdpHandler::~CUdpHandler() {}

unsigned int CUdpHandler::InetAddr(const char* ip) const { return inet_addr(ip); }

int CUdpHandler::InitServerSocket(int port)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0x11);
    m_sock = fd;
    if (m_sock == -1)
    {
        printf("Could not create a UDP socket : %d\n", getErrno());
        return -1;
    }
    sockaddr_in addr;
    memset(&addr, 0, 0x10);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0);
    addr.sin_port = htons((unsigned short)port);
    if (bind(m_sock, (sockaddr*)&addr, 0x10) != 0)
    {
        int err = getErrno();
        if (err == 0x62)
        {
            printf("Port %d for receiving UDP is in use\n", port);
        }
        else if (err == 99)
        {
            puts("Cannot assign requested address");
        }
        else if (err != 0)
        {
            printf("Could not bind UDP receive port. Error= %d , strerror = %s\n", err,
                   strerror(err));
        }
        m_sock = -1;
    }
    unsigned int rcvbuf = 0xf4240;
    setsockopt(m_clientSock, SOL_SOCKET, SO_RCVBUF, (char*)&rcvbuf, 4);
    CMyFileLog log(__FUNCTION__, 0x6e);
    log("./log/Udp", "Opened port %d with fd %d, recv buf size %d\n", port, m_sock,
        rcvbuf);
    return m_sock;
}

int CUdpHandler::InitClientSocket()
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0x11);
    m_clientSock = fd;
    if (m_clientSock == -1)
    {
        printf("Could not create a UDP socket : %d\n", getErrno());
        return -1;
    }
    CMyFileLog log(__FUNCTION__, 0x8f);
    log("./log/Udp", "Opened port with fd %d\n", m_clientSock);
    return m_clientSock;
}

char CUdpHandler::RecvFromClient(char* buf, int* size, unsigned int* addr,
                                 unsigned short* port) const
{
    if (m_sock == -1)
    {
        return 0;
    }
    socklen_t len = 0x10;
    sockaddr_in from;
    ssize_t n = recvfrom(m_sock, buf, *size, 0, (sockaddr*)&from, &len);
    *size = n;
    if (*size == -1)
    {
        int err = getErrno();
        if (err == 0x58)
        {
            puts("Error fd not a socket");
            DNF_LOG_SCOPE_AT("RecvFromClient", 0xaf, "./log/UdpErr", "Error fd not a socket\n");
        }
        else if (err == 0x68)
        {
            puts("Error connection reset - host not reachable");
            DNF_LOG_SCOPE_AT("RecvFromClient", 0xb6, "./log/UdpErr", "Error connection reset - host not reachable\n");
        }
        else
        {
            printf("Hm! Time out Or Socket Error = %d\n", err);
        }
        return 0;
    }
    if (*size < 1)
    {
        printf("Socket closed? Recv size = %d\n", *size);
        DNF_LOG_SCOPE_AT("RecvFromClient", 0xc6, "./log/UdpErr", "Socket closed? Recv size = %d\n", *size);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *addr = ntohl(from.sin_addr.s_addr);
    char* ip = inet_ntoa(from.sin_addr);
    if (*(short*)buf == 0x4c8 || *(short*)buf == 0x4c9 || *(short*)buf == 0x44f ||
        *(short*)buf == 0x450)
    {
        DNF_LOG_SCOPE_AT("RecvFromClient", 0xd1,"./log/Udp", "PacketId(%d) Recv success! IP = %s, Port %d, Recv size = %d",
            *(unsigned short*)buf, ip, *port, *size);
    }
    buf[*size] = '\0';
    return 1;
}

int CUdpHandler::SendToClient(char* buf, int len, unsigned short port, const char* ip,
                              unsigned int addr) const
{
    if (ip == 0 && addr == 0)
    {
        return 0;
    }
    if (ip != 0)
    {
        addr = inet_addr(ip);
    }
    if (m_sock == -1)
    {
        return 0;
    }
    int sent;
    if (port == 0)
    {
        if (addr == 0)
        {
            sent = send(m_sock, buf, len, 0);
        }
    }
    else
    {
        sockaddr_in to;
        memset(&to, 0, 0x10);
        to.sin_family = AF_INET;
        to.sin_port = htons(port);
        to.sin_addr.s_addr = ntohl(addr);
        sent = sendto(m_sock, buf, len, 0, (sockaddr*)&to, 0x10);
    }
    if (sent == -1)
    {
        int err = getErrno();
        if (err == 0x61)
        {
            puts("err EAFNOSUPPORT in send");
            DNF_LOG_SCOPE_AT("SendToClient", 0x119, "./log/UdpErr", "Error( EAFNOSUPPORT ) in send = %d\n", err);
        }
        else if (err < 0x61 || 2 < (unsigned int)(err - 0x6f))
        {
            printf("err = %d , strerror = %s in send\n", err, strerror(err));
            DNF_LOG_SCOPE_AT("SendToClient", 0x11f, "./log/UdpErr", "err = %d , strerror = %s in send\n", err, strerror(err));
        }
        else
        {
            printf("Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
            DNF_LOG_SCOPE_AT("SendToClient", 0x113,"./log/UdpErr",
                "Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
        }
        return 0;
    }
    if (sent == len)
    {
        return 1;
    }
    printf("Only %d out of %d bytes sent\n", sent, len);
    DNF_LOG_SCOPE_AT("SendToClient", 0x133, "./log/UdpErr", "Only %d out of %d bytes sent\n", sent, len);
    return 0;
}

int CUdpHandler::SendToServer(char* buf, int len, unsigned short port, const char* ip) const
{
    if (m_clientSock == -1)
    {
        return 0;
    }
    int sent;
    if (port == 0)
    {
        if (ip == 0)
        {
            sent = send(m_clientSock, buf, len, 0);
        }
    }
    else
    {
        sockaddr_in to;
        memset(&to, 0, 0x10);
        to.sin_family = AF_INET;
        to.sin_port = htons(port);
        to.sin_addr.s_addr = inet_addr(ip);
        sent = sendto(m_clientSock, buf, len, 0, (sockaddr*)&to, 0x10);
    }
    if (sent == -1)
    {
        int err = getErrno();
        if (err == 0x61)
        {
            DNF_LOG_SCOPE_LINE(0x1b8, "./log/UdpErr", "Error( EAFNOSUPPORT ) in send = %d\n", err);
        }
        else if (err < 0x61 || 2 < (unsigned int)(err - 0x6f))
        {
            printf("err = %d , strerror = %s in send\n", err, strerror(err));
            DNF_LOG_SCOPE_LINE(0x1be, "./log/UdpErr", "err = %d , strerror = %s in send\n", err, strerror(err));
        }
        else
        {
            printf("Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
            DNF_LOG_SCOPE_LINE(0x1b2,"./log/UdpErr",
                "Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
        }
        return 0;
    }
    if (sent == len)
    {
        return 1;
    }
    printf("Only %d out of %d bytes sent\n", sent, len);
    DNF_LOG_SCOPE_LINE(0x1d2, "./log/UdpErr", "Only %d out of %d bytes sent\n", sent, len);
    return 0;
}

char CUdpHandler::RecvFromServer(char* buf, int* size, unsigned int* addr,
                                 unsigned short* port) const
{
    if (m_clientSock == -1)
    {
        return 0;
    }
    socklen_t len = 0x10;
    sockaddr_in from;
    ssize_t n = recvfrom(m_clientSock, buf, *size, 0, (sockaddr*)&from, &len);
    *size = n;
    if (*size == -1)
    {
        int err = getErrno();
        if (err == 0x58)
        {
            puts("Error fd not a socket");
            DNF_LOG_SCOPE_AT("RecvFromServer", 0x1e1, "./log/UdpErr", "Error fd not a socket\n");
        }
        else if (err == 0x68)
        {
            puts("Error connection reset - host not reachable");
            DNF_LOG_SCOPE_AT("RecvFromServer", 0x1e8, "./log/UdpErr", "Error connection reset - host not reachable\n");
        }
        else
        {
            printf("Hm! Time out Or Socket Error = %d\n", err);
        }
        return 0;
    }
    if (*size < 1)
    {
        printf("Socket closed? Recv size = %d\n", *size);
        DNF_LOG_SCOPE_AT("RecvFromServer", 0x1f8, "./log/UdpErr", "Socket closed? Recv size = %d\n", *size);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *addr = ntohl(from.sin_addr.s_addr);
    buf[*size] = '\0';
    return 1;
}
