// df_manager_r — TCPSocket（ORIG DNFTcpSocket.cpp）
#include "DNFTcpSocket.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

TCPSocket::TCPSocket()
{
    m_fd = -1;
    memset(&m_addr, 0, 4);
    memset(&m_data4, 0, 0x10);
    m_port = 0;
}

TCPSocket::~TCPSocket()
{
    close();
}

char TCPSocket::open()
{
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd == -1)
    {
        printf("socket error %d", errno);
        return 0;
    }
    return 1;
}

void TCPSocket::close()
{
    if (m_fd != -1)
    {
        ::close(m_fd);
        m_fd = -1;
        memset(&m_addr, 0, 4);
        m_port = 0;
    }
}

int TCPSocket::shutdown(int how)
{
    // ORIG 反汇编：不调用 ::shutdown(2)，仅装载 m_fd 并与 -1 比较（返回值仍为 m_fd）。
    (void)how;
    m_fd == -1;
    return m_fd;
}

int TCPSocket::send(char* buf, int len)
{
    if (!buf || len <= 0)
    {
        printf("buf error or size-%d error", len);
        return -1;
    }
    int n = write(m_fd, buf, len);
    if (n <= 0)
    {
        int e = errno;
        if (e == 0xb || e == 0x4 || e == 0xb)
        {
            printf("tcp send fail='%d', error ='%s'", n, strerror(e));
            return -1;
        }
        if (e != 0)
        {
            printf("tcp send retry='%d', error ='%s'", n, strerror(e));
            return 0;
        }
        printf("send error no 0");
        printf("tcp send retry='%d', error ='%s'", n, strerror(e));
        return 0;
    }
    printf("tcp send='%d', error ='%s'", n, strerror(errno));
    return n;
}

int TCPSocket::recv(char* buf, int len)
{
    if (!buf || len <= 0)
    {
        printf("In recv : recv buffer is null");
        return -1;
    }
    int n = read(m_fd, buf, len);
    if (n < 0)
    {
        int e = errno;
        if (e == 0xb || e == 0x4 || e == 0xb)
        {
            if (n != 0)
                return n;
            printf("tcp recv : FIN recv, %s", strerror(e));
            return -1;
        }
        if (e != 0)
            return n;
        return 0;
    }
    printf("tcp recv ='%d'", n);
    return n;
}

char TCPSocket::setOptResizeRecvBuf(int size)
{
    if (size <= 0)
        return 0;
    int ret = setsockopt(m_fd, SOL_SOCKET, SO_RCVBUF, &size, 4);
    if (ret < 0)
        return 0;
    return 1;
}

char TCPSocket::connect(const char* ip, unsigned short port)
{
    struct sockaddr_in addr;
    memset(&addr, 0, 0x10);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    int len = 0x10;
    if (::connect(m_fd, (struct sockaddr*)&addr, len) < 0)
    {
        printf("CONNECTION FAIL IP =%s, PORT =%d, reason =%s",
               ip, port, strerror(errno));
        return 0;
    }
    memcpy((char*)this + 0x14, (char*)&addr + 4, 4);
    m_port = *(unsigned short*)((char*)&addr + 2);
    return 1;
}

char TCPSocket::setOptNonBlock()
{
    int flags = fcntl(m_fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    if (fcntl(m_fd, F_SETFL, flags) < 0)
        return 0;
    return 1;
}

char TCPSocket::setOptReuseAdrs(bool flag)
{
    int opt = flag ? 1 : 0;
    if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, 4) < 0)
        return 0;
    return 1;
}

char TCPSocket::setOptLinger(bool flag)
{
    struct linger linger;
    linger.l_onoff = flag ? 1 : 0;
    linger.l_linger = 0;
    if (setsockopt(m_fd, SOL_SOCKET, SO_LINGER, &linger, 8) < 0)
        return 0;
    return 1;
}

char TCPSocket::setOptResizeSendBuf(int size)
{
    if (size <= 0)
        return 0;
    if (setsockopt(m_fd, SOL_SOCKET, SO_SNDBUF, &size, 4) < 0)
        return 0;
    return 1;
}

char* TCPSocket::getPeerIP()
{
    static char ip[0x20];
    sprintf(ip, "%d.%d.%d.%d",
            (unsigned char)m_addr[0],
            (unsigned char)m_addr[1],
            (unsigned char)m_addr[2],
            (unsigned char)m_addr[3]);
    return ip;
}

char TCPSocket::pollWriteEvent() const
{
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(m_fd, &writefds);
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int ret = select(2, 0, &writefds, 0, &tv);
    if (ret < 0)
    {
        printf("pollWriteEvent(%s)", strerror(errno));
        return 0;
    }
    return FD_ISSET(m_fd, &writefds) ? 1 : 0;
}

char TCPSocket::pollErrorEvent() const
{
    fd_set exceptfds;
    FD_ZERO(&exceptfds);
    FD_SET(m_fd, &exceptfds);
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int ret = select(2, 0, 0, &exceptfds, &tv);
    if (ret < 0)
    {
        printf("pollErrorEvent(%s)", strerror(errno));
        return 0;
    }
    return FD_ISSET(m_fd, &exceptfds) ? 1 : 0;
}

int TCPSocket::pollReadWriteErrEvent() const
{
    fd_set readfds, writefds, exceptfds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    FD_SET(m_fd, &readfds);
    FD_SET(m_fd, &writefds);
    FD_SET(m_fd, &exceptfds);
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int ret = select(2, &readfds, &writefds, &exceptfds, &tv);
    if (ret < 0)
    {
        printf("pollReadWriteErrEvent(%s)", strerror(errno));
        return ret;
    }
    int result = 0;
    if (FD_ISSET(m_fd, &readfds))
        result = 1;
    else if (FD_ISSET(m_fd, &writefds))
        result = 2;
    else if (FD_ISSET(m_fd, &exceptfds))
        result = 3;
    return result;
}

char TCPSocket::bind(unsigned short port, bool flag)
{
    setOptReuseAdrs(true);
    struct sockaddr_in addr;
    memset(&addr, 0, 0x10);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = 0;
    if (::bind(m_fd, (struct sockaddr*)&addr, 0x10) < 0)
    {
        close();
        return 0;
    }
    if (flag)
        setOptNonBlock();
    printf("succeeded in binding TCP socket port #%d\n", port);
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
    FD_ZERO(&readfds);
    FD_SET(m_fd, &readfds);
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    int ret = select(m_fd + 1, &readfds, 0, 0, &tv);
    if (ret < 0)
    {
        printf("pollReadEvent(%s)", strerror(errno));
        return 0;
    }
    return FD_ISSET(m_fd, &readfds) ? 1 : 0;
}

char TCPSocket::accept(TCPSocket& sock)
{
    socklen_t len = 0x10;
    sock.m_fd = ::accept(m_fd, (struct sockaddr*)((char*)&sock + 4), &len);
    if (sock.m_fd == 0)
    {
        FILE* f = fopen("log.txt", "a+");
        if (f)
        {
            fprintf(f, "[TCPSocket::Accept] Accept fail[%d]\n", sock.m_fd);
            fclose(f);
        }
    }
    if (sock.m_fd < 0)
    {
        FILE* f = fopen("log.txt", "a+");
        if (f)
        {
            fprintf(f, "[TCPSocket::Accept] Accept fail[%d]\n", sock.m_fd);
            fclose(f);
        }
    }
    if (sock.m_fd == -1)
        return 0;
    memcpy((char*)&sock + 0x14, (char*)&sock + 8, 4);
    sock.m_port = *(unsigned short*)((char*)&sock + 6);
    sock.setOptNonBlock();
    return 1;
}

int TCPSocket::getHandle() const { return m_fd; }
char* TCPSocket::getPeerAdrs() { return (char*)&m_addr; }
unsigned short TCPSocket::getPeerPort() { return m_port; }
