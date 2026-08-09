#include "Socket.h"
#include "ChannelServiceApp.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

TCPSocket::TCPSocket()
{
    sock_ = -1;
    memset(c_adrs_, 0, 4);
    port_ = 0;
}

TCPSocket::~TCPSocket()
{
    close();
}

bool TCPSocket::open()
{
    sock_ = socket(2, 1, 6);
    if (sock_ == -1)
    {
        int err = *__errno_location();
        printf("Could not create a TDP socket : %d\n", err);
        return false;
    }
    setOptLinger(false);
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
    if (::bind(sock_, (sockaddr*)&svradr, 0x10) < 0)
    {
        puts("[TCPSocket::bind] Bind error");
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
    if (::listen(sock_, back_log) < 0)
    {
        puts("[TCPSocket::listen] Listen error");
        close();
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
    if (::connect(sock_, (sockaddr*)&svradr, len) < 0)
    {
        return false;
    }
    memcpy(c_adrs_, (char*)&svradr.sin_addr, 4);
    port_ = svradr.sin_port;
    return true;
}

int TCPSocket::send(char* buf, int size)
{
    if (this == NULL)
    {
        return -2;
    }
    if ((buf == NULL) || (size < 1))
    {
        GLOG(ChannelServiceApp::gFileLogInfo, "!buf or size<1 :");
        return -1;
    }
    int n_bytes = write(sock_, buf, size);
    if (n_bytes < 1)
    {
        if ((((*__errno_location() == 0xb) || (*__errno_location() == 4)) ||
             (*__errno_location() == 0xb)) || (*__errno_location() == 0))
        {
            GLOG(ChannelServiceApp::gFileLogInfo, "tcp send fail= " << n_bytes << " error = " << strerror(*__errno_location()));
            return 0;
        }
        GLOG(ChannelServiceApp::gFileLogInfo, "tcp send fail= " << n_bytes << " error = " << strerror(*__errno_location()));
        return -1;
    }
    GLOG(ChannelServiceApp::gFileLogInfo, "tcp send = " << n_bytes << " error = " << strerror(*__errno_location()));
    return n_bytes;
}

int TCPSocket::recv(char* buf, int size)
{
    if ((buf == NULL) || (size < 1))
    {
        GLOG(ChannelServiceApp::gFileLogInfo, "tcp recv error : !buf or size=" << size);
        return -1;
    }
    int n_read = read(sock_, buf, size);
    if (n_read < 0)
    {
        if ((((*__errno_location() == 0xb) || (*__errno_location() == 4)) ||
             (*__errno_location() == 0xb)) || (*__errno_location() == 0))
        {
            GLOG(ChannelServiceApp::gFileLogInfo, "tcp recv error : " << strerror(*__errno_location()));
            return 0;
        }
    }
    else if (n_read == 0)
    {
        GLOG(ChannelServiceApp::gFileLogInfo, "tcp recv : FIN recv");
        return -1;
    }
    return n_read;
}

void TCPSocket::close()
{
    if (sock_ == -1)
    {
        return;
    }
    ::close(sock_);
    sock_ = -1;
    memset(c_adrs_, 0, 4);
    port_ = 0;
}

int TCPSocket::shutdown(int opt)
{
    // Verified from original: does NOT call ::shutdown(2); only loads sock_
    // and compares against -1 (return value remains sock_ in eax).
    (void)opt;
    sock_ == -1;
    return sock_;
}

bool TCPSocket::accept(TCPSocket& accepted)
{
    socklen_t len = 0x10;
    accepted.sock_ = ::accept(sock_, (sockaddr*)&accepted.adrs_, &len);
    if (accepted.sock_ == 0)
    {
        FILE* log = fopen("log.txt", "a+");
        if (log != NULL)
        {
            fprintf(log, "[TCPSocket::Accept] Accept fail[%d]\n", accepted.sock_);
            fclose(log);
        }
    }
    if ((accepted.sock_ < 0) || (accepted.sock_ == -1))
    {
        FILE* log = fopen("log.txt", "a+");
        if (log != NULL)
        {
            fprintf(log, "[TCPSocket::Accept] Accept fail[%d]\n", accepted.sock_);
            fclose(log);
        }
        return false;
    }
    memcpy(accepted.c_adrs_, (char*)&accepted.adrs_.sin_addr, 4);
    accepted.port_ = accepted.adrs_.sin_port;
    accepted.setOptNonBlock();
    accepted.setOptResizeSendBuf(0x19000);
    accepted.setOptResizeRecvBuf(0x19000);
    return true;
}

bool TCPSocket::setOptLinger(bool b)
{
    linger l;
    l.l_onoff = b ? 1 : 0;
    l.l_linger = 0;
    if (setsockopt(sock_, 1, 0xd, &l, 8) < 0)
    {
        return false;
    }
    return true;
}

bool TCPSocket::setOptNonBlock()
{
    int flags = fcntl(sock_, 3, 0);
    flags = flags | 0x800;
    if (-1 >= fcntl(sock_, 4, flags))
    {
        puts("[TCPSocket::nonblock] Can't make nonblock socket");
        return false;
    }
    return true;
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
    if (setsockopt(sock_, 1, 2, &t, 4) < 0)
    {
        return false;
    }
    return true;
}

bool TCPSocket::setOptResizeSendBuf(int size)
{
    if (size < 1)
    {
        return false;
    }
    int sndbuf = 0;
    socklen_t optlen = 4;
    int nRet = setsockopt(sock_, 1, 7, &size, 4);
    if (nRet < 0)
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
    int sndbuf = 0;
    socklen_t optlen = 4;
    getsockopt(sock_, 1, 8, &sndbuf, &optlen);
    int nRet = setsockopt(sock_, 1, 8, &size, 4);
    if (nRet < 0)
    {
        return false;
    }
    return true;
}

SOCKET TCPSocket::getHandle() const
{
    return sock_;
}

unsigned char* TCPSocket::getPeerAdrs()
{
    return c_adrs_;
}

unsigned short TCPSocket::getPeerPort()
{
    return port_;
}

bool TCPSocket::pollReadEvent() const
{
    fd_set set;
    int result;
    unsigned int __i;
    fd_set* __arr = &set;
    for (__i = 0; __i < sizeof(fd_set) / sizeof(__fd_mask); ++__i)
    {
        __arr->fds_bits[__i] = 0;
    }
    FD_SET((unsigned int)sock_, &set);
    timeval waitTimeStr;
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    result = select(2, &set, NULL, NULL, &waitTimeStr);
    if (result == 0)
    {
        return false;
    }
    return true;
}

bool TCPSocket::pollWriteEvent() const
{
    fd_set set;
    int result;
    unsigned int __i;
    fd_set* __arr = &set;
    for (__i = 0; __i < sizeof(fd_set) / sizeof(__fd_mask); ++__i)
    {
        __arr->fds_bits[__i] = 0;
    }
    FD_SET((unsigned int)sock_, &set);
    timeval waitTimeStr;
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    result = select(2, NULL, &set, NULL, &waitTimeStr);
    if (result == 0)
    {
        return false;
    }
    return true;
}

bool TCPSocket::pollErrorEvent() const
{
    fd_set set;
    int result;
    unsigned int __i;
    fd_set* __arr = &set;
    for (__i = 0; __i < sizeof(fd_set) / sizeof(__fd_mask); ++__i)
    {
        __arr->fds_bits[__i] = 0;
    }
    FD_SET((unsigned int)sock_, &set);
    timeval waitTimeStr;
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    result = select(2, NULL, NULL, &set, &waitTimeStr);
    if (result == 0)
    {
        return false;
    }
    return true;
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
    if (sock_ != -1)
    {
        return false;
    }
    sock_ = socket(2, 2, 0);
    if (sock_ == -1)
    {
        int err = *__errno_location();
        printf("Could not create a UDP socket : %d\n", err);
        return false;
    }
    return true;
}

bool UDPSocket::bind(unsigned short port, bool bNonBlock)
{
    port_ = port;
    memset(&adrs_, 0, 0x10);
    adrs_.sin_family = 2;
    adrs_.sin_addr.s_addr = htonl(0);
    adrs_.sin_port = htons(port_);
    if (::bind(sock_, (sockaddr*)&adrs_, 0x10) != 0)
    {
        int err = *__errno_location();
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
            printf("Could not bind UDP receive port. Error= %d , strerror = %s\n", err, strerror(err));
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

int UDPSocket::send(char* buf, int size, unsigned short nPort, const char* szDestIp)
{
    if ((buf == NULL) || (size < 1))
    {
        return -1;
    }
    if (szDestIp == NULL)
    {
        return 0;
    }
    sockaddr_in to;
    memset(&to, 0, 0x10);
    to.sin_family = 2;
    to.sin_port = htons(nPort);
    to.sin_addr.s_addr = inet_addr(szDestIp);
    int n_bytes = sendto(sock_, buf, size, 0, (sockaddr*)&to, 0x10);
    if (n_bytes < 0)
    {
        if (((*__errno_location() == 0xb) || (*__errno_location() == 0xb)) ||
            (*__errno_location() == 4))
        {
            return 0;
        }
    }
    return n_bytes;
}

int UDPSocket::recv(char* buf, int size)
{
    if ((buf == NULL) || (size < 1))
    {
        return -1;
    }
    int fromLen = 0x10;
    int n = recvfrom(sock_, buf, size, 0, (sockaddr*)&from_, (socklen_t*)&fromLen);
    if (n < 0)
    {
        return 0;
    }
    return n;
}

void UDPSocket::close()
{
    if (sock_ == -1)
        return;
    ::close(sock_);
    sock_ = -1;
    port_ = 0;
}

bool UDPSocket::setOptNonBlock()
{
    int flags = fcntl(sock_, 3, 0);
    flags = flags | 0x800;
    if (fcntl(sock_, 4, flags) < 0)
    {
        return 0;
    }
    return 1;
}

bool UDPSocket::setOptResizeSendBuf(int size)
{
    if (size < 1)
    {
        return false;
    }
    int nRet = setsockopt(sock_, 1, 7, &size, 4);
    if (nRet < 0)
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
    int nRet = setsockopt(sock_, 1, 8, &size, 4);
    if (nRet < 0)
    {
        return false;
    }
    return true;
}

SOCKET UDPSocket::getHandle()
{
    return sock_;
}

bool UDPSocket::pollReadEvent() const
{
    fd_set set;
    int result;
    unsigned int __i;
    fd_set* __arr = &set;
    for (__i = 0; __i < sizeof(fd_set) / sizeof(__fd_mask); ++__i)
    {
        __arr->fds_bits[__i] = 0;
    }
    FD_SET((unsigned int)sock_, &set);
    timeval waitTimeStr;
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    result = select(2, &set, NULL, NULL, &waitTimeStr);
    if (result == 0)
    {
        return false;
    }
    return true;
}

bool UDPSocket::pollWriteEvent() const
{
    fd_set set;
    int result;
    unsigned int __i;
    fd_set* __arr = &set;
    for (__i = 0; __i < sizeof(fd_set) / sizeof(__fd_mask); ++__i)
    {
        __arr->fds_bits[__i] = 0;
    }
    FD_SET((unsigned int)sock_, &set);
    timeval waitTimeStr;
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    result = select(2, NULL, &set, NULL, &waitTimeStr);
    if (result == 0)
    {
        return false;
    }
    return true;
}

bool UDPSocket::pollErrorEvent() const
{
    fd_set set;
    int result;
    unsigned int __i;
    fd_set* __arr = &set;
    for (__i = 0; __i < sizeof(fd_set) / sizeof(__fd_mask); ++__i)
    {
        __arr->fds_bits[__i] = 0;
    }
    FD_SET((unsigned int)sock_, &set);
    timeval waitTimeStr;
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    result = select(2, NULL, NULL, &set, &waitTimeStr);
    if (result == 0)
    {
        return false;
    }
    return true;
}
