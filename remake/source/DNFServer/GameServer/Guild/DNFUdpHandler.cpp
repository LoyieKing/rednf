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
    m_sock = socket(2, 2, 0x11);
    if (m_sock == -1)
    {
        printf("Could not create a UDP socket : %d\n", getErrno());
        return -1;
    }
    sockaddr local;
    memset(&local, 0, 0x10);
    local.sa_family = 2;
    *(unsigned int*)(local.sa_data + 2) = htonl(0);
    *(unsigned short*)local.sa_data = htons((unsigned short)port);
    if (bind(m_sock, &local, 0x10) != 0)
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
    int bufsize = 1000000;
    setsockopt(m_clientSock, 1, 8, &bufsize, 4);









































    DNF_LOG_SCOPE("./log/Udp", "Opened port %d with fd %d, recv buf size %d\n", port, m_sock, bufsize);
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
    socklen_t slen = 0x10;
    sockaddr from;
    ssize_t r = recvfrom(m_sock, buf, *len, 0, &from, &slen);
    *len = r;
    if (r == -1)
    {
        int err = getErrno();
        if (err == 0x58)
        {
            puts("Error fd not a socket");
            DNF_LOG_SCOPE_LINE(0xaf, "./log/UdpErr", "Error fd not a socket\n");
        }
        else if (err == 0x68)
        {
            puts("Error connection reset - host not reachable");
            DNF_LOG_SCOPE_LINE(0xb6, "./log/UdpErr", "Error connection reset - host not reachable\n");
        }
        else
        {
            printf("Hm! Time out Or Socket Error = %d\n", err);
        }
        return 0;
    }
    if (r < 1)
    {
        printf("Socket closed? Recv size = %d\n", r);
        DNF_LOG_SCOPE_LINE(0xc6, "./log/UdpErr", "Socket closed? Recv size = %d\n", r);
        return 0;
    }
    *port = ntohs(*(unsigned short*)from.sa_data);
    *ip = ntohl(*(unsigned int*)(from.sa_data + 2));
    char* ipStr = inet_ntoa(*(struct in_addr*)(from.sa_data + 2));
    unsigned short pktId = *(unsigned short*)buf;
    if (pktId == 0x4c8 || pktId == 0x4c9 || pktId == 0x44f || pktId == 0x450)
    {
        DNF_LOG_SCOPE_LINE(0xd1, "./log/Udp",
            "PacketId(%d) Recv success! IP = %s, Port %d, Recv size = %d",
            pktId, ipStr, (int)*port, *len);
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
    if (port != 0)
    {
        sockaddr to;
        memset(&to, 0, 0x10);
        to.sa_family = 2;
        *(unsigned short*)to.sa_data = htons(port);
        *(unsigned int*)(to.sa_data + 2) = ntohl(ipaddr);
        sent = sendto(m_sock, buf, len, 0, &to, 0x10);
    }
    else if (ipaddr == 0)
    {
        sent = send(m_sock, buf, len, 0);
    }
    if (sent == -1)
    {
        int err = getErrno();
        switch (err)
        {
        case 0x61:
        {
            puts("err EAFNOSUPPORT in send");
            DNF_LOG_SCOPE_LINE(0x119, "./log/UdpErr", "Error( EAFNOSUPPORT ) in send = %d\n", err);
            break;
        }
        case 0x6f:
        case 0x70:
        case 0x71:
        {
            printf("Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
            DNF_LOG_SCOPE_LINE(0x113, "./log/UdpErr",
                "Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
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
    if (sent != len)
    {
        printf("Only %s out of %d bytes sent\n", len, sent);
        DNF_LOG_SCOPE_LINE(0x133, "./log/UdpErr", "Only %s out of %d bytes sent\n", len, sent);
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
    socklen_t slen = 0x10;
    sockaddr from;
    ssize_t r = recvfrom(m_clientSock, buf, *len, 0, &from, &slen);
    *len = r;
    if (r == -1)
    {
        int err = getErrno();
        if (err == 0x58)
        {
            puts("Error fd not a socket");
            DNF_LOG_SCOPE_LINE(0x156, "./log/UdpErr", "Error fd not a socket\n");
        }
        else if (err == 0x68)
        {
            puts("Error connection reset - host not reachable");
            DNF_LOG_SCOPE_LINE(0x15d, "./log/UdpErr", "Error connection reset - host not reachable\n");
        }
        else
        {
            printf("Hm! Time out Or Socket Error = %d\n", err);
        }
        return 0;
    }
    if (r < 1)
    {
        printf("Socket closed? Recv size = %d\n", r);
        DNF_LOG_SCOPE_LINE(0x16d, "./log/UdpErr", "Socket closed? Recv size = %d\n", r);
        return 0;
    }
    *port = ntohs(*(unsigned short*)from.sa_data);
    *ip = ntohl(*(unsigned int*)(from.sa_data + 2));
    char* ipStr = inet_ntoa(*(struct in_addr*)(from.sa_data + 2));
    unsigned short pktId = *(unsigned short*)buf;
    if (pktId == 0x4c8 || pktId == 0x4c9)
    {
        DNF_LOG_SCOPE_LINE(0x179, "./log/Udp",
            "PacketId(%d) Recv success! IP = %s, Port %d, Recv size = %d",
            pktId, ipStr, (int)*port, *len);
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
    if (port != 0)
    {
        sockaddr to;
        memset(&to, 0, 0x10);
        to.sa_family = 2;
        *(unsigned short*)to.sa_data = htons(port);
        *(unsigned int*)(to.sa_data + 2) = inet_addr(ip);
        sent = sendto(m_clientSock, buf, len, 0, &to, 0x10);
    }
    else if (ip == 0)
    {
        sent = send(m_clientSock, buf, len, 0);
    }
    if (sent == -1)
    {
        int err = getErrno();
        switch (err)
        {
        case 0x61:
        {
            DNF_LOG_SCOPE_LINE(0x1b8, "./log/UdpErr", "Error( EAFNOSUPPORT ) in send = %d\n", err);
            break;
        }
        case 0x6f:
        case 0x70:
        case 0x71:
        {
            DNF_LOG_SCOPE_LINE(0x1b2, "./log/UdpErr",
                "Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
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
    if (sent != len)
    {
        DNF_LOG_SCOPE_LINE(0x1ce, "./log/UdpErr", "Only %d out of %d bytes sent\n", len, sent);
        return 0;
    }
    return 1;
}

void CUdpHandler::InetAddr(char const* ip) const
{
    inet_addr(ip);
}
