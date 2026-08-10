// df_monitor_r — DNFTcpSocket（从 MonitorTypes/App/Table 拆分）
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

#include "DNFTcpSocket.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

TCPSocket::TCPSocket()
{
    m_fd = -1;
    memset((char*)this + 0x14, 0, 4);
    memset((char*)this + 4, 0, 0x10);
    *(unsigned short*)((char*)this + 0x18) = 0;
}

TCPSocket::~TCPSocket()
{
    close();
}

char TCPSocket::open()
{
    int fd = socket(2, 1, 0);
    m_fd = fd;
    if (m_fd == -1)
    {
        printf("Could not create a TDP socket : %d\n", errno);
    }
    return (char)(m_fd != -1);
}

char TCPSocket::connect(const char* ip, unsigned short port)
{
    sockaddr addr;
    memset(&addr, 0, 0x10);
    addr.sa_family = 2;
    *(unsigned int*)((char*)&addr + 4) = inet_addr(ip);
    *(unsigned short*)((char*)&addr + 2) = htons(port);
    int r = ::connect(m_fd, &addr, 0x10);
    if (r >= 0)
    {
        memcpy((char*)this + 0x14, (char*)&addr + 4, 4);
        *(unsigned short*)((char*)this + 0x18) = *(unsigned short*)((char*)&addr + 2);
    }
    else
    {
        printf("CONNECTION FAIL IP =%s, PORT =%d, reason =%s", ip, (unsigned int)port,
               strerror(errno));
    }
    return (char)(r >= 0);
}

char TCPSocket::setOptNonBlock()
{
    unsigned int flags = fcntl(m_fd, 3, 0);
    int r = fcntl(m_fd, 4, flags | 0x800);
    return (char)(r >= 0);
}

char TCPSocket::bind(unsigned short port, bool flag)
{
    setOptReuseAdrs(true);
    sockaddr addr;
    memset(&addr, 0, 0x10);
    addr.sa_family = 2;
    *(unsigned short*)((char*)&addr + 2) = htons(port);
    int r = ::bind(m_fd, &addr, 0x10);
    if (r < 0)
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

char TCPSocket::listen(int backlog)
{
    int r = ::listen(m_fd, backlog);
    if (r < 0)
    {
        close();
    }
    return (char)(r >= 0);
}

char TCPSocket::pollReadEvent() const
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(m_fd, &readfds);
    timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    int r = select(m_fd + 1, &readfds, 0, 0, &tv);
    if (r < 0)
    {
        printf("pollReadEvent(%s)", strerror(errno));
        return 0;
    }
    return (char)FD_ISSET(m_fd, &readfds);
}

char TCPSocket::pollWriteEvent() const
{
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(m_fd, &writefds);
    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int r = select(2, 0, &writefds, 0, &tv);
    if (r < 0)
    {
        printf("pollWriteEvent(%s)", strerror(errno));
        return 0;
    }
    return (char)FD_ISSET(m_fd, &writefds);
}

char TCPSocket::pollErrorEvent() const
{
    fd_set errfds;
    FD_ZERO(&errfds);
    FD_SET(m_fd, &errfds);
    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int r = select(2, 0, 0, &errfds, &tv);
    if (r < 0)
    {
        printf("pollErrorEvent(%s)", strerror(errno));
        return 0;
    }
    return (char)FD_ISSET(m_fd, &errfds);
}

int TCPSocket::pollReadWriteErrEvent() const
{
    fd_set readfds;
    fd_set writefds;
    fd_set errfds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&errfds);
    FD_SET(m_fd, &readfds);
    FD_SET(m_fd, &writefds);
    FD_SET(m_fd, &errfds);
    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int result = 0;
    int flag = 0;
    result = select(2, &readfds, &writefds, &errfds, &tv);
    if (result < 0)
    {
        printf("pollReadWriteErrEvent(%s)", strerror(errno));
    }
    else if (!FD_ISSET(m_fd, &readfds))
    {
        if (!FD_ISSET(m_fd, &writefds))
        {
            result = flag;
            if (FD_ISSET(m_fd, &errfds))
            {
                flag = 3;
                result = flag;
            }
        }
        else
        {
            flag = 2;
            result = flag;
        }
    }
    else
    {
        flag = 1;
        result = flag;
    }
    return result;
}

char TCPSocket::accept(TCPSocket& sock)
{
    socklen_t len = 0x10;
    int fd = ::accept(m_fd, (sockaddr*)((char*)&sock + 4), &len);
    sock.m_fd = fd;
    if (sock.m_fd == 0)
    {
        FILE* f = fopen("log.txt", "a+");
        if (f != 0)
        {
            fprintf(f, "[TCPSocket::Accept] Accept fail[%d]\n", sock.m_fd);
            fclose(f);
        }
    }
    if (sock.m_fd < 0 || sock.m_fd == -1)
    {
        FILE* f = fopen("log.txt", "a+");
        if (f != 0)
        {
            fprintf(f, "[TCPSocket::Accept] Accept fail[%d]\n", sock.m_fd);
            fclose(f);
        }
        return 0;
    }
    memcpy((char*)&sock + 0x14, (char*)&sock + 8, 4);
    *(unsigned short*)((char*)&sock + 0x18) = *(unsigned short*)((char*)&sock + 6);
    sock.setOptNonBlock();
    return 1;
}

int TCPSocket::getHandle() const { return m_fd; }

int TCPSocket::send(char* buf, int len)
{
    if (buf == 0 || len < 1)
    {
        printf("buf error or size-%d error", len);
        return -1;
    }
    int n = write(m_fd, buf, len);
    if (n < 1)
    {
        if (errno != EAGAIN && errno != EINTR && errno != 0)
        {
            printf("tcp send fail=\'%d\', error =\'%s\'", n, strerror(errno));
            return -1;
        }
        printf("");
        printf("tcp send retry=\'%d\', error =\'%s\'", n, strerror(errno));
        return 0;
    }
    printf("1.tcp send=\'%d\', error =\'%s\'", n, strerror(errno));
    return n;
}

int TCPSocket::recv(char* buf, int len)
{
    if (buf == 0 || len < 1)
    {
        printf("In recv : recv buffer is null");
        return -1;
    }
    int n = read(m_fd, buf, len);
    if (n < 0)
    {
        if (errno == EAGAIN || errno == EINTR || errno == 0)
        {
            return 0;
        }
    }
    else if (n == 0)
    {
        printf("tcp recv : FIN recv, %s", strerror(errno));
        return -1;
    }
    printf("tcp recv =\'%d\'", n);
    return n;
}

void TCPSocket::close()
{
    if (m_fd != -1)
    {
        ::close(m_fd);
        m_fd = -1;
        memset((char*)this + 0x14, 0, 4);
        *(unsigned short*)((char*)this + 0x18) = 0;
    }
}

int TCPSocket::shutdown(int how) { return m_fd; }

char TCPSocket::setOptReuseAdrs(bool flag)
{
    unsigned int opt = (unsigned int)flag;
    int r = setsockopt(m_fd, 1, 2, &opt, 4);
    return (char)(r >= 0);
}

char TCPSocket::setOptLinger(bool flag)
{
    unsigned int opt[2];
    opt[0] = (unsigned int)flag;
    opt[1] = 0;
    int r = setsockopt(m_fd, 1, 0xd, opt, 8);
    return (char)(r >= 0);
}

char* TCPSocket::getPeerAdrs() { return (char*)this + 0x14; }

unsigned short TCPSocket::getPeerPort() { return *(unsigned short*)((char*)this + 0x18); }

char* TCPSocket::getPeerIP()
{
    static char ip[0x10];
    sprintf(ip, "%d.%d.%d.%d", (unsigned int)(unsigned char)*(char*)((char*)this + 0x14),
            (unsigned int)(unsigned char)*(char*)((char*)this + 0x15),
            (unsigned int)(unsigned char)*(char*)((char*)this + 0x16),
            (unsigned int)(unsigned char)*(char*)((char*)this + 0x17));
    return ip;
}

char TCPSocket::setOptResizeSendBuf(int size)
{
    if (size < 1)
    {
        return 0;
    }
    int r = setsockopt(m_fd, 1, 7, &size, 4);
    return (char)(r >= 0);
}

char TCPSocket::setOptResizeRecvBuf(int size)
{
    if (size <= 0)
    {
        return 0;
    }
    int r = setsockopt(m_fd, 1, 8, &size, 4);
    return (char)(r >= 0);
}

