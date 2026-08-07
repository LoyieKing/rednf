#include "Socket.h"
#include "ChannelServiceApp.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

TCPSocket::TCPSocket()
{
    sock_ = -1;
    memset(c_adrs_, 0, 4);
    port_ = 0;
}

TCPSocket::~TCPSocket()
{
}

bool TCPSocket::open()
{
    sock_ = socket(2, 1, 6);
    bool bRet = sock_ != -1;
    if (bRet)
    {
        setOptLinger(false);
    }
    else
    {
        printf("Could not create a TDP socket : %d\n", *__errno_location());
    }
    return bRet;
}

bool TCPSocket::bind(unsigned short port, bool bNonBlock)
{
    setOptReuseAdrs(true);
    sockaddr_in svradr;
    memset(&svradr, 0, 0x10);
    svradr.sin_family = 2;
    svradr.sin_port = htons(port);
    svradr.sin_addr.s_addr = 0;
    int nRet = ::bind(sock_, (sockaddr*)&svradr, 0x10);
    if (nRet < 0)
    {
        puts("[TCPSocket::bind] Bind error");
        close();
        return false;
    }
    else
    {
        if (bNonBlock)
        {
            setOptNonBlock();
        }
        printf("succeeded in binding TCP socket port #%d\n", (unsigned int)port);
        return true;
    }
}

bool TCPSocket::listen(int back_log)
{
    int nRet = ::listen(sock_, back_log);
    if (-1 >= nRet)
    {
        puts("[TCPSocket::listen] Listen error");
        close();
    }
    return -1 < nRet;
}

bool TCPSocket::connect(const char* ip, unsigned short port)
{
    sockaddr_in svradr;
    memset(&svradr, 0, 0x10);
    svradr.sin_family = 2;
    svradr.sin_addr.s_addr = inet_addr(ip);
    svradr.sin_port = htons(port);
    int len = 0x10;
    int nRet = ::connect(sock_, (sockaddr*)&svradr, 0x10);
    if (-1 < nRet)
    {
        memcpy(c_adrs_, (char*)&svradr.sin_addr + 2, 4);
        port_ = svradr.sin_port;
    }
    else
    {
        ChannelServiceApp::gFileLogError << "[TCPSocket::connect] Can't connect : ip" << ip << "port=" << (unsigned int)port << endl;
        ChannelServiceApp::gFileLogError << "Error=" << strerror(*__errno_location()) << endl;
    }
    return -1 < nRet;
}

int TCPSocket::send(char* buf, int size)
{
    if ((buf == NULL) || (size < 1))
    {
        ChannelServiceApp::gFileLogInfo << "!buf or size<1 :" << endl;
        return -1;
    }
    else
    {
        int n_bytes = write(sock_, buf, size);
        if (n_bytes < 1)
        {
            if (((*__errno_location() == 0xb) || (*__errno_location() == 4)) ||
                (*__errno_location() == 0xb) || (*__errno_location() == 0))
            {
                ChannelServiceApp::gFileLogInfo << "tcp send fail= " << n_bytes << " error = " << strerror(*__errno_location()) << endl;
                return 0;
            }
            else
            {
                ChannelServiceApp::gFileLogInfo << "tcp send fail= " << n_bytes << " error = " << strerror(*__errno_location()) << endl;
                return -1;
            }
        }
        return n_bytes;
    }
}

int TCPSocket::recv(char* buf, int size)
{
    int n_bytes = read(sock_, buf, size);
    if (n_bytes < 1)
    {
        ChannelServiceApp::gFileLogInfo << "tcp recv fail= " << n_bytes << " error = " << strerror(*__errno_location()) << endl;
        return -1;
    }
    return n_bytes;
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
}

int TCPSocket::shutdown(int how)
{
    return ::shutdown(sock_, how);
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
    return accepted.sock_ != -1;
}

bool TCPSocket::setOptLinger(bool b)
{
    linger l;
    l.l_onoff = (int)b;
    l.l_linger = 0;
    int nRet = setsockopt(sock_, 1, 0xd, &l, 8);
    return -1 < nRet;
}

bool TCPSocket::setOptNonBlock()
{
    int flags = fcntl(sock_, 3, 0);
    flags = flags | 0x800;
    int nRet = fcntl(sock_, 4, flags);
    if (-1 >= nRet)
    {
        puts("[TCPSocket::nonblock] Can't make nonblock socket");
    }
    return -1 < nRet;
}

bool TCPSocket::setOptReuseAdrs(bool b)
{
    int t = (int)b;
    int nRet = setsockopt(sock_, 1, 2, &t, 4);
    return -1 < nRet;
}

bool TCPSocket::setOptResizeSendBuf(int size)
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

bool TCPSocket::setOptResizeRecvBuf(int size)
{
    if (size < 1)
    {
        return false;
    }
    int rcvbuf = 0;
    socklen_t optlen = 4;
    getsockopt(sock_, 1, 8, &rcvbuf, &optlen);
    int nRet = setsockopt(sock_, 1, 8, &size, 4);
    if (nRet < 0)
    {
        return false;
    }
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

SOCKET TCPSocket::getHandle() const
{
    return sock_;
}

bool TCPSocket::pollReadEvent() const
{
    fd_set set;
    FD_ZERO(&set);
    FD_SET(sock_, &set);
    timeval waitTimeStr;
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    int result = select(2, &set, NULL, NULL, &waitTimeStr);
    return result != 0;
}

bool TCPSocket::pollWriteEvent() const
{
    fd_set set;
    FD_ZERO(&set);
    FD_SET(sock_, &set);
    timeval waitTimeStr;
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    int result = select(2, NULL, &set, NULL, &waitTimeStr);
    return result != 0;
}

bool TCPSocket::pollErrorEvent() const
{
    fd_set set;
    FD_ZERO(&set);
    FD_SET(sock_, &set);
    timeval waitTimeStr;
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    int result = select(2, NULL, NULL, &set, &waitTimeStr);
    return result != 0;
}

UDPSocket::UDPSocket()
{
    sock_ = -1;
    port_ = 0;
}

UDPSocket::~UDPSocket()
{
}

bool UDPSocket::open()
{
    sock_ = socket(2, 2, 0);
    return sock_ != -1;
}

bool UDPSocket::bind(unsigned short port, bool bNonBlock)
{
    sockaddr_in svradr;
    memset(&svradr, 0, 0x10);
    svradr.sin_family = 2;
    svradr.sin_port = htons(port);
    svradr.sin_addr.s_addr = 0;
    int nRet = ::bind(sock_, (sockaddr*)&svradr, 0x10);
    if (nRet < 0)
    {
        puts("[UDPSocket::bind] Bind error");
        close();
        return false;
    }
    else
    {
        if (bNonBlock)
        {
            setOptNonBlock();
        }
        printf("succeeded in binding UDP socket port #%d\n", (unsigned int)port);
        return true;
    }
}

int UDPSocket::send(char* buf, int size, unsigned short nPort, const char* szDestIp)
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
    int n_bytes = sendto(sock_, buf, size, 0, (sockaddr*)&to, 0x10);
    if ((n_bytes < 0) && ((*__errno_location() == 0xb) || (*__errno_location() == 0xb) || (*__errno_location() == 4)))
    {
        n_bytes = 0;
    }
    return n_bytes;
}

int UDPSocket::recv(char* buf, int size)
{
    socklen_t len = 0x10;
    return recvfrom(sock_, buf, size, 0, (sockaddr*)&adrs_, &len);
}

void UDPSocket::close()
{
    if (sock_ != -1)
    {
        ::close(sock_);
        sock_ = -1;
        port_ = 0;
    }
}

SOCKET UDPSocket::getHandle()
{
    return sock_;
}

bool UDPSocket::setOptNonBlock()
{
    int flags = fcntl(sock_, 3, 0);
    flags = flags | 0x800;
    int nRet = fcntl(sock_, 4, flags);
    return -1 < nRet;
}

bool UDPSocket::setOptResizeSendBuf(int size)
{
    if (size < 1)
    {
        return false;
    }
    int nRet = setsockopt(sock_, 1, 7, &size, 4);
    return -1 < nRet;
}

bool UDPSocket::setOptResizeRecvBuf(int size)
{
    if (size < 1)
    {
        return false;
    }
    int nRet = setsockopt(sock_, 1, 8, &size, 4);
    return -1 < nRet;
}

bool UDPSocket::pollReadEvent() const
{
    fd_set set;
    FD_ZERO(&set);
    FD_SET(sock_, &set);
    timeval waitTimeStr;
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    int result = select(2, &set, NULL, NULL, &waitTimeStr);
    return result != 0;
}

bool UDPSocket::pollWriteEvent() const
{
    fd_set set;
    FD_ZERO(&set);
    FD_SET(sock_, &set);
    timeval waitTimeStr;
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    int result = select(2, NULL, &set, NULL, &waitTimeStr);
    return result != 0;
}

bool UDPSocket::pollErrorEvent() const
{
    fd_set set;
    FD_ZERO(&set);
    FD_SET(sock_, &set);
    timeval waitTimeStr;
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    int result = select(2, NULL, NULL, &set, &waitTimeStr);
    return result != 0;
}
