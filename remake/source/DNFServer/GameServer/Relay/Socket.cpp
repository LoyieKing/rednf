// df_relay_r — TCPSocket/UDPSocket（GCC 4.1.2, 无 DWARF — Ghidra 反汇编还原）
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>

#include "RelayService.h"
#include "Helper.h"
#include "ScriptData.h"
#include "Socket.h"
#include "ThreadLock.h"

// ---- TCPSocket ----

TCPSocket::TCPSocket()
{
    sock_ = -1;
    peer_port_ = 0;
    memset((char*)this + 0x14, 0, 4);
    memset((char*)this + 4, 0, 0x10);
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
        int nErr = *__errno_location();
        return 0;
    }
    setOptLinger(false);
    setOptRecvBufferSize(0xa000);
    return 1;
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
        close();
        return 0;
    }
    if (bNonBlock)
    {
        setOptNonBlock();
    }
    printf("succeeded in binding TCP socket port #%d\n", (unsigned int)port);
    return 1;
}

bool TCPSocket::listen(int backlog)
{
    if (::listen(sock_, backlog) < 0)
    {
        close();
        return 0;
    }
    return 1;
}

ssize_t TCPSocket::send(char* buf, int len)
{
    if ((buf == 0) || (len < 1))
    {
        return -1;
    }
    ssize_t r = write(sock_, buf, len);
    if (r < 1)
    {
        if ((*__errno_location() == EAGAIN) || (*__errno_location() == EINTR) ||
            (*__errno_location() == EAGAIN) || (*__errno_location() == 0))
        {
            return 0;
        }
        return -1;
    }
    return r;
}

ssize_t TCPSocket::recv(char* buf, int len)
{
    if ((buf == 0) || (len < 1))
    {
        return -1;
    }
    ssize_t r = read(sock_, buf, len);
    if (r < 0)
    {
        if ((*__errno_location() == EAGAIN) || (*__errno_location() == EINTR) ||
            (*__errno_location() == EAGAIN) || (*__errno_location() == 0))
        {
            return 0;
        }
        return -1;
    }
    else if (r == 0)
    {
        return -1;
    }
    return r;
}

int TCPSocket::shutdown(int how)
{
    // 语义还原（2026-08-11 用户规矩：不允许硬套 asm）。
    (void)how;
    sock_ == -1;
    return *(int*)this;
}

void TCPSocket::close()
{
    if (sock_ == -1)
    {
        return;
    }
    ::close(sock_);
    sock_ = -1;
    memset((char*)this + 0x14, 0, 4);
    peer_port_ = 0;
}

bool TCPSocket::setOptNonBlock()
{
    int flags = fcntl(sock_, 3, 0);
    flags = flags | 0x800;
    if (fcntl(sock_, 4, flags) < 0)
    {
        return 0;
    }
    return 1;
}

bool TCPSocket::setOptReuseAdrs(bool bReuse)
{
    int t = 0;
    if (bReuse)
    {
        t = 1;
    }
    else
    {
        t = 0;
    }
    if (setsockopt(sock_, 1, 2, &t, 4) < 0)
    {
        return 0;
    }
    return 1;
}

bool TCPSocket::setOptLinger(bool bLinger)
{
    int v[2];
    v[0] = bLinger ? 1 : 0;
    v[1] = 0;
    if (setsockopt(sock_, 1, 0xd, v, 8) < 0)
    {
        return 0;
    }
    return 1;
}

bool TCPSocket::setOptNagle(bool bNagle)
{
    int v;
    v = bNagle ? 0 : 1;
    if (setsockopt(sock_, 6, 1, &v, 4) < 0)
    {
        return 0;
    }
    return 1;
}

bool TCPSocket::setOptRecvBufferSize(int size)
{
    if (setsockopt(sock_, 1, 8, &size, 4) < 0)
    {
        return 0;
    }
    return 1;
}

bool TCPSocket::connect(char const* ip, unsigned short port)
{
    sockaddr_in svradr;
    memset(&svradr, 0, 0x10);
    svradr.sin_family = 2;
    svradr.sin_addr.s_addr = inet_addr(ip);
    svradr.sin_port = htons(port);
    int len = 0x10;
    if (::connect(sock_, (sockaddr*)&svradr, len) < 0)
    {
        return 0;
    }
    memcpy(&peer_ip_, &svradr.sin_addr, 4);
    peer_port_ = svradr.sin_port;
    setOptNonBlock();
    return 1;
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

bool TCPSocket::accept(TCPSocket& new_sock)
{
    socklen_t len = 0x10;
    new_sock.sock_ = ::accept(sock_, (sockaddr*)&new_sock.adrs_, &len);
    if ((new_sock.sock_ < 0) || (new_sock.sock_ == -1))
    {
        return 0;
    }
    memcpy(&new_sock.peer_ip_, &new_sock.adrs_.sin_addr, 4);
    new_sock.peer_port_ = new_sock.adrs_.sin_port;
    return 1;
}

char* TCPSocket::getPeerAdrs()
{
    return (char*)this + 0x14;
}

unsigned short TCPSocket::getPeerPort()
{
    return peer_port_;
}

int TCPSocket::setOptResizeSendBuf(int size)
{
    if (size < 1)
    {
        return 0;
    }
    int r = setsockopt(sock_, 1, 7, &size, 4);
    if (r < 0)
    {
        return 0;
    }
    return 1;
}

int TCPSocket::setOptResizeRecvBuf(int size)
{
    if (size < 1)
    {
        return 0;
    }
    int r = setsockopt(sock_, 1, 8, &size, 4);
    if (r < 0)
    {
        return 0;
    }
    return 1;
}

// ---- UDPSocket ----

UDPSocket::UDPSocket()
    : sock_(-1), port_(0)
{
    memset((char*)this + 4, 0, 0x10);
    memset((char*)this + 0x18, 0, 0x10);
}

UDPSocket::~UDPSocket()
{
    close();
}

bool UDPSocket::open()
{
    if (sock_ != -1)
    {
        return 0;
    }
    sock_ = socket(2, 2, 0);
    if (sock_ == -1)
    {
        int err = *__errno_location();
        printf("Could not create a UDP socket : %d\n", err);
        return 0;
    }
    return 1;
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
        case 99:
            puts("Cannot assign requested address");
            break;
        case 0:
            break;
        default:
            printf("Could not bind UDP receive port. Error= %d , strerror = %s\n", err, strerror(err));
            break;
        }
        return 0;
    }
    if (bNonBlock)
    {
        setOptNonBlock();
    }
    return 1;
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

void UDPSocket::delDisconnectUser(unsigned int acc_id)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > scoped(lock_);
    if (monitor_set.find(acc_id) != monitor_set.end())
    {
        monitor_set.erase(monitor_set.find(acc_id));
    }
}

void UDPSocket::pushMonitorAuthPacket(unsigned int acc_id)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > scoped(lock_);
    if (monitor_set.find(acc_id) == monitor_set.end())
    {
        monitor_queue.push(acc_id);
        monitor_set.insert(acc_id);
    }
}

void UDPSocket::popMonitorAuthPacket()
{
    TScopedLock<TThreadLock<ThreadLock_linux> > scoped(lock_);
    unsigned int acc_id;
    while (monitor_queue.size() != 0)
    {
        acc_id = monitor_queue.front();
        monitor_queue.pop();
        if (monitor_set.find(acc_id) != monitor_set.end())
        {
            monitor_set.erase(monitor_set.find(acc_id));
            RelayServiceApp::Packet_Relay_User_Check packet;
            packet.m_f = acc_id;
            packet.m_g = G_ScriptData()->mReservedD;
            int nRet = send((char*)&packet, 0xf, G_ScriptData()->mReservedC,
                            G_ScriptData()->mServerIpB, 0);
            break;
        }
    }
}

unsigned int UDPSocket::sizeMonitorAuthPacket()
{
    TScopedLock<TThreadLock<ThreadLock_linux> > scoped(lock_);
    return monitor_set.size();
}

ssize_t UDPSocket::send(char* buf, int len, unsigned short port, const char* ip,
                        unsigned int ip_addr)
{
    if ((buf == 0) || (len < 1))
    {
        return -1;
    }
    if ((ip == 0) && (ip_addr == 0))
    {
        return 0;
    }
    sockaddr_in to;
    memset(&to, 0, 0x10);
    if (ip != 0)
    {
        ip_addr = inet_addr(ip);
    }
    to.sin_family = 2;
    to.sin_port = htons(port);
    to.sin_addr.s_addr = ip_addr;
    ssize_t r = sendto(sock_, buf, len, 0, (sockaddr*)&to, 0x10);
    if (r < 0)
    {
        if ((*__errno_location() == EAGAIN) || (*__errno_location() == EAGAIN) ||
            (*__errno_location() == EINTR))
        {
            return 0;
        }
    }
    return r;
}

ssize_t UDPSocket::recv(char* buf, int len)
{
    if ((buf == 0) || (len < 1))
    {
        return -1;
    }
    int slen = 0x10;
    ssize_t r = recvfrom(sock_, buf, len, 0, (sockaddr*)&from_, (socklen_t*)&slen);
    if (r < 0)
    {
        return 0;
    }
    return r;
}

int UDPSocket::getHandle()
{
    return sock_;
}

void UDPSocket::close()
{
    if (sock_ == -1)
    {
        return;
    }
    ::close(sock_);
    sock_ = -1;
    port_ = 0;
}

int UDPSocket::setOptResizeSendBuf(int size)
{
    if (size < 1)
    {
        return 0;
    }
    int r = setsockopt(sock_, 1, 7, &size, 4);
    if (r < 0)
    {
        return 0;
    }
    return 1;
}

int UDPSocket::setOptResizeRecvBuf(int size)
{
    if (size < 1)
    {
        return 0;
    }
    int r = setsockopt(sock_, 1, 8, &size, 4);
    if (r < 0)
    {
        return 0;
    }
    return 1;
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
