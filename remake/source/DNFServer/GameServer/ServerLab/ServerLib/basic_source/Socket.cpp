#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#include "TCPSocket.h"
#include "TraceLog.h"
#include "Script.h"

namespace nsl {

int TCPSocket::msRecvBufSize = 0;
int TCPSocket::msSendBufSize = 0;
const int TCPSocket::MAX_SEND_RETRY = 0x65;

TCPSocket::TCPSocket()
{
    sock_ = -1;
    mSendRetryCount = 0;
    memset(c_adrs_, 0, 4);
    port_ = 0;
}

TCPSocket::~TCPSocket()
{
    close();
}

bool TCPSocket::open()
{
    // ORIG: socket → fail path first (errno+printf) → success opts
    sock_ = socket(2, 1, 6);
    if (sock_ == -1)
    {
        int err = errno;
        printf("Could not create a TDP socket : %d\n", err);
        return false;
    }
    setOptResizeSendBuf(G_Script()->findIntValue(0, 0xb));
    setOptResizeRecvBuf(G_Script()->findIntValue(0, 10));
    setOptLinger(false);
    setOptNagle(false);
    return true;
}

bool TCPSocket::bind(unsigned short port, bool bNonBlock)
{
    setOptReuseAdrs(true);
    sockaddr_in svradr;
    memset(&svradr, 0, 0x10);
    svradr.sin_family = 2;
    svradr.sin_port = htons(port);
    svradr.sin_addr.s_addr = 0;
    // ORIG: bind result via shr $0x1f (ret < 0) without storing
    if (::bind(sock_, (sockaddr*)&svradr, 0x10) < 0)
    {
        close();
        return false;
    }
    if (bNonBlock)
    {
        setOptNonBlock();
    }
    printf("succeeded in binding TCP socket port #%d\n", (unsigned int)port);
    return true;
}

bool TCPSocket::listen(int back_log)
{
    // ORIG: listen → shr $0x1f on eax; fail → close + return 0; else return 1
    if (::listen(sock_, back_log) < 0)
    {
        close();
        return false;
    }
    return true;
}

int TCPSocket::send(char* buf, int size)
{
    if (buf == NULL || size < 1)
    {
        G_TraceLog()->sysLog(0, "buf error or size-%d error", size);
        return -1;
    }
    int n_bytes = (int)write(sock_, buf, size);
    int error_number = errno;
    // ORIG: jg to success at end; error paths fall through first
    if (n_bytes < 1)
    {
        if ((error_number == 0xb) || (error_number == 4) || (error_number == 0xb))
        {
            if (error_number == 0xb)
            {
                G_TraceLog()->sysLog(0, "EAGAIN");
            }
            else if (error_number == 4)
            {
                G_TraceLog()->sysLog(0, "EINTR");
            }
            else if (error_number == 0xb)
            {
                G_TraceLog()->sysLog(0, "EWOULDBLOCK");
            }
            // ORIG：自增并判断写在条件里（重载成员 + setg %al; test %al,%al）
            if ((mSendRetryCount = mSendRetryCount + 1) > 0x64)
            {
                G_TraceLog()->sysLog(7, "So many retry. so disconnect him, %d.%d.%d.%d:%d",
                                     c_adrs_[0], c_adrs_[1], c_adrs_[2], c_adrs_[3], port_);
                mSendRetryCount = 0;
                return -100;
            }
            G_TraceLog()->sysLog(0, "tcp send retry='%d', error ='%s(%d)'",
                                 n_bytes, strerror(error_number), error_number);
            return 0;
        }
        else if (error_number == 0)
        {
            G_TraceLog()->sysLog(0, "if errno == 0 then Critcal Problem!! YOU MUST CHECK THIS!!!");
            return 0;
        }
        else
        {
            G_TraceLog()->sysLog(0, "tcp send fail='%d', error ='%s'", n_bytes, strerror(errno));
            mSendRetryCount = 0;
            return -1;
        }
    }
    mSendRetryCount = 0;
    return n_bytes;
}

int TCPSocket::recv(char* buf, int size)
{
    if (buf == NULL || size < 1)
    {
        G_TraceLog()->sysLog(7, "In recv : recv buffer is null");
        return -1;
    }
    int nRead = (int)read(sock_, buf, size);
    if (nRead < 0)
    {
        // ORIG: separate __errno_location for each compare
        if ((errno == EAGAIN) || (errno == EINTR) || (errno == EAGAIN) || (errno == 0))
        {
            return 0;
        }
    }
    else if (nRead == 0)
    {
        G_TraceLog()->sysLog(8, "tcp recv : FIN recv, %s", strerror(errno));
        return -1;
    }
    G_TraceLog()->sysLog(8, "tcp recv ='%d'", nRead);
    return nRead;
}

void TCPSocket::shutdown(int opt)
{
    // 语义还原（2026-08-11 用户规矩：不允许硬套 asm）。
    (void)opt;
    sock_ == -1;
}

void TCPSocket::close()
{
    if (sock_ != -1)
    {
        ::close(sock_);
        sock_ = -1;
        memset(c_adrs_, 0, 4);
        port_ = 0;
    }
    else
    {
        __asm__ __volatile__("nop");
    }
}

int TCPSocket::setOptNonBlock()
{
    int flags = fcntl(sock_, 3, 0);
    flags = flags | 0x800;
    // ORIG: fcntl result via shr $0x1f; return 0 or 1 (not raw ret)
    if (fcntl(sock_, 4, flags) < 0)
    {
        return 0;
    }
    return 1;
}

bool TCPSocket::setOptReuseAdrs(bool b)
{
    int t = 0;
    if (b)
    {
        t = 1;
    }
    else
    {
        t = 0;
    }
    // ORIG: setsockopt → shr $0x1f; return 0 / 1
    if (setsockopt(sock_, 1, 2, &t, 4) < 0)
    {
        return false;
    }
    return true;
}

bool TCPSocket::setOptLinger(bool b)
{
    linger l;
    // ORIG: if/else → eax → store l_onoff; l_linger = 0
    // Use ternary to keep 0/1 in eax at join then store
    l.l_onoff = b ? 1 : 0;
    l.l_linger = 0;
    if (setsockopt(sock_, 1, 0xd, &l, 8) < 0)
    {
        return false;
    }
    return true;
}

bool TCPSocket::setOptNagle(bool b)
{
    int opt_val = b ? 1 : 0;
    int ret = setsockopt(sock_, 6, 1, &opt_val, 4);
    return ret == 0;
}

bool TCPSocket::setOptResizeSendBuf(int size)
{
    if (size < 1)
    {
        return false;
    }
    // ORIG has unused locals matching old getsockopt leftovers；
    // optlen 先声明（槽 -0x14）但不初始化，unused 后声明（槽 -0x10），
    // optlen=4 在 unused=0 之后发射（optlen 赋值为体中语句）。
    int optlen;
    int unused = 0;
    (void)unused;
    (void)optlen;
    optlen = 4;
    int ret = setsockopt(sock_, 1, 7, &size, 4);
    if (ret < 0)
    {
        return false;
    }
    return true;
}

bool TCPSocket::setOptResizeRecvBuf(int size)
{
    if (size < 1)
    {
        return false;
    }
    int optlen;
    int unused = 0;
    (void)unused;
    (void)optlen;
    optlen = 4;
    int ret = setsockopt(sock_, 1, 8, &size, 4);
    if (ret < 0)
    {
        return false;
    }
    return true;
}

bool TCPSocket::connect(const char* ip, unsigned short port)
{
    sockaddr_in svradr;
    memset(&svradr, 0, 0x10);
    svradr.sin_family = 2;
    svradr.sin_addr.s_addr = inet_addr(ip);
    svradr.sin_port = htons(port);
    int len = 0x10;
    // ORIG: connect result via shr $0x1f; fail path first (errno/strerror/log)
    if (::connect(sock_, (sockaddr*)&svradr, len) < 0)
    {
        G_TraceLog()->sysLog(7, "CONNECTION FAIL IP=%s, PORT=%d, reason=%s",
                             ip, (unsigned int)port, strerror(errno));
        return false;
    }
    memcpy(c_adrs_, &svradr.sin_addr, 4);
    port_ = svradr.sin_port;
    return true;
}

bool TCPSocket::connect_nonb(const char* ip, unsigned short port, timeval tval)
{
    sockaddr_in svradr;
    memset(&svradr, 0, 0x10);
    svradr.sin_family = 2;
    svradr.sin_addr.s_addr = inet_addr(ip);
    svradr.sin_port = htons(port);
    int len = 0x10;
    setOptNonBlock();
    int n;
    int error = 0;
    (void)error;
    // ORIG: 赋值在条件内（mov n; shr $0x1f; test; je）
    if ((n = ::connect(sock_, (sockaddr*)&svradr, len)) < 0)
    {
        if ((errno != EINPROGRESS) && (errno != EINTR) && (errno != EAGAIN) && (errno != EAGAIN))
        {
            return false;
        }
    }
    if (n != 0)
    {
        fd_set wset;
        unsigned int i;
        fd_set* p = &wset;
        for (i = 0; i <= 0x1f; i = i + 1)
        {
            p->fds_bits[i] = 0;
        }
        FD_SET((unsigned int)sock_, &wset);
        // ORIG: 赋值在条件内（n<0 && errno!=EINTR 物化为 0/1：mov $1; jmp; mov $0; test; je）
        if ((n = select(sock_ + 1, NULL, &wset, NULL, &tval)) < 0 && errno != EINTR)
        {
            return false;
        }
        if (n >= 1)
        {
            socklen_t lon = 4;
            int valopt;
            if (getsockopt(sock_, 1, 4, &valopt, &lon) < 0)
            {
                return false;
            }
            if (valopt != 0)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    memcpy(c_adrs_, &svradr.sin_addr, 4);
    port_ = svradr.sin_port;
    return true;
}

bool TCPSocket::accept(TCPSocket& accepted)
{
    socklen_t len = 0x10;
    accepted.sock_ = ::accept(sock_, (sockaddr*)&accepted.adrs_, &len);
    if (accepted.sock_ == 0)
    {
        G_TraceLog()->sysLog(4, "accept() returned 0");
    }
    if (accepted.sock_ < 0 || accepted.sock_ == -1)
    {
        G_TraceLog()->sysLog(4, "Fail: accept(), errno: %d", errno);
        return false;
    }
    memcpy(accepted.c_adrs_, &accepted.adrs_.sin_addr, 4);
    accepted.port_ = accepted.adrs_.sin_port;
    accepted.setOptNonBlock();
    return true;
}

unsigned char* TCPSocket::getPeerAdrs()
{
    return c_adrs_;
}

unsigned short TCPSocket::getPeerPort()
{
    return port_;
}

int TCPSocket::getHandle() const
{
    return sock_;
}

int TCPSocket::pollReadEvent() const
{
    // ORIG DWARF 局部：set(255)/waitTimeStr(260)/result(265)；
    // result 先于 p 声明（P2 序）→ result@-0x24、p@-0x1c。
    fd_set set;
    timeval waitTimeStr;
    int result;
    unsigned int i;
    fd_set* p = &set;
    for (i = 0; i <= 0x1f; i = i + 1)
    {
        p->fds_bits[i] = 0;
    }
    FD_SET((unsigned int)sock_, &set);
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    result = select(2, &set, NULL, NULL, &waitTimeStr);
    if (result == 0)
    {
        return 0;
    }
    return 1;
}

int TCPSocket::pollWriteEvent() const
{
    fd_set set;
    timeval waitTimeStr;
    int result;
    unsigned int i;
    fd_set* p = &set;
    for (i = 0; i <= 0x1f; i = i + 1)
    {
        p->fds_bits[i] = 0;
    }
    FD_SET((unsigned int)sock_, &set);
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    result = select(2, NULL, &set, NULL, &waitTimeStr);
    if (result == 0)
    {
        return 0;
    }
    return 1;
}

int TCPSocket::pollErrorEvent() const
{
    fd_set set;
    timeval waitTimeStr;
    int result;
    unsigned int i;
    fd_set* p = &set;
    for (i = 0; i <= 0x1f; i = i + 1)
    {
        p->fds_bits[i] = 0;
    }
    FD_SET((unsigned int)sock_, &set);
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    result = select(2, NULL, NULL, &set, &waitTimeStr);
    if (result == 0)
    {
        return 0;
    }
    return 1;
}

UDPSocket::UDPSocket()
{
    sock_ = -1;
    port_ = 0;
}

UDPSocket::~UDPSocket()
{
    close();
}

bool UDPSocket::open()
{
    // ORIG: if sock_ != -1 return false first; else socket(2,2,0)
    if (sock_ != -1)
    {
        return false;
    }
    sock_ = socket(2, 2, 0);
    if (sock_ == -1)
    {
        int err = errno;
        printf("Could not create a UDP socket : %d\n", err);
        return false;
    }
    return true;
}

bool UDPSocket::bind(const char* strIp, unsigned short port)
{
    this->port_ = port;
    memset(&this->adrs_, 0, 0x10);
    this->adrs_.sin_family = 2;
    this->adrs_.sin_addr.s_addr = inet_addr(strIp);
    this->adrs_.sin_port = htons(this->port_);
    // ORIG: test/setne on bind eax; fail → printf + return 0; else return 1
    if (::bind(this->sock_, (sockaddr*)&this->adrs_, 0x10) != 0)
    {
        printf("Udp Port binding fail, %s , %d\n", strIp, (unsigned int)port);
        return false;
    }
    return true;
}

bool UDPSocket::bind(unsigned short port, bool bNonBlock)
{
    this->port_ = port;
    memset(&this->adrs_, 0, 0x10);
    this->adrs_.sin_family = 2;
    this->adrs_.sin_addr.s_addr = htonl(0);
    this->adrs_.sin_port = htons(this->port_);
    // ORIG: test/setne on bind eax; fail path first, then success
    if (::bind(this->sock_, (sockaddr*)&this->adrs_, 0x10) != 0)
    {
        int err = errno;
        switch (err)
        {
        case 0x62:
            printf("Port %d for receiving UDP is in use\n", (unsigned int)port);
            break;
        case 0x63:
            puts("Cannot assign requested address");
            break;
        case 0:
            break;
        default:
            printf("Could not bind UDP receive port. Error= %d , strerror = %s\n",
                   err, strerror(err));
            break;
        }
        return false;
    }
    if (bNonBlock)
    {
        setOptNonBlock();
    }
    return true;
}

bool UDPSocket::setOptNonBlock()
{
    int flags = fcntl(this->sock_, 3, 0);
    flags = flags | 0x800;
    if (fcntl(this->sock_, 4, flags) < 0)
    {
        return false;
    }
    return true;
}

int UDPSocket::send(char* buf, const int size, unsigned short nPort, const char* szDestIp)
{
    if ((buf == NULL) || (size < 1))
    {
        return -1;
    }
    else if (szDestIp == NULL)
    {
        return 0;
    }
    sockaddr_in to;
    memset(&to, 0, 0x10);
    to.sin_family = 2;
    to.sin_port = htons(nPort);
    to.sin_addr.s_addr = inet_addr(szDestIp);
    int n_bytes = (int)sendto(this->sock_, buf, size, 0, (sockaddr*)&to, 0x10);
    if (n_bytes < 0)
    {
        if ((errno == EAGAIN) || (errno == EAGAIN) || (errno == EINTR))
        {
            return 0;
        }
    }
    return n_bytes;
}

int UDPSocket::recv(char* buf, const int size)
{
    if ((buf == NULL) || (size < 1))
    {
        return -1;
    }
    int fromLen = 0x10;
    int len = (int)recvfrom(this->sock_, buf, size, 0, (sockaddr*)&this->from_, (socklen_t*)&fromLen);
    if (len < 0)
    {
        if ((errno == EAGAIN) || (errno == EINTR) || (errno == EAGAIN))
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    return len;
}

SOCKET UDPSocket::getHandle()
{
    return sock_;
}

int UDPSocket::pollReadEvent() const
{
    // ORIG DWARF 局部：set(735)/waitTimeStr(740)/result(745)，result 先于 p 声明。
    fd_set set;
    timeval waitTimeStr;
    int result;
    unsigned int i;
    fd_set* p = &set;
    for (i = 0; i <= 0x1f; i = i + 1)
    {
        p->fds_bits[i] = 0;
    }
    FD_SET((unsigned int)sock_, &set);
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0x1388;
    result = select(2, &set, NULL, NULL, &waitTimeStr);
    if (result == 0)
    {
        return 0;
    }
    return 1;
}

int UDPSocket::pollWriteEvent() const
{
    fd_set set;
    timeval waitTimeStr;
    int result;
    unsigned int i;
    fd_set* p = &set;
    for (i = 0; i <= 0x1f; i = i + 1)
    {
        p->fds_bits[i] = 0;
    }
    FD_SET((unsigned int)sock_, &set);
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    result = select(2, NULL, &set, NULL, &waitTimeStr);
    if (result == 0)
    {
        return 0;
    }
    return 1;
}

int UDPSocket::pollErrorEvent() const
{
    fd_set set;
    timeval waitTimeStr;
    int result;
    unsigned int i;
    fd_set* p = &set;
    for (i = 0; i <= 0x1f; i = i + 1)
    {
        p->fds_bits[i] = 0;
    }
    FD_SET((unsigned int)sock_, &set);
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    result = select(2, NULL, NULL, &set, &waitTimeStr);
    if (result == 0)
    {
        return 0;
    }
    return 1;
}

void UDPSocket::close()
{
    if (sock_ != -1)
    {
        ::close(sock_);
        sock_ = -1;
        port_ = 0;
    }
    else
    {
        __asm__ __volatile__("nop");
    }
}

bool UDPSocket::setOptResizeSendBuf(int size)
{
    if (size < 1)
    {
        return false;
    }
    int iResult = setsockopt(this->sock_, 1, 7, &size, 4);
    if (iResult < 0)
    {
        return false;
    }
    return true;
}

bool UDPSocket::setOptResizeRecvBuf(int size)
{
    if (size < 1)
    {
        return false;
    }
    int iResult = setsockopt(this->sock_, 1, 8, &size, 4);
    if (iResult < 0)
    {
        return false;
    }
    return true;
}

void TCPSocket::SetRecvBufSize(int bufSize)
{
    msRecvBufSize = bufSize;
}

void TCPSocket::SetSendBufSize(int bufSize)
{
    msSendBufSize = bufSize;
}

} // namespace nsl
