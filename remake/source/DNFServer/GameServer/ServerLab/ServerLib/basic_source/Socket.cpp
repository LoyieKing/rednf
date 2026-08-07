#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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
    sock_ = socket(2, 1, 6);
    bool bRet = sock_ != -1;
    if (bRet)
    {
        setOptResizeSendBuf(G_Script()->findIntValue(0, 0xb));
        setOptResizeRecvBuf(G_Script()->findIntValue(0, 10));
        setOptLinger(false);
        setOptNagle(false);
    }
    else
    {
        printf("Could not create a TDP socket : %d\n", errno);
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
    int ret = ::listen(sock_, back_log);
    if (ret <= -1)
    {
        close();
    }
    return ret > -1;
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
            mSendRetryCount = mSendRetryCount + 1;
            if (mSendRetryCount < MAX_SEND_RETRY)
            {
                G_TraceLog()->sysLog(0, "tcp send retry='%d', error ='%s(%d)'", n_bytes, strerror(error_number), error_number);
                return 0;
            }
            G_TraceLog()->sysLog(7, "So many retry. so disconnect him, %d.%d.%d.%d:%d", c_adrs_[0], c_adrs_[1], c_adrs_[2], c_adrs_[3], port_);
            mSendRetryCount = 0;
            return -100;
        }
        else if (error_number == 0)
        {
            G_TraceLog()->sysLog(0, "if errno == 0 then Critcal Problem!! YOU MUST CHECK THIS!!!");
            return 0;
        }
        else
        {
            G_TraceLog()->sysLog(0, "tcp send fail='%d', error ='%s'", n_bytes, strerror(error_number));
            return -1;
        }
    }
    G_TraceLog()->sysLog(0, "tcp send ='%d'", n_bytes);
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
        if ((errno == 0xb) || (errno == 4) || (errno == 0xb) || (errno == 0))
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
    sock_ != -1;
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

int TCPSocket::setOptNonBlock()
{
    int flags = fcntl(sock_, F_GETFL, 0);
    flags = flags | O_NONBLOCK;
    int ret = fcntl(sock_, F_SETFL, flags);
    if (ret < 0)
    {
        return 0;
    }
    return ret;
}

bool TCPSocket::setOptReuseAdrs(bool b)
{
    int t = (unsigned int)b;
    return setsockopt(sock_, 1, 2, &t, 4) > -1;
}

bool TCPSocket::setOptLinger(bool b)
{
    linger l;
    l.l_onoff = (unsigned int)b;
    l.l_linger = 0;
    return setsockopt(sock_, 1, 0xd, &l, 8) > -1;
}

bool TCPSocket::setOptNagle(bool b)
{
    int opt_val = (unsigned int)b;
    return setsockopt(sock_, 6, 1, &opt_val, 4) == 0;
}

bool TCPSocket::setOptResizeSendBuf(int size)
{
    if (size < 1)
    {
        return false;
    }
    return setsockopt(sock_, 1, 7, &size, 4) >= 0;
}

bool TCPSocket::setOptResizeRecvBuf(int size)
{
    if (size < 1)
    {
        return false;
    }
    return setsockopt(sock_, 1, 8, &size, 4) >= 0;
}

bool TCPSocket::connect(const char* ip, unsigned short port)
{
    sockaddr_in svradr;
    memset(&svradr, 0, 0x10);
    svradr.sin_family = 2;
    svradr.sin_addr.s_addr = inet_addr(ip);
    svradr.sin_port = htons(port);
    int ret = ::connect(sock_, (sockaddr*)&svradr, 0x10);
    if (ret > -1)
    {
        memcpy(c_adrs_, &svradr.sin_addr, 4);
        port_ = svradr.sin_port;
    }
    else
    {
        G_TraceLog()->sysLog(7, "CONNECTION FAIL IP=%s, PORT=%d, reason=%s", ip, (unsigned int)port, strerror(errno));
    }
    return ret > -1;
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
    int error = 0;
    int n = ::connect(sock_, (sockaddr*)&svradr, len);
    if (((n < 0 && errno != 0x73) && (errno != 4)) && ((errno != 0xb && errno != 0xb)))
    {
        return false;
    }
    if (n != 0)
    {
        fd_set wset;
        FD_ZERO(&wset);
        FD_SET(sock_, &wset);
        n = select(sock_ + 1, NULL, &wset, NULL, &tval);
        if ((n < 0 && errno != 4))
        {
            return false;
        }
        if (n < 1)
        {
            return false;
        }
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
    fd_set rset;
    fd_set* p = &rset;
    for (unsigned int i = 0; i <= 0x1f; i = i + 1)
    {
        p->fds_bits[i] = 0;
    }
    FD_SET((unsigned int)sock_, &rset);
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    int ret = select(2, &rset, NULL, NULL, &tv);
    if (ret == 0)
    {
        return 0;
    }
    return 1;
}

int TCPSocket::pollWriteEvent() const
{
    fd_set wset;
    fd_set* p = &wset;
    for (unsigned int i = 0; i <= 0x1f; i = i + 1)
    {
        p->fds_bits[i] = 0;
    }
    FD_SET((unsigned int)sock_, &wset);
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    int ret = select(2, NULL, &wset, NULL, &tv);
    if (ret == 0)
    {
        return 0;
    }
    return 1;
}

int TCPSocket::pollErrorEvent() const
{
    fd_set eset;
    fd_set* p = &eset;
    for (unsigned int i = 0; i <= 0x1f; i = i + 1)
    {
        p->fds_bits[i] = 0;
    }
    FD_SET((unsigned int)sock_, &eset);
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    int ret = select(2, NULL, NULL, &eset, &tv);
    if (ret == 0)
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
}

bool UDPSocket::open()
{
    if (sock_ == -1)
    {
        sock_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_ == -1)
        {
            printf("Could not create a UDP socket : %d\n", errno);
            return false;
        }
        return true;
    }
    return false;
}

bool UDPSocket::bind(const char* strIp, unsigned short port)
{
    this->port_ = port;
    memset(&this->adrs_, 0, 0x10);
    this->adrs_.sin_family = AF_INET;
    this->adrs_.sin_addr.s_addr = inet_addr(strIp);
    this->adrs_.sin_port = htons(this->port_);
    int iResult = ::bind(this->sock_, (sockaddr*)&this->adrs_, 0x10);
    if (iResult != 0)
    {
        printf("Udp Port binding fail, %s , %d\n", strIp, (unsigned int)port);
    }
    return iResult == 0;
}

bool UDPSocket::bind(unsigned short port, bool bNonBlock)
{
    this->port_ = port;
    memset(&this->adrs_, 0, 0x10);
    this->adrs_.sin_family = AF_INET;
    this->adrs_.sin_addr.s_addr = htonl(0);
    this->adrs_.sin_port = htons(this->port_);
    int iResult = ::bind(this->sock_, (sockaddr*)&this->adrs_, 0x10);
    if (iResult == 0)
    {
        if (bNonBlock)
        {
            setOptNonBlock();
        }
        return true;
    }
    int err = errno;
    if (err == 0x62)
    {
        printf("Port %d for receiving UDP is in use\n", (unsigned int)port);
    }
    else if (err == 99)
    {
        puts("Cannot assign requested address");
    }
    else if (err != 0)
    {
        printf("Could not bind UDP receive port. Error= %d , strerror = %s\n", err, strerror(err));
    }
    return false;
}

bool UDPSocket::setOptNonBlock()
{
    unsigned int flags = fcntl(this->sock_, F_GETFL, 0);
    int iResult = fcntl(this->sock_, F_SETFL, flags | O_NONBLOCK);
    return iResult > -1;
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
    sockaddr to;
    memset(&to, 0, 0x10);
    to.sa_family = AF_INET;
    *(unsigned short*)to.sa_data = htons(nPort);
    *(unsigned int*)(to.sa_data + 2) = inet_addr(szDestIp);
    int n_bytes = sendto(this->sock_, buf, size, 0, &to, 0x10);
    if ((n_bytes < 0) && ((errno == 0xb) || (errno == 4)))
    {
        n_bytes = 0;
    }
    return n_bytes;
}

int UDPSocket::recv(char* buf, const int size)
{
    if ((buf == NULL) || (size < 1))
    {
        return -1;
    }
    socklen_t fromLen = 0x10;
    int len = recvfrom(this->sock_, buf, size, 0, (sockaddr*)&this->from_, &fromLen);
    if (len < 0)
    {
        if ((errno == 0xb) || (errno == 4))
        {
            len = 0;
        }
        else
        {
            len = -1;
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
    fd_set rset;
    fd_set* p = &rset;
    for (unsigned int i = 0; i <= 0x1f; i = i + 1)
    {
        p->fds_bits[i] = 0;
    }
    FD_SET((unsigned int)sock_, &rset);
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    int ret = select(2, &rset, NULL, NULL, &tv);
    if (ret == 0)
    {
        return 0;
    }
    return 1;
}

int UDPSocket::pollWriteEvent() const
{
    fd_set wset;
    fd_set* p = &wset;
    for (unsigned int i = 0; i <= 0x1f; i = i + 1)
    {
        p->fds_bits[i] = 0;
    }
    FD_SET((unsigned int)sock_, &wset);
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    int ret = select(2, NULL, &wset, NULL, &tv);
    if (ret == 0)
    {
        return 0;
    }
    return 1;
}

int UDPSocket::pollErrorEvent() const
{
    fd_set eset;
    fd_set* p = &eset;
    for (unsigned int i = 0; i <= 0x1f; i = i + 1)
    {
        p->fds_bits[i] = 0;
    }
    FD_SET((unsigned int)sock_, &eset);
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    int ret = select(2, NULL, NULL, &eset, &tv);
    if (ret == 0)
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
}

bool UDPSocket::setOptResizeSendBuf(int size)
{
    if (size < 1)
    {
        return false;
    }
    int iResult = setsockopt(this->sock_, SOL_SOCKET, SO_SNDBUF, &size, 4);
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
    int iResult = setsockopt(this->sock_, SOL_SOCKET, SO_RCVBUF, &size, 4);
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
