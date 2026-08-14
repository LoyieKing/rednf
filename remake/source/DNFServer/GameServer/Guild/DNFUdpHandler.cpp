// df_guild_r — DNFUdpHandler (split from source/guild per ORIG layout)
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "DNFFileLog.h"

#include "DNFUdpHandler.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFGameServer.h"
#include "DNFGuild.h"
#include "DNFGuildManager.h"
#include "DNFGuildServerMain.h"
#include "DNFGuildWar.h"
#include "DNFInnerMsgHandler.h"
#include "DNFKillUserConfig.h"
#include "DNFManagerServer.h"
#include "DNFMonitorServer.h"
#include "DNFNetworkThread.h"
#include "DNFPacketBuffer.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTracer.h"
#include "DNFPacketTranslater.h"
#include "DNFPowerWarConfig.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFSignal.h"
#include "DNFSignalTranslator.h"
#include "DNFSystemFailSig.h"
#include "DNFTableBase.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "DNFTerminateSig.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"
#include "DNFUserManager.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "GuildCargo.h"
#include "MemPool.h"
#include "MemoryCashManager.h"
#include "PacketCounter.h"
#include "Peer.h"
#include "Power.h"
#include "PowerManager.h"
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

CUdpHandler::~CUdpHandler()
{
    if (m_sock != -1)
    {
        close(m_sock);
    }
    if (m_clientSock != -1)
    {
        close(m_clientSock);
    }
}
CUdpHandler::CUdpHandler()
{
    m_sock = -1;
    m_clientSock = -1;
}

int CUdpHandler::InitServerSocket(int port)
{
    int err;
    int bufsize;
    m_sock = socket(2, 2, 0x11);
    if (m_sock == -1) { err = getErrno(); printf("Could not create a UDP socket : %d\n", err); return -1; }
    sockaddr_in local; memset(&local, 0, 0x10);
    local.sin_family = 2; local.sin_addr.s_addr = htonl(0);
    local.sin_port = htons((unsigned short)port);
    if (bind(m_sock, (sockaddr*)&local, 0x10) != 0) {
        err = getErrno();
        switch (err)
        {
        case 0x62: printf("Port %d for receiving UDP is in use\n", port); break;
        case 0x63: puts("Cannot assign requested address"); break;
        case 0: break;
        default:
            printf("Could not bind UDP receive port. Error= %d , strerror = %s\n", err, strerror(err));
        }
        m_sock = -1;
    }
    bufsize = 1000000; setsockopt(m_clientSock, 1, 8, &bufsize, 4);
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
    DNF_LOG_SCOPE_LINE(0x8f, "./log/Udp", "Opened port with fd %d\n", m_clientSock);
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
    socklen_t slen = 0x10;
    register socklen_t* ps = &slen;
    *len = recvfrom(m_sock, buf, *len, 0, (sockaddr*)&from, ps);
    if (*len == -1)
    {
        int err = getErrno();
        switch (err)
        {
        case 0x58:
            puts("Error fd not a socket");
            DNF_LOG_SCOPE_LINE(0xaf, "./log/UdpErr", "Error fd not a socket\n");
            break;
        case 0x68:
            puts("Error connection reset - host not reachable");
            DNF_LOG_SCOPE_LINE(0xb6, "./log/UdpErr", "Error connection reset - host not reachable\n");
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
        DNF_LOG_SCOPE_LINE(0xc6, "./log/UdpErr", "Socket closed? Recv size = %d\n", *len);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *ip = ntohl(from.sin_addr.s_addr);
    char* ipStr = inet_ntoa(from.sin_addr);
    char* bufPtr = buf;
    if (*(unsigned short*)bufPtr == 0x4c8 || *(unsigned short*)bufPtr == 0x4c9 ||
        *(unsigned short*)bufPtr == 0x44f || *(unsigned short*)bufPtr == 0x450)
    {
        DNF_LOG_SCOPE_LINE(0xd1, "./log/Udp",
            "PacketId(%d) Recv success! IP = %s, Port %d, Recv size = %d",
            *(unsigned short*)bufPtr, ipStr, (int)*port, *len);
    }
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
    int sent;
    if (port == 0)
    {
        if (ipaddr == 0)
        {
            sent = send(m_sock, buf, len, 0);
        }
    }
    else
    {
        sockaddr_in to;
        memset(&to, 0, 0x10);
        to.sin_family = 2;
        to.sin_port = htons(port);
        to.sin_addr.s_addr = ntohl(ipaddr);
        sent = sendto(m_sock, buf, len, 0, (sockaddr*)&to, 0x10);
    }
    if (sent == -1)
    {
        int err = getErrno();
        switch (err)
        {
        case 0x6f:
        case 0x70:
        case 0x71:
        {
            printf("Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
            DNF_LOG_SCOPE_LINE(0x113, "./log/UdpErr",
                "Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
            break;
        }
        case 0x61:
        {
            puts("err EAFNOSUPPORT in send");
            DNF_LOG_SCOPE_LINE(0x119, "./log/UdpErr", "Error( EAFNOSUPPORT ) in send = %d\n", err);
            break;
        }
        default:
        {
            printf("err = %d , strerror = %s in send\n", err, strerror(err));
            DNF_LOG_SCOPE_LINE(0x11f, "./log/UdpErr", "err = %d , strerror = %s in send\n",
                err, strerror(err));
            break;
        }
        }
        return 0;
    }
    if (sent == 0)
    {
        puts("no data sent in send");
        DNF_LOG_SCOPE_LINE(0x128, "./log/UdpErr", "no data sent in send\n");
        return 0;
    }
    if (len != sent)
    {
        printf("Only %s out of %d bytes sent\n", sent, len);
        DNF_LOG_SCOPE_LINE(0x133, "./log/UdpErr", "Only %s out of %d bytes sent\n", sent, len);
        return 0;
    }
    return 1;
}

int CUdpHandler::RecvFromServer(char* buf, int* len, unsigned int* ip,
                                unsigned short* port) const
{
    if (m_clientSock == -1)
    {
        return 0;
    }
    sockaddr_in from;
    socklen_t slen = 0x10;
    register socklen_t* ps = &slen;
    *len = recvfrom(m_clientSock, buf, *len, 0, (sockaddr*)&from, ps);
    if (*len == -1)
    {
        int err = getErrno();
        switch (err)
        {
        case 0x58:
            puts("Error fd not a socket");
            DNF_LOG_SCOPE_LINE(0x156, "./log/UdpErr", "Error fd not a socket\n");
            break;
        case 0x68:
            puts("Error connection reset - host not reachable");
            DNF_LOG_SCOPE_LINE(0x15d, "./log/UdpErr", "Error connection reset - host not reachable\n");
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
        DNF_LOG_SCOPE_LINE(0x16d, "./log/UdpErr", "Socket closed? Recv size = %d\n", *len);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *ip = ntohl(from.sin_addr.s_addr);
    char* ipStr = inet_ntoa(from.sin_addr);
    char* bufPtr = buf;
    if (*(unsigned short*)bufPtr == 0x4c8 || *(unsigned short*)bufPtr == 0x4c9)
    {
        DNF_LOG_SCOPE_LINE(0x179, "./log/Udp",
            "PacketId(%d) Recv success! IP = %s, Port %d, Recv size = %d",
            *(unsigned short*)bufPtr, ipStr, (int)*port, *len);
    }
    buf[*len] = '\0';
    return 1;
}

int CUdpHandler::SendToServer(char* buf, int len, unsigned short port, char const* ip) const
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
        to.sin_family = 2;
        to.sin_port = htons(port);
        to.sin_addr.s_addr = inet_addr(ip);
        sent = sendto(m_clientSock, buf, len, 0, (sockaddr*)&to, 0x10);
    }
    if (sent == -1)
    {
        int err = getErrno();
        switch (err)
        {
        case 0x6f:
        case 0x70:
        case 0x71:
        {
            DNF_LOG_SCOPE_LINE(0x1b2, "./log/UdpErr",
                "Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
            break;
        }
        case 0x61:
        {
            DNF_LOG_SCOPE_LINE(0x1b8, "./log/UdpErr", "Error( EAFNOSUPPORT ) in send = %d\n", err);
            break;
        }
        default:
        {
            DNF_LOG_SCOPE_LINE(0x1be, "./log/UdpErr", "err = %d , strerror = %s in send\n",
                err, strerror(err));
            break;
        }
        }
        return 0;
    }
    if (sent == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1c7, "./log/UdpErr", "no data sent in send\n");
        return 0;
    }
    if (len != sent)
    {
        DNF_LOG_SCOPE_LINE(0x1ce, "./log/UdpErr", "Only %d out of %d bytes sent\n", sent, len);
        return 0;
    }
    return 1;
}

void CUdpHandler::InetAddr(char const* ip) const
{
    inet_addr(ip);
}
