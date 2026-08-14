// df_monitor_r — DNFTcpSocket（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
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
    memset(m_peerIp, 0, 4);
    memset(&m_addr, 0, 0x10);
    m_peerPort = 0;
}

TCPSocket::~TCPSocket()
{
    close();
}

char TCPSocket::open()
{
    m_fd = socket(2, 1, 0);
    if (m_fd == -1)
    {
        int err = errno;
        printf("Could not create a TDP socket : %d\n", err);
        return 0;
    }
    return 1;
}

char TCPSocket::connect(const char* ip, unsigned short port)
{
    sockaddr_in addr;
    memset(&addr, 0, 0x10);
    addr.sin_family = 2;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    int len = 0x10;
    if (::connect(m_fd, (sockaddr*)&addr, len) < 0)
    {
        printf("CONNECTION FAIL IP =%s, PORT =%d, reason =%s", ip, (unsigned int)port,
               strerror(errno));
        return 0;
    }
    memcpy(m_peerIp, (void*)((char*)&addr + 4), 4);
    m_peerPort = addr.sin_port;
    return 1;
}

char TCPSocket::setOptNonBlock()
{
    unsigned int flags = fcntl(m_fd, 3, 0);
    flags |= 0x800;
    if (fcntl(m_fd, 4, flags) < 0)
    {
        return 0;
    }
    return 1;
}

char TCPSocket::bind(unsigned short port, bool flag)
{
    setOptReuseAdrs(true);
    sockaddr_in addr;
    memset(&addr, 0, 0x10);
    addr.sin_family = 2;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = 0;
    if (::bind(m_fd, (sockaddr*)&addr, 0x10) < 0)
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
    if (::listen(m_fd, backlog) < 0)
    {
        close();
        return 0;
    }
    return 1;
}

char TCPSocket::pollReadEvent() const
{
    fd_set readfds;
    // ORIG 用 RHEL5 风格 FD_ZERO 循环（__arr/__i 槽位 + setbe 终止）；
    // 当前 RHEL6 头 FD_ZERO 展开为 rep stos，按 ORIG 显式循环复刻。
    {
        unsigned int __i;
        fd_set* __arr = &readfds;
        for (__i = 0; __i < 32; ++__i) __arr->fds_bits[__i] = 0;
    }
    FD_SET((unsigned int)m_fd, &readfds);  // ORIG：shr 逻辑移位（无符号）
    timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    int r = 0;
    if ((r = select(m_fd + 1, &readfds, 0, 0, &tv)) < 0)
    {
        printf("pollReadEvent(%s)", strerror(errno));
        return 0;
    }
    // ORIG FD_ISSET 形态：(fds_bits[fd>>5] >> (fd&31)) & 1（sar）
    return (char)((readfds.fds_bits[(unsigned int)m_fd >> 5] >>
                   ((unsigned int)m_fd & 0x1f)) & 1);
}

char TCPSocket::pollWriteEvent() const
{
    fd_set writefds;
    {
        unsigned int __i;
        fd_set* __arr = &writefds;
        for (__i = 0; __i < 32; ++__i) __arr->fds_bits[__i] = 0;
    }
    FD_SET((unsigned int)m_fd, &writefds);
    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int r = 0;
    if ((r = select(2, 0, &writefds, 0, &tv)) < 0)
    {
        printf("pollWriteEvent(%s)", strerror(errno));
        return 0;
    }
    return (char)((writefds.fds_bits[(unsigned int)m_fd >> 5] >>
                   ((unsigned int)m_fd & 0x1f)) & 1);
}

char TCPSocket::pollErrorEvent() const
{
    fd_set errfds;
    {
        unsigned int __i;
        fd_set* __arr = &errfds;
        for (__i = 0; __i < 32; ++__i) __arr->fds_bits[__i] = 0;
    }
    FD_SET((unsigned int)m_fd, &errfds);
    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int r = 0;
    if ((r = select(2, 0, 0, &errfds, &tv)) < 0)
    {
        printf("pollErrorEvent(%s)", strerror(errno));
        return 0;
    }
    return (char)((errfds.fds_bits[(unsigned int)m_fd >> 5] >>
                   ((unsigned int)m_fd & 0x1f)) & 1);
}

int TCPSocket::pollReadWriteErrEvent() const
{
    fd_set readfds;
    fd_set writefds;
    fd_set errfds;
    {
        unsigned int __i;
        fd_set* __arr = &readfds;
        for (__i = 0; __i < 32; ++__i) __arr->fds_bits[__i] = 0;
    }
    {
        unsigned int __i;
        fd_set* __arr = &writefds;
        for (__i = 0; __i < 32; ++__i) __arr->fds_bits[__i] = 0;
    }
    {
        unsigned int __i;
        fd_set* __arr = &errfds;
        for (__i = 0; __i < 32; ++__i) __arr->fds_bits[__i] = 0;
    }
    FD_SET((unsigned int)m_fd, &readfds);
    FD_SET((unsigned int)m_fd, &writefds);
    FD_SET((unsigned int)m_fd, &errfds);
    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int result = 0;
    int flag = 0;
    if ((result = select(2, &readfds, &writefds, &errfds, &tv)) < 0)
    {
        printf("pollReadWriteErrEvent(%s)", strerror(errno));
        return result;  // ORIG：错误路径返回 select 结果
    }
    if ((readfds.fds_bits[(unsigned int)m_fd >> 5] >>
         ((unsigned int)m_fd & 0x1f)) & 1)
        flag = 1;
    else if ((writefds.fds_bits[(unsigned int)m_fd >> 5] >>
              ((unsigned int)m_fd & 0x1f)) & 1)
        flag = 2;
    else if ((errfds.fds_bits[(unsigned int)m_fd >> 5] >>
              ((unsigned int)m_fd & 0x1f)) & 1)
        flag = 3;
    return flag;
}

char TCPSocket::accept(TCPSocket& sock)
{
    socklen_t len = 0x10;
    sock.m_fd = ::accept(m_fd, (sockaddr*)&sock.m_addr, &len);
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
    memcpy(sock.m_peerIp, &sock.m_addr.m_ip, 4);
    sock.m_peerPort = sock.m_addr.m_port;
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
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN || errno == 0)
        {
            printf("\xbf\xa9\xb1\xe2 \xb0\xc9\xb8\xae\xb8\xe9\xbc\xad errno \xb0\xa1 0 \xc0\xcc\xb8\xe9 \xb9\xae\xc1\xa6 \xb9\xdf\xbb\xfd \xc7\xd1\xb4\xd9 !!!! \xb2\xc0 \xc8\xae\xc0\xce!!!");
            printf("tcp send retry=\'%d\', error =\'%s\'", n, strerror(errno));
            return 0;
        }
        printf("tcp send fail=\'%d\', error =\'%s\'", n, strerror(errno));
        return -1;
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
        if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK || errno == 0)
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
        memset(m_peerIp, 0, 4);
        m_peerPort = 0;
    }
    return;
}

void TCPSocket::shutdown(int how)
{
    (void)how;
    if (m_fd == -1) return;
}

char TCPSocket::setOptReuseAdrs(bool flag)
{
    unsigned int opt = 0;
    if (flag)
    {
        opt = 1;
    }
    else
    {
        opt = 0;
    }
    if (setsockopt(m_fd, 1, 2, &opt, 4) < 0)
    {
        return 0;
    }
    return 1;
}

char TCPSocket::setOptLinger(bool flag)
{
    unsigned int opt[2];
    opt[0] = flag ? 1 : 0;
    opt[1] = 0;
    if (setsockopt(m_fd, 1, 0xd, opt, 8) < 0)
    {
        return 0;
    }
    return 1;
}

char* TCPSocket::getPeerAdrs() { return (char*)m_peerIp; }

unsigned short TCPSocket::getPeerPort() { return m_peerPort; }

char* TCPSocket::getPeerIP()
{
    static char ip[0x10];
    sprintf(ip, "%d.%d.%d.%d", (unsigned int)m_peerIp[0],
            (unsigned int)m_peerIp[1],
            (unsigned int)m_peerIp[2],
            (unsigned int)m_peerIp[3]);
    return ip;
}

char TCPSocket::setOptResizeSendBuf(int size)
{
    if (size < 1)
    {
        return 0;
    }
    int optlen;
    int opt = 0;
    optlen = 4;
    int r = setsockopt(m_fd, 1, 7, &size, 4);
    if (r < 0)
    {
        return 0;
    }
    return 1;
}

char TCPSocket::setOptResizeRecvBuf(int size)
{
    if (size <= 0)
    {
        return 0;
    }
    int r = setsockopt(m_fd, 1, 8, &size, 4);
    if (r < 0)
    {
        return 0;
    }
    return 1;
}
