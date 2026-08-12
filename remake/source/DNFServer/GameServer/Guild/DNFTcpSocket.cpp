// df_guild_r — DNFTcpSocket (split from source/guild per ORIG layout)
#include <string.h>
#include <time.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include "tinyxml.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel_Next.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel_Next.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "DNFTcpSocket.h"
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
#include "DNFTerminateSig.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFUdpHandler.h"
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

TCPSocket::TCPSocket()
{
    m_sock = -1;
    memset(m_peerAdrs, 0, 4);
    memset(m_sockaddr, 0, 0x10);
    m_peerPort = 0;
}

TCPSocket::~TCPSocket()
{
    close();
}

bool TCPSocket::open()
{
    m_sock = socket(2, 1, 0);
    if (m_sock == -1)
    {
        int err = errno;
        printf("Could not create a TDP socket : %d\n", err);
        return 0;
    }
    return 1;
}

bool TCPSocket::bind(unsigned short port, bool flag)
{
    setOptReuseAdrs(true);
    sockaddr_in local;
    memset(&local, 0, 0x10);
    local.sin_family = 2;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = 0;
    if ((unsigned)::bind(m_sock, (sockaddr*)&local, 0x10) >> 31)
    {
        close();
        return 0;
    }
    if (flag)
    {
        setOptNonBlock();
    }
    printf("succeeded in binding TCP socket port #%d\n", (unsigned int)port);
    return 1;
}

bool TCPSocket::listen(int backlog)
{
    if ((unsigned)::listen(m_sock, backlog) >> 31)
    {
        close();
        return 0;
    }
    return 1;
}

int TCPSocket::send(char* buf, int len)
{
    if (buf == 0 || len < 1)
    {
        printf("buf error or size-%d error", len);
        return -1;
    }
    int r = write(m_sock, buf, len);
    if (r < 1)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN || errno == 0)
        {
            printf("\xbf\xa9\xb1\xe2 \xb0\xc9\xb8\xae\xb8\xe9\xbc\xad errno \xb0\xa1 0 \xc0\xcc\xb8\xe9 \xb9\xae\xc1\xa6 \xb9\xdf\xbb\xfd \xc7\xd1\xb4\xd9 !!!! \xb2\xc0 \xc8\xae\xc0\xce!!!");
            printf("tcp send retry='%d', error ='%s'", r, strerror(errno));
            return 0;
        }
        printf("tcp send fail='%d', error ='%s'", r, strerror(errno));
        return -1;
    }
    printf("1.tcp send='%d', error ='%s'", r, strerror(errno));
    return r;
}

int TCPSocket::recv(char* buf, int len)
{
    if (buf == 0 || len < 1)
    {
        printf("In recv : recv buffer is null");
        return -1;
    }
    int r = read(m_sock, buf, len);
    if (r < 0)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN || errno == 0)
        {
            return 0;
        }
    }
    else if (r == 0)
    {
        printf("tcp recv : FIN recv, %s", strerror(errno));
        return -1;
    }
    printf("tcp recv ='%d'", r);
    return r;
}

int TCPSocket::getHandle() const
{
    return m_sock;
}

int TCPSocket::shutdown(int how)
{
    m_sock == -1;
    return m_sock;
}

void TCPSocket::close()
{
    if (m_sock != -1)
    {
        ::close(m_sock);
        m_sock = -1;
        memset(m_peerAdrs, 0, 4);
        m_peerPort = 0;
    }
    return;
}

bool TCPSocket::setOptNonBlock()
{
    int flags = fcntl(m_sock, 3, 0);
    flags |= 0x800;
    if ((unsigned)fcntl(m_sock, 4, flags) >> 31) return 0;
    return 1;
}

bool TCPSocket::setOptReuseAdrs(bool flag)
{
    int v = 0;
    if (flag) v = 1; else v = 0;
    if ((unsigned)setsockopt(m_sock, 1, 2, &v, 4) >> 31) return 0;
    return 1;
}

bool TCPSocket::setOptLinger(bool flag)
{
    int v[2];
    v[0] = flag ? 1 : 0;
    v[1] = 0;
    if ((unsigned)setsockopt(m_sock, 1, 0xd, v, 8) >> 31) return 0;
    return 1;
}

bool TCPSocket::connect(const char* ip, unsigned short port)
{
    sockaddr_in local;
    memset(&local, 0, 0x10);
    local.sin_family = 2;
    local.sin_addr.s_addr = inet_addr(ip);
    local.sin_port = htons(port);
    int len = 0x10;
    if ((unsigned)::connect(m_sock, (sockaddr*)&local, len) >> 31)
    {
        printf("CONNECTION FAIL IP =%s, PORT =%d, reason =%s", ip, (unsigned int)port,
               strerror(errno));
        return 0;
    }
    memcpy(m_peerAdrs, &local.sin_addr, 4);
    m_peerPort = local.sin_port;
    return 1;
}

bool TCPSocket::pollReadEvent() const
{
    fd_set fds;
    timeval tv;
    int n;
    unsigned int i;
    fd_set* p = &fds;
    for (i = 0; i < 0x20; i++)
    {
        p->fds_bits[i] = 0;
    }
    fds.fds_bits[(unsigned int)m_sock >> 5] =
        fds.fds_bits[(unsigned int)m_sock >> 5] | (1 << ((unsigned int)m_sock & 0x1f));
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    n = 0;
    if ((unsigned)(n = select(m_sock + 1, &fds, 0, 0, &tv)) >> 31)
    {
        printf("pollReadEvent(%s)", strerror(errno));
        return 0;
    }
    return (fds.fds_bits[(unsigned int)m_sock >> 5] >> ((unsigned int)m_sock & 0x1f)) & 1;
}

bool TCPSocket::pollWriteEvent() const
{
    fd_set fds;
    timeval tv;
    int n;
    unsigned int i;
    fd_set* p = &fds;
    for (i = 0; i < 0x20; i++)
    {
        p->fds_bits[i] = 0;
    }
    fds.fds_bits[(unsigned int)m_sock >> 5] =
        fds.fds_bits[(unsigned int)m_sock >> 5] | (1 << ((unsigned int)m_sock & 0x1f));
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    n = 0;
    if ((unsigned)(n = select(2, 0, &fds, 0, &tv)) >> 31)
    {
        printf("pollWriteEvent(%s)", strerror(errno));
        return 0;
    }
    return (fds.fds_bits[(unsigned int)m_sock >> 5] >> ((unsigned int)m_sock & 0x1f)) & 1;
}

bool TCPSocket::pollErrorEvent() const
{
    fd_set fds;
    timeval tv;
    int n;
    unsigned int i;
    fd_set* p = &fds;
    for (i = 0; i < 0x20; i++)
    {
        p->fds_bits[i] = 0;
    }
    fds.fds_bits[(unsigned int)m_sock >> 5] =
        fds.fds_bits[(unsigned int)m_sock >> 5] | (1 << ((unsigned int)m_sock & 0x1f));
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    n = 0;
    if ((unsigned)(n = select(2, 0, 0, &fds, &tv)) >> 31)
    {
        printf("pollErrorEvent(%s)", strerror(errno));
        return 0;
    }
    return (fds.fds_bits[(unsigned int)m_sock >> 5] >> ((unsigned int)m_sock & 0x1f)) & 1;
}

bool TCPSocket::accept(TCPSocket& peer)
{
    socklen_t slen = 0x10;
    peer.m_sock = ::accept(m_sock, (sockaddr*)peer.m_sockaddr, &slen);
    if (peer.m_sock == 0)
    {
        FILE* f = fopen("log.txt", "a+");
        if (f != 0)
        {
            fprintf(f, "[TCPSocket::Accept] Accept fail[%d]\n", peer.m_sock);
            fclose(f);
        }
    }
    if (peer.m_sock < 0 || peer.m_sock == -1)
    {
        FILE* f = fopen("log.txt", "a+");
        if (f != 0)
        {
            fprintf(f, "[TCPSocket::Accept] Accept fail[%d]\n", peer.m_sock);
            fclose(f);
        }
        return 0;
    }
    memcpy(peer.m_peerAdrs, (char*)&peer + 8, 4);
    peer.m_peerPort = *(unsigned short*)((char*)&peer + 6);
    peer.setOptNonBlock();
    return 1;
}

int TCPSocket::setOptResizeSendBuf(int size)
{
    if (size < 1)
    {
        return 0;
    }
    int b, a;
    a = 0;
    b = 4;
    int r = setsockopt(m_sock, 1, 7, &size, 4);
    if (r >> 31) return 0;
    return 1;
}

int TCPSocket::setOptResizeRecvBuf(int size)
{
    if (size < 1)
    {
        return 0;
    }
    int r = setsockopt(m_sock, 1, 8, &size, 4);
    if (r >> 31) return 0;
    return 1;
}

int TCPSocket::pollReadWriteErrEvent() const
{
    fd_set rfds;
    fd_set wfds;
    fd_set efds;
    for (unsigned int i = 0; i < 0x20; i++)
    {
        rfds.fds_bits[i] = 0;
        wfds.fds_bits[i] = 0;
        efds.fds_bits[i] = 0;
    }
    rfds.fds_bits[(unsigned int)m_sock >> 5] =
        (1 << ((unsigned int)m_sock & 0x1f)) | rfds.fds_bits[(unsigned int)m_sock >> 5];
    wfds.fds_bits[(unsigned int)m_sock >> 5] =
        (1 << ((unsigned int)m_sock & 0x1f)) | wfds.fds_bits[(unsigned int)m_sock >> 5];
    efds.fds_bits[(unsigned int)m_sock >> 5] =
        (1 << ((unsigned int)m_sock & 0x1f)) | efds.fds_bits[(unsigned int)m_sock >> 5];
    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int result = 0;
    int r = select(2, &rfds, &wfds, &efds, &tv);
    if (r < 0)
    {
        printf("pollReadWriteErrEvent(%s)", strerror(errno));
    }
    else if ((rfds.fds_bits[(unsigned int)m_sock >> 5] >> ((unsigned int)m_sock & 0x1f) & 1U) == 0)
    {
        if ((wfds.fds_bits[(unsigned int)m_sock >> 5] >> ((unsigned int)m_sock & 0x1f) & 1U) == 0)
        {
            if ((efds.fds_bits[(unsigned int)m_sock >> 5] >> ((unsigned int)m_sock & 0x1f) & 1U) != 0)
            {
                result = 3;
            }
        }
        else
        {
            result = 2;
        }
    }
    else
    {
        result = 1;
    }
    return result;
}

char* TCPSocket::getPeerIP()
{
    static char ip[16];
    sprintf(ip, "%d.%d.%d.%d", (unsigned int)(unsigned char)m_peerAdrs[0],
            (unsigned int)(unsigned char)m_peerAdrs[1], (unsigned int)(unsigned char)m_peerAdrs[2],
            (unsigned int)(unsigned char)m_peerAdrs[3]);
    return ip;
}

char* TCPSocket::getPeerAdrs()
{
    return m_peerAdrs;
}

unsigned short TCPSocket::getPeerPort()
{
    return m_peerPort;
}
