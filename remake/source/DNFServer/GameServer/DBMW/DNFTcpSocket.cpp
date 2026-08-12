// df_dbmw_r - DNFTcpSocket (ORIG DNFTcpSocket.cpp)
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

// ORIG 用 glibc 2.5 头编译：FD_ZERO 展开为循环（0x20 次 dword 清零），
// __NFDBITS 为无符号（shr/and 形态）；c6root glibc 2.12 的 FD_ZERO 为
// inline asm（rep stos）且带 (int) 强转（sar 形态），按 2.5 形态复现
// （同 manager 先例）。
#define DBMW_NFDBITS (8 * sizeof(__fd_mask))
#define DBMW_FD_ZERO(fdsetp) \
    do { \
        unsigned int __dbmw_i; \
        fd_set* __dbmw_arr = (fdsetp); \
        for (__dbmw_i = 0; __dbmw_i < sizeof(fd_set) / sizeof(__fd_mask); ++__dbmw_i) \
            __FDS_BITS(__dbmw_arr)[__dbmw_i] = 0; \
    } while (0)
#define DBMW_FD_SET(d, fdsetp) \
    (__FDS_BITS((fdsetp))[(d) / DBMW_NFDBITS] |= (__fd_mask)1 << ((d) % DBMW_NFDBITS))
#define DBMW_FD_ISSET(d, fdsetp) \
    ((__FDS_BITS((fdsetp))[(d) / DBMW_NFDBITS] & (__fd_mask)1 << ((d) % DBMW_NFDBITS)) != 0)

int getErrno();

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
bool TCPSocket::open()
{
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd == -1)
    {
        int e = errno;
        printf("Could not create a TDP socket : %d\n", e);
        return 0;
    }
    return 1;
}
void TCPSocket::close()
{
    if (m_fd == -1)
        return;
    ::close(m_fd);
    m_fd = -1;
    memset(&m_addr, 0, 4);
    m_port = 0;
}
int TCPSocket::shutdown(int how)
{
    // ORIG = mov;mov;cmp;pop;ret：switch 双空 case 保留死比较（同 manager 先例）
    (void)how;
    switch (m_fd == -1)
    {
    case 0:
        break;
    case 1:
        break;
    }
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
        if (errno == 0xb || errno == 0x4 || errno == 0xb || errno == 0)
        {
            printf("\xbf\xa9\xb1\xe2 \xb0\xc9\xb8\xae\xb8\xe9\xbc\xad errno \xb0\xa1 0 \xc0\xcc\xb8\xe9 \xb9\xae\xc1\xa6 \xb9\xdf\xbb\xfd \xc7\xd1\xb4\xd9 !!!! \xb2\xc0 \xc8\xae\xc0\xce!!!");
            printf("tcp send retry='%d', error ='%s'", n, strerror(errno));
            return 0;
        }
        printf("tcp send fail='%d', error ='%s'", n, strerror(errno));
        return -1;
    }
    printf("1.tcp send='%d', error ='%s'", n, strerror(errno));
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
        if (errno == 0xb || errno == 0x4 || errno == 0xb || errno == 0)
            return 0;
    }
    else if (n == 0)
    {
        printf("tcp recv : FIN recv, %s", strerror(errno));
        return -1;
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
bool TCPSocket::connect(const char* ip, unsigned short port)
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
    memcpy(&m_addr, &addr.sin_addr.s_addr, 4);
    m_port = addr.sin_port;
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
    int opt = 0;
    if (flag)
        opt = 1;
    else
        opt = 0;
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
    int len;
    int opt;
    opt = 0;
    len = 4;
    int ret = setsockopt(m_fd, SOL_SOCKET, SO_SNDBUF, &size, 4);
    if (ret < 0)
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
    fd_set fds;
    struct timeval tv;
    int ret;
    unsigned int i;
    fd_set* p = &fds;
    for (i = 0; i < 0x20; i++)
    {
        p->fds_bits[i] = 0;
    }
    fds.fds_bits[(unsigned int)m_fd >> 5] =
        fds.fds_bits[(unsigned int)m_fd >> 5] | (1 << ((unsigned int)m_fd & 0x1f));
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    ret = 0;
    if ((unsigned)(ret = select(2, 0, &fds, 0, &tv)) >> 31)
    {
        printf("pollWriteEvent(%s)", strerror(errno));
        return 0;
    }
    return (fds.fds_bits[(unsigned int)m_fd >> 5] >> ((unsigned int)m_fd & 0x1f)) & 1;
}
char TCPSocket::pollErrorEvent() const
{
    fd_set fds;
    struct timeval tv;
    int ret;
    unsigned int i;
    fd_set* p = &fds;
    for (i = 0; i < 0x20; i++)
    {
        p->fds_bits[i] = 0;
    }
    fds.fds_bits[(unsigned int)m_fd >> 5] =
        fds.fds_bits[(unsigned int)m_fd >> 5] | (1 << ((unsigned int)m_fd & 0x1f));
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    ret = 0;
    if ((unsigned)(ret = select(2, 0, 0, &fds, &tv)) >> 31)
    {
        printf("pollErrorEvent(%s)", strerror(errno));
        return 0;
    }
    return (fds.fds_bits[(unsigned int)m_fd >> 5] >> ((unsigned int)m_fd & 0x1f)) & 1;
}
int TCPSocket::pollReadWriteErrEvent() const
{
    fd_set readfds, writefds, exceptfds;
    struct timeval tv;
    int ret;
    int result;
    DBMW_FD_ZERO(&readfds);
    DBMW_FD_ZERO(&writefds);
    DBMW_FD_ZERO(&exceptfds);
    readfds.fds_bits[(unsigned int)m_fd >> 5] =
        readfds.fds_bits[(unsigned int)m_fd >> 5] | (1 << ((unsigned int)m_fd & 0x1f));
    writefds.fds_bits[(unsigned int)m_fd >> 5] =
        writefds.fds_bits[(unsigned int)m_fd >> 5] | (1 << ((unsigned int)m_fd & 0x1f));
    exceptfds.fds_bits[(unsigned int)m_fd >> 5] =
        exceptfds.fds_bits[(unsigned int)m_fd >> 5] | (1 << ((unsigned int)m_fd & 0x1f));
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    ret = 0;
    result = 0;
    if ((unsigned)(ret = select(2, &readfds, &writefds, &exceptfds, &tv)) >> 31)
    {
        printf("pollReadWriteErrEvent(%s)", strerror(errno));
        return ret;
    }
    if ((readfds.fds_bits[(unsigned int)m_fd >> 5] >> ((unsigned int)m_fd & 0x1f)) & 1)
        result = 1;
    else if ((writefds.fds_bits[(unsigned int)m_fd >> 5] >> ((unsigned int)m_fd & 0x1f)) & 1)
        result = 2;
    else if ((exceptfds.fds_bits[(unsigned int)m_fd >> 5] >> ((unsigned int)m_fd & 0x1f)) & 1)
        result = 3;
    return result;
}
bool TCPSocket::bind(unsigned short port, bool flag)
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
bool TCPSocket::listen(int backlog)
{
    if (::listen(m_fd, backlog) < 0)
    {
        close();
        return 0;
    }
    return 1;
}
bool TCPSocket::pollReadEvent() const
{
    fd_set fds;
    struct timeval tv;
    int n;
    unsigned int i;
    fd_set* p = &fds;
    for (i = 0; i < 0x20; i++)
    {
        p->fds_bits[i] = 0;
    }
    fds.fds_bits[(unsigned int)m_fd >> 5] =
        fds.fds_bits[(unsigned int)m_fd >> 5] | (1 << ((unsigned int)m_fd & 0x1f));
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    n = 0;
    if ((unsigned)(n = select(m_fd + 1, &fds, 0, 0, &tv)) >> 31)
    {
        printf("pollReadEvent(%s)", strerror(errno));
        return 0;
    }
    return (fds.fds_bits[(unsigned int)m_fd >> 5] >> ((unsigned int)m_fd & 0x1f)) & 1;
}
bool TCPSocket::accept(TCPSocket& sock)
{
    socklen_t len = 0x10;
    sock.m_fd = ::accept(m_fd, (struct sockaddr*)&sock.m_data4, &len);
    if (sock.m_fd == 0)
    {
        FILE* f = fopen("log.txt", "a+");
        if (f)
        {
            fprintf(f, "[TCPSocket::Accept] Accept fail[%d]\n", sock.m_fd);
            fclose(f);
        }
    }
    if (sock.m_fd < 0 || sock.m_fd == -1)
    {
        FILE* f = fopen("log.txt", "a+");
        if (f)
        {
            fprintf(f, "[TCPSocket::Accept] Accept fail[%d]\n", sock.m_fd);
            fclose(f);
        }
        return 0;
    }
    memcpy((char*)&sock + 0x14, (char*)&sock + 8, 4);
    sock.m_port = sock.m_data4.sin_port;
    sock.setOptNonBlock();
    return 1;
}
int TCPSocket::getHandle() const { return m_fd; }
char* TCPSocket::getPeerAdrs() { return (char*)&m_addr; }
unsigned short TCPSocket::getPeerPort() { return m_port; }
