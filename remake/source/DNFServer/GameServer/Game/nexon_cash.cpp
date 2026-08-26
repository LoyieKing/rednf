// ============================================================================
// nexon::cash 域实现 —— socket / ByteBuffer / NCashMessage 组。
// 逐符号按 df_game_r ORIG 反汇编转写；地址见各函数注释。
// Transaction/UserBalance/TransactionManager/NCashImpl/NexonCash 见文件后段
// （由 NexonTxnTranscribe / NexonImplTranscribe 批次结论落地）。
// ============================================================================
#include "nexon_cash.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <algorithm>
#include <ctime>
#include <map>
#include <vector>
#include <new>
#include <string.h>
#include <wchar.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

namespace nexon {
namespace cash {

// ===========================================================================
// TCPSocket
// ===========================================================================

// ORIG 0x081af4cc (sz 0x42)：m_socket=-1; m_sendRetryCount=0;
// memset(&m_peerAdrs,0,4); m_peerPort=0。
TCPSocket::TCPSocket()
    : m_socket(-1), m_sendRetryCount(0)
{
    memset(&m_pad04, 0, sizeof(m_pad04));
    m_peerAdrs = 0;
    m_peerPort = 0;
}

// ORIG 0x081af50e (sz 0x0f)：反汇编体仅把参数 port 存入栈局部并返回，
// 不触及 this、无任何字段写入（弱符号内联残留形态）。忠实保留为空操作。
TCPSocket::TCPSocket(const char* host, unsigned short port)
    : m_socket(-1), m_sendRetryCount(0)
{
    (void)host;
    (void)port;
}

// ORIG 0x081af51e：仅调用 close()。
TCPSocket::~TCPSocket()
{
    close();
}

// ORIG 0x081af532：
//   m_socket = socket(AF_INET=2, SOCK_STREAM=1, IPPROTO_TCP=6);
//   if (m_socket == -1) { printf("Could not create a TCP socket : %d\n",
//        errno); return false; }  // 字符串 @0x08bb4aac
//   setOptLinger(false); setOptNagle(false); return true;
bool TCPSocket::open()
{
    m_socket = ::socket(2, 1, 6);
    if (m_socket == -1)
    {
        int err = errno;
        printf("Could not create a TCP socket : %d\n", err);
        return false;
    }
    setOptLinger(false);
    setOptNagle(false);
    return true;
}

// ORIG 0x081af5b4：setOptReuseAdrs(true)；本地 sockaddr_in{AF_INET,htons(port),
// INADDR_ANY}；bind 失败（结果的符号位）→ close(); return false；
// 成功且 nonBlock → setOptNonBlock()；printf("TCP Port binding success, port
// = %d\n", port)（字符串 @0x08bb4ad0）；return true。
bool TCPSocket::bind(unsigned short port, bool nonBlock)
{
    setOptReuseAdrs(true);

    struct sockaddr_in adrs;
    memset(&adrs, 0, sizeof(adrs));
    adrs.sin_family = 2;
    adrs.sin_port = htons(port);
    adrs.sin_addr.s_addr = 0;

    if (::bind(m_socket, reinterpret_cast<struct sockaddr*>(&adrs),
               sizeof(adrs)) < 0)
    {
        close();
        return false;
    }
    if (nonBlock)
        setOptNonBlock();
    printf("TCP Port binding success, port = %d\n", static_cast<int>(port));
    return true;
}

// ORIG 0x081af674：listen(m_socket, backlog) 符号位失败 → close(); false。
bool TCPSocket::listen(int backlog)
{
    if (::listen(m_socket, backlog) < 0)
    {
        close();
        return false;
    }
    return true;
}

// ORIG 0x081af6ae：
//   if (!buffer || !length) return -1;
//   n = write(m_socket, buffer, length); savedErrno = errno;
//   if (n > 0) { m_sendRetryCount = 0; return n; }
//   if (savedErrno==EAGAIN || savedErrno==EINTR || savedErrno==EAGAIN) {
//       ++m_sendRetryCount;
//       if (m_sendRetryCount > MAX_SEND_RETRY) { m_sendRetryCount = 0;
//           return -100; }
//       return 0;
//   }
//   if (savedErrno == 0) return 0;
//   m_sendRetryCount = 0; return -1;
int TCPSocket::send(char* buffer, unsigned int length)
{
    if (buffer == 0 || length == 0)
        return -1;

    int n = static_cast<int>(::write(m_socket, buffer, length));
    int savedErrno = errno;

    if (n > 0)
    {
        m_sendRetryCount = 0;
        return n;
    }
    if (savedErrno == EAGAIN || savedErrno == EINTR || savedErrno == EAGAIN)
    {
        ++m_sendRetryCount;
        if (static_cast<int>(m_sendRetryCount) > 0x64)  // MAX_SEND_RETRY
        {
            m_sendRetryCount = 0;
            return -100;
        }
        return 0;
    }
    if (savedErrno == 0)
        return 0;
    m_sendRetryCount = 0;
    return -1;
}

// ORIG 0x081af76e：
//   if (!buffer || !length) return -1;
//   n = read(...); if (n < 0) { e=errno; e==EAGAIN→0; e==EINTR→0;
//       e==EAGAIN→0; e!=0→-1; /*e==0*/ →0; }
//   else if (n == 0) return -1;   // 对端关闭
//   return n;
int TCPSocket::recv(char* buffer, unsigned int length)
{
    if (buffer == 0 || length == 0)
        return -1;

    int n = static_cast<int>(::read(m_socket, buffer, length));
    if (n < 0)
    {
        if (errno == EAGAIN)
            return 0;
        if (errno == EINTR)
            return 0;
        if (errno == EAGAIN)
            return 0;
        if (errno != 0)
            return -1;
        return 0;  // 0x81af7da 路径：errno==0 时仍走 ret 0
    }
    if (n == 0)
        return -1;
    return n;
}

int TCPSocket::getHandle() const
{
    return m_socket;
}

// ORIG 0x081af7fe (sz 0xd)：读 m_socket 与 -1 比较，结果丢弃，直接返回。
void TCPSocket::shutdown(int how)
{
    // 忠实转写：ORIG 本函数未发起 shutdown 系统调用（内联裁剪后的残体）。
    (void)how;
    const bool unusedCheck = (m_socket == -1);
    (void)unusedCheck;
}

// ORIG 0x081af80c：
//   if (m_socket == -1) return;
//   close(m_socket); m_socket=-1; memset(this+0x14,0,4); m_peerPort=0;
void TCPSocket::close()
{
    if (m_socket == -1)
        return;
    ::close(m_socket);
    m_socket = -1;
    m_peerAdrs = 0;
    m_peerPort = 0;
}

// ORIG 0x081af85e：flags=fcntl(F_GETFL); flags|=O_NONBLOCK(0x800);
// fcntl(F_SETFL, flags) 符号位失败→false。
bool TCPSocket::setOptNonBlock()
{
    int flags = ::fcntl(m_socket, 3 /*F_GETFL*/, 0);
    flags |= 0x800;
    if (::fcntl(m_socket, 4 /*F_SETFL*/, flags) < 0)
        return false;
    return true;
}

// ORIG 0x081af8bc：setsockopt(SOL_SOCKET, SO_REUSEADDR=2, &flag(0/1), 4)。
bool TCPSocket::setOptReuseAdrs(bool on)
{
    int flag = on ? 1 : 0;
    if (::setsockopt(m_socket, 1, 2, &flag, sizeof(flag)) < 0)
        return false;
    return true;
}

// ORIG 0x081af926：linger{l_onoff=(on?1:0), l_linger=0}；
// setsockopt(SOL_SOCKET, SO_LINGER=13, &l, 8)，负结果→false。
bool TCPSocket::setOptLinger(bool on)
{
    int opt[2];
    opt[0] = on ? 1 : 0;  // l_onoff
    opt[1] = 0;           // l_linger
    if (::setsockopt(m_socket, 1, 13, opt, sizeof(opt)) < 0)
        return false;
    return true;
}

// ORIG 0x081af990：setsockopt(IPPROTO_TCP=6, TCP_NODELAY=1, &(on?1:0), 4)；
// 返回 setsockopt()==0。
bool TCPSocket::setOptNagle(bool on)
{
    int flag = on ? 1 : 0;
    return ::setsockopt(m_socket, 6, 1, &flag, sizeof(flag)) == 0;
}

// ORIG 0x081af9ea：本地 sockaddr_in{AF_INET,inet_addr(host),htons(port)}；
// connect 失败→false；成功则 memcpy(&m_peerAdrs,&sin_addr,4)、
// m_peerPort=sin_port，return true。
bool TCPSocket::connect(const char* host, unsigned short port)
{
    struct sockaddr_in adrs;
    memset(&adrs, 0, sizeof(adrs));
    adrs.sin_family = 2;
    adrs.sin_addr.s_addr = inet_addr(host);
    adrs.sin_port = htons(port);

    socklen_t len = sizeof(adrs);
    if (::connect(m_socket, reinterpret_cast<struct sockaddr*>(&adrs), len) < 0)
        return false;

    memcpy(&m_peerAdrs, &adrs.sin_addr, 4);
    m_peerPort = adrs.sin_port;
    return true;
}

// ORIG 0x081afa98 (sz 0x230)：非阻塞连接 + select 等待 + SO_ERROR 校验。
bool TCPSocket::connect_nonb(const char* host, unsigned short port,
                             timeval timeout)
{
    struct sockaddr_in adrs;
    memset(&adrs, 0, sizeof(adrs));
    adrs.sin_family = 2;
    adrs.sin_addr.s_addr = inet_addr(host);
    adrs.sin_port = htons(port);

    socklen_t len = sizeof(adrs);

    setOptNonBlock();

    int result = ::connect(m_socket,
                           reinterpret_cast<struct sockaddr*>(&adrs), len);
    if (result < 0)
    {
        int e = errno;
        if (!(e == 0x73 /*EINPROGRESS*/ || e == EINTR || e == EAGAIN ||
              e == EAGAIN))
            return false;
    }
    if (result == 0)
    {
        memcpy(&m_peerAdrs, &adrs.sin_addr, 4);
        m_peerPort = adrs.sin_port;
        return true;
    }

    fd_set writeSet;
    FD_ZERO(&writeSet);
    FD_SET(m_socket, &writeSet);

    result = ::select(m_socket + 1, 0, &writeSet, 0, &timeout);
    if (result < 0 && errno != EINTR)
        return false;
    if (result <= 0)
        return false;

    int soError = 0;
    socklen_t optLen = 4;
    if (::getsockopt(m_socket, 1 /*SOL_SOCKET*/, 4 /*SO_ERROR*/,
                     &soError, &optLen) < 0)
        return false;
    if (soError != 0)
        return false;

    memcpy(&m_peerAdrs, &adrs.sin_addr, 4);
    m_peerPort = adrs.sin_port;
    return true;
}

// ORIG 0x081aff2c：peer.m_socket = accept(m_socket, &peer.m_adrs…?)。
// 细节：accept 的地址缓冲写入 peer+0x04（sin 区域），len=16；
// 结果 <0 或 ==-1 → false；
// memcpy(&peer.m_peerAdrs, peer_adrs+0x08, 4)（即 sin_addr）；
// peer.m_peerPort = *(u16*)(adrs+0x06)（sin_port 原样）；
// peer.setOptNonBlock(); return true。
bool TCPSocket::accept(TCPSocket& peer)
{
    socklen_t len = sizeof(peer.m_pad04);  // 16 == sizeof(sockaddr_in)
    peer.m_socket = ::accept(m_socket, reinterpret_cast<struct sockaddr*>(
                                            peer.m_pad04), &len);
    if (peer.m_socket < 0 || peer.m_socket == -1)
        return false;

    // ORIG: memcpy(dst=this+0x14(视图字段), src=adrs+0x08(sin_addr), 4)
    memcpy(&peer.m_peerAdrs, peer.m_pad04 + 8, 4);
    unsigned short rawPort;
    memcpy(&rawPort, peer.m_pad04 + 6, 2);
    peer.m_peerPort = rawPort;
    peer.setOptNonBlock();
    return true;
}

// ORIG 0x081affb8：lea eax,[this+0x14]; ret —— 返回成员 m_peerAdrs 的地址。
unsigned int* TCPSocket::getPeerAdrs()
{
    return &m_peerAdrs;
}

unsigned short TCPSocket::getPeerPort()
{
    return m_peerPort;
}

bool TCPSocket::setOptResizeSendBuf(int size)
{
    if (size <= 0)
        return false;
    if (::setsockopt(m_socket, 1, 7 /*SO_SNDBUF*/, &size, sizeof(size)) < 0)
        return false;
    return true;
}

bool TCPSocket::setOptResizeRecvBuf(int size)
{
    if (size <= 0)
        return false;
    if (::setsockopt(m_socket, 1, 8 /*SO_RCVBUF*/, &size, sizeof(size)) < 0)
        return false;
    return true;
}

void TCPSocket::SetRecvBufSize(int size)
{
    msRecvBufSize = size;
}

void TCPSocket::SetSendBufSize(int size)
{
    msSendBufSize = size;
}

// 共同骨架（ORIG 三者同构，仅 select 第二/三/四参与超时不同）：
// 清零 fd_set(32*int)，FD_SET(m_socket)，select(nfds=2, ...)，ret !=0。
bool TCPSocket::pollReadEvent() const
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_socket, &fds);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return ::select(2, &fds, 0, 0, &timeout) != 0;
}

bool TCPSocket::pollWriteEvent() const
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_socket, &fds);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return ::select(2, 0, &fds, 0, &timeout) != 0;
}

bool TCPSocket::pollErrorEvent() const
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_socket, &fds);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return ::select(2, 0, 0, &fds, &timeout) != 0;
}

int TCPSocket::msRecvBufSize = 0;  // B 0x09407d68
int TCPSocket::msSendBufSize = 0;  // B 0x09407d6c


// ===========================================================================
// UDPSocket
// ===========================================================================

// ORIG 0x081aed2c：m_socket=-1; m_port=0。
UDPSocket::UDPSocket()
    : m_socket(-1), m_port(0)
{
}

// ORIG 0x081aed44：仅调用 close()。
UDPSocket::~UDPSocket()
{
    close();
}

// ORIG 0x081aed58：
//   if (m_socket != -1) return false;
//   m_socket = socket(AF_INET, SOCK_DGRAM(2), 0);
//   if (-1) printf("Could not create a UDP socket : %d\n", errno) → false;
//   else true。字符串 @0x08bb49e4。
bool UDPSocket::open()
{
    if (m_socket != -1)
        return false;
    m_socket = ::socket(2, 2, 0);
    if (m_socket == -1)
    {
        int err = errno;
        printf("Could not create a UDP socket : %d\n", err);
        return false;
    }
    return true;
}

// ORIG 0x081aedc6：保存主机序端口；清零 m_adrs；
// m_adrs={AF_INET=2, inet_addr(host)}；sin_port=htons(m_port)；
// bind(m_socket,&m_adrs,16)；失败→printf("Udp Port binding fail, %s , %d\n",
// host, port)（@0x08bb4a08）并 false。
bool UDPSocket::bind(const char* host, unsigned short port)
{
    m_port = port;
    memset(m_adrs, 0, sizeof(m_adrs));
    *reinterpret_cast<unsigned short*>(m_adrs + 0) = 2;  // AF_INET
    unsigned int addr = inet_addr(host);
    memcpy(m_adrs + 4, &addr, 4);
    unsigned short netPort = htons(port);
    memcpy(m_adrs + 2, &netPort, 2);

    if (::bind(m_socket,
               reinterpret_cast<struct sockaddr*>(m_adrs), 16) != 0)
    {
        printf("Udp Port binding fail, %s , %d\n", host,
               static_cast<int>(port));
        return false;
    }
    return true;
}

// ORIG 0x081aee82：同上但 INADDR_ANY；失败按 errno 分支：
//   EADDRINUSE(98)→printf("Port %d for receiving UDP is in use\n",port)
//                 （@0x08bb4a28），
//   EACCES(99)→puts("Cannot assign requested address.")（@0x08bb4a50），
//   其他非零→printf("Could not bind UDP receive port. Error= %d , strerror"
//                 "=%s\n"…)（@0x08bb4a70）；errno==0 不打印。
// 失败返回 false；成功且 nonBlock→setOptNonBlock()，true。
bool UDPSocket::bind(unsigned short port, bool nonBlock)
{
    m_port = port;
    memset(m_adrs, 0, sizeof(m_adrs));
    *reinterpret_cast<unsigned short*>(m_adrs + 0) = 2;
    unsigned int any = htonl(0);
    memcpy(m_adrs + 4, &any, 4);
    unsigned short netPort = htons(port);
    memcpy(m_adrs + 2, &netPort, 2);

    if (::bind(m_socket,
               reinterpret_cast<struct sockaddr*>(m_adrs), 16) != 0)
    {
        int e = errno;
        if (e == 98 /*EADDRINUSE*/)
            printf("Port %d for receiving UDP is in use\n",
                   static_cast<int>(port));
        else if (e == 99 /*EACCES*/)
            puts("Cannot assign requested address.");
        else if (e != 0)
            printf("Could not bind UDP receive port. Error= %d , strerror=%s\n",
                   e, strerror(e));
        return false;
    }
    if (nonBlock)
        setOptNonBlock();
    return true;
}

// ORIG 0x081af000：
//   if (!data || length<=0) return -1; if (!host) return 0;
//   目标 sockaddr_in{AF_INET,htons(port),inet_addr(host)}；
//   n = sendto(m_socket,data,length,0,&adrs,16)；
//   n<0 且 errno∈{EAGAIN,EINTR,EAGAIN}→0；否则 n。
int UDPSocket::send(char* data, int length, unsigned short port,
                    const char* host)
{
    if (data == 0 || length <= 0)
        return -1;
    if (host == 0)
        return 0;

    struct sockaddr_in adrs;
    memset(&adrs, 0, sizeof(adrs));
    adrs.sin_family = 2;
    adrs.sin_port = htons(port);
    adrs.sin_addr.s_addr = inet_addr(host);

    int n = static_cast<int>(::sendto(
        m_socket, data, length, 0,
        reinterpret_cast<struct sockaddr*>(&adrs),
        static_cast<socklen_t>(sizeof(adrs))));
    if (n < 0)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN)
            return 0;
    }
    return n;
}

// ORIG 0x081af0de：
//   if (!buffer || length<=0) return -1;
//   addrlen=16；n=recvfrom(m_socket,buffer,length,0,(sockaddr*)&m_from,&addrlen)
//   —— ORIG 直接以 this+0x18 作为源地址存储（即成员 m_from）。
//   n<0: errno==EAGAIN→0 / EINTR→0 / EAGAIN→0 / 否则（e!=0）→-1 / e==0 落到
//        ret n；n>=0 原样返回。
int UDPSocket::recv(char* buffer, int length)
{
    if (buffer == 0 || length <= 0)
        return -1;

    socklen_t addrlen = 16;
    int n = static_cast<int>(::recvfrom(
        m_socket, buffer, length, 0,
        reinterpret_cast<struct sockaddr*>(m_from), &addrlen));
    if (n < 0)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN)
            return 0;
        if (errno != 0)
            return -1;
    }
    return n;
}

int UDPSocket::getHandle()
{
    return m_socket;
}

// ORIG 0x081af184：if(m_socket==-1)return; close(); m_socket=-1; m_port=0;
void UDPSocket::close()
{
    if (m_socket == -1)
        return;
    ::close(m_socket);
    m_socket = -1;
    m_port = 0;
}

// ORIG 0x081aefa2：与 TCPSocket 同构的 F_GETFL|O_NONBLOCK / F_SETFL。
bool UDPSocket::setOptNonBlock()
{
    int flags = ::fcntl(m_socket, 3, 0);
    flags |= 0x800;
    if (::fcntl(m_socket, 4, flags) < 0)
        return false;
    return true;
}

// ORIG 0x081af1b8 / 0x081af210：size<=0→false；
// setsockopt(SOL_SOCKET, SO_SNDBUF/SO_RCVBUF, &size, 4)，负→false。
bool UDPSocket::setOptResizeSendBuf(int size)
{
    if (size <= 0)
        return false;
    if (::setsockopt(m_socket, 1, 7, &size, sizeof(size)) < 0)
        return false;
    return true;
}

bool UDPSocket::setOptResizeRecvBuf(int size)
{
    if (size <= 0)
        return false;
    if (::setsockopt(m_socket, 1, 8, &size, sizeof(size)) < 0)
        return false;
    return true;
}

// ORIG 0x081af268：read 集 select(nfds=2)，超时 {0,5000}微秒，ret!=0。
bool UDPSocket::pollReadEvent() const
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_socket, &fds);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0x1388;
    return ::select(2, &fds, 0, 0, &timeout) != 0;
}

// ORIG 0x081af334：write 集超时 {0,0}。
bool UDPSocket::pollWriteEvent() const
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_socket, &fds);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return ::select(2, 0, &fds, 0, &timeout) != 0;
}

// ORIG 0x081af400：except 集，超时 {0,0}。
bool UDPSocket::pollErrorEvent() const
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_socket, &fds);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return ::select(2, 0, 0, &fds, &timeout) != 0;
}

// ===========================================================================
// ByteBuffer
// ===========================================================================

// ORIG 0x081ae9e2：start=write=read=buffer; capacity=len。
ByteBuffer::ByteBuffer(char* buffer, unsigned int len)
    : m_start(buffer), m_writePtr(buffer), m_readPtr(buffer), m_capacity(len)
{
}

// ORIG 0x081ae9b8：调用 ctor 后按值返回 this（-O0 下 sret 形态等价于自赋值）。
ByteBuffer& ByteBuffer::wrap(char* buffer, unsigned int len)
{
    this->~ByteBuffer();
    new (this) ByteBuffer(buffer, len);
    return *this;
}

// ORIG 0x081acf60：length = m_writePtr - m_start。
unsigned int ByteBuffer::length() const
{
    return static_cast<unsigned int>(m_writePtr - m_start);
}

// 内联通用形态：cap 校验通过则逐字节大端写入，推进 write 游标；
// 空间不足时静默丢弃（游标不动），ORIG 无返回错误通道。

// ORIG 0x081acba0：写 C 字符串含结尾 '\0'（strlen(buf)+1 字节，无容量预检、
// 无上限——直接循环写入直到 NUL，之后 ++writePtr 收尾）。
ByteBuffer& ByteBuffer::operator<<(const char* value)
{
    // 忠实转写：ORIG 用 strlen 一次性拷贝（无空间校验，见汇编 cmp 分支仅对
    // 迭代计数）。实现保持逐字节语义等价。
    size_t len = strlen(value) + 1;
    for (size_t i = 0; i < len; ++i)
        *m_writePtr++ = value[i];
    return *this;
}

// ORIG 0x081acd1e：wchar_t 版本按宿主宽字符逐个写出（转写为字节级截取，
// 与 ORIG 在 Linux wchar_t=4B 平台上的原始双字拷贝一致）。
ByteBuffer& ByteBuffer::operator<<(const wchar_t* value)
{
    size_t len = wcslen(value) + 1;
    const char* raw = reinterpret_cast<const char*>(value);
    for (size_t i = 0; i < len * sizeof(wchar_t); ++i)
        *m_writePtr++ = raw[i];
    return *this;
}

// ORIG 0x081ae614：if(length()+1 < m_capacity){ *wp=v; ++wp; }
ByteBuffer& ByteBuffer::operator<<(unsigned char value)
{
    if (static_cast<unsigned int>(length() + 1) < m_capacity)
    {
        *m_writePtr = static_cast<char>(value);
        ++m_writePtr;
    }
    return *this;
}

namespace {

// 大端逐字节模板化核心（对应 <<s/<<i/<<j 的展开形）。
inline void write_be_n(char* dst, const char* src, unsigned int n)
{
    for (unsigned int i = 0; i < n; ++i)
        dst[i] = src[n - 1 - i];
}

}  // namespace

// ORIG 0x081ae65e：if(length()+2 <= cap) 写 2B 大端。
ByteBuffer& ByteBuffer::operator<<(short value)
{
    if (static_cast<unsigned int>(length() + 2) <= m_capacity)
    {
        char tmp[sizeof(short)];
        memcpy(tmp, &value, sizeof(tmp));
        write_be_n(m_writePtr, tmp, 2);
        m_writePtr += 2;
    }
    return *this;
}

// ORIG 0x081acefa：u16 版本。
ByteBuffer& ByteBuffer::operator<<(unsigned short value)
{
    if (static_cast<unsigned int>(length() + 2) <= m_capacity)
    {
        char tmp[sizeof(unsigned short)];
        memcpy(tmp, &value, sizeof(tmp));
        write_be_n(m_writePtr, tmp, 2);
        m_writePtr += 2;
    }
    return *this;
}

// ORIG 0x081ae6c4：int 大端 4B。
ByteBuffer& ByteBuffer::operator<<(int value)
{
    if (static_cast<unsigned int>(length() + 4) <= m_capacity)
    {
        char tmp[4];
        memcpy(tmp, &value, 4);
        write_be_n(m_writePtr, tmp, 4);
        m_writePtr += 4;
    }
    return *this;
}

// ORIG 0x081ae74c：u32 版本。
ByteBuffer& ByteBuffer::operator<<(unsigned int value)
{
    if (static_cast<unsigned int>(length() + 4) <= m_capacity)
    {
        char tmp[4];
        memcpy(tmp, &value, 4);
        write_be_n(m_writePtr, tmp, 4);
        m_writePtr += 4;
    }
    return *this;
}

// ORIG 0x081ae7d4：u64 大端 8B。
ByteBuffer& ByteBuffer::operator<<(unsigned long long value)
{
    if (static_cast<unsigned int>(length() + 8) <= m_capacity)
    {
        char tmp[8];
        memcpy(tmp, &value, 8);
        write_be_n(m_writePtr, tmp, 8);
        m_writePtr += 8;
    }
    return *this;
}

// ORIG 0x081ae8bc：从 read 游标读 1B 并推进。
ByteBuffer& ByteBuffer::operator>>(unsigned char& value)
{
    value = static_cast<unsigned char>(*m_readPtr);
    ++m_readPtr;
    return *this;
}

namespace {

inline void read_be_n(char* dst, const char* src, unsigned int n)
{
    for (unsigned int i = 0; i < n; ++i)
        dst[i] = src[n - 1 - i];
}

}  // namespace

// ORIG 0x081ae8e4：读 4B 大端组成 int。
ByteBuffer& ByteBuffer::operator>>(int& value)
{
    char tmp[4];
    read_be_n(tmp, m_readPtr, 4);
    memcpy(&value, tmp, 4);
    m_readPtr += 4;
    return *this;
}

// ORIG 0x081ae94e：u32 版本。
ByteBuffer& ByteBuffer::operator>>(unsigned int& value)
{
    char tmp[4];
    read_be_n(tmp, m_readPtr, 4);
    memcpy(&value, tmp, 4);
    m_readPtr += 4;
    return *this;
}

// ===========================================================================
// NCashMessage
// ===========================================================================

// ORIG 0x081ae52e：magic='U'; bodyLength=0。
NCashMessage::NCashMessage()
    : m_magic('U'), m_pad01(0), m_bodyLength(0)
{
}

unsigned char* NCashMessage::data()
{
    return reinterpret_cast<unsigned char*>(&m_raw[0]);  // this+4
}

unsigned char* NCashMessage::body()
{
    return reinterpret_cast<unsigned char*>(&m_raw[3]);  // this+7
}

unsigned short NCashMessage::body_length()
{
    return m_bodyLength;
}

// ORIG 0x081ae54e：return u16(this+2)+3。
int NCashMessage::length() const
{
    return static_cast<int>(m_bodyLength) + 3;
}

// ORIG 0x081ae578：raw[0]='U'; u16@raw[1]=htons(bodyLength)；true。
bool NCashMessage::encode_header()
{
    m_raw[0] = 'U';
    unsigned short netLen = htons(m_bodyLength);
    memcpy(&m_raw[1], &netLen, 2);
    return true;
}

// ORIG 0x081ae5ae：bodyLength 参数直存 +0x02 后调用 encode_header()。
void NCashMessage::encode_header(unsigned int bodyLength)
{
    m_bodyLength = static_cast<unsigned short>(bodyLength);
    encode_header();
}

// ORIG 0x081ae5ce：m_bodyLength=ntohs(u16(raw[1]))；
// 若 >0x200 → 清零返 false，否则 true。
bool NCashMessage::decode_header()
{
    unsigned short netLen;
    memcpy(&netLen, &m_raw[1], 2);
    m_bodyLength = ntohs(netLen);
    if (static_cast<int>(m_bodyLength) > 0x200)
    {
        m_bodyLength = 0;
        return false;
    }
    return true;
}

// ===========================================================================
// Transaction
// ===========================================================================

// ORIG 0x081b02ec (sz 0x49)：id=0→getNextID；amount、createdTime=0→time(0)。
Transaction::Transaction(UserBalance* userBalance, int amount)
    : m_id(0), m_amount(amount), m_createdTime(0), m_userBalance(userBalance)
{
    m_id = TransactionManager::getNextTransactionID();
    m_createdTime = static_cast<int>(time(NULL));
}

// ORIG 0x081b0336：balance < amount（有符号 setl）→ 3；
// 否则 balance(-amount) + addTransaction → 0。ORIG 无二次执行防护，忠实保留。
int Transaction::execute()
{
    if (m_userBalance->balance() < m_amount)
        return 3;
    m_userBalance->balance(-m_amount);
    TransactionManager::instance().addTransaction(this);
    return 0;
}

// ORIG 0x081b0398：仅出管理器登记表。
void Transaction::confirm()
{
    TransactionManager::instance().removeTransaction(this);
}

// ORIG 0x081b03b4：先退款再出表。
void Transaction::cancel()
{
    m_userBalance->balance(m_amount);
    TransactionManager::instance().removeTransaction(this);
}

// ORIG 0x081b1656：time(NULL)-createdTime > 15。
bool Transaction::expired() const
{
    return (static_cast<int>(time(NULL)) - m_createdTime) > 15;
}

unsigned int Transaction::id() const            // ORIG 0x081b050c
{
    return m_id;
}

UserBalance* Transaction::getUser() const       // ORIG 0x081b0516
{
    return m_userBalance;
}

// ===========================================================================
// UserBalance
// ===========================================================================

int UserBalance::balance() const                // ORIG 0x081b0522 W
{
    return m_balance;
}

void UserBalance::balance(int delta)            // ORIG 0x081b052e W：+=
{
    m_balance += delta;
}

// ORIG 0x081b15ca：printf("addNewTx: %p\n", tx)（fmt @0x8bb55c0 原样）+ push_back。
void UserBalance::addNewTransaction(Transaction* tx)
{
    printf("addNewTx: %p\n", tx);
    m_transactions.push_back(tx);
}

// ORIG 0x081b1530：erase(remove(begin,end,tx), end) 惯用法（ORIG 实例化了
// std::remove 与两参 erase 的 W 符号）。
void UserBalance::removeTransaction(Transaction* tx)
{
    std::vector<Transaction*>::iterator last = std::remove(
        m_transactions.begin(), m_transactions.end(), tx);
    m_transactions.erase(last, m_transactions.end());
}

// ORIG 0x081b147c：遍历删除 expired 者（tx->cancel() 退款+出管理器表，
// erase(it) 复用迭代器）。
void UserBalance::expire()
{
    std::vector<Transaction*>::iterator it = m_transactions.begin();
    while (it != m_transactions.end())
    {
        if ((*it)->expired())
        {
            (*it)->cancel();
            it = m_transactions.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

// ===========================================================================
// TransactionManager
// ===========================================================================

int TransactionManager::tx_id_ = 0;             // B 0x09407d90

// ORIG 0x081b0128：guard 单例（ORIG guard@0x9407db8 / obj@0x9407dc0 +
// __cxa_atexit(D1)）。C++98 局部 static 由编译器生成同型代码。
TransactionManager& TransactionManager::instance()
{
    static TransactionManager i;
    return i;
}

TransactionManager::TransactionManager() {}     // ORIG 0x081b04f8：仅 map 默认构造
TransactionManager::~TransactionManager() {}    // ORIG 0x081b0544：仅 map 析构

// ORIG 0x081b01bc：insert(make_pair(tx->id(), tx))。
void TransactionManager::addTransaction(Transaction* tx)
{
    m_transactions.insert(std::make_pair(tx->id(), tx));
}

// ORIG 0x081b021c：erase(key)。
void TransactionManager::removeTransaction(Transaction* tx)
{
    m_transactions.erase(tx->id());
}

// ORIG 0x081b0444：find == end → NULL。
Transaction* TransactionManager::getTransaction(unsigned int id)
{
    std::map<unsigned int, Transaction*>::const_iterator it =
        m_transactions.find(id);
    if (it == m_transactions.end())
        return 0;
    return it->second;
}

// ORIG 0x081b0244：tx 存在→tx->cancel()（退款+自移除）、user 表移除、delete tx。
void TransactionManager::cancel(unsigned int id)
{
    Transaction* tx = getTransaction(id);
    if (tx == 0)
        return;
    tx->cancel();
    tx->getUser()->removeTransaction(tx);
    delete tx;
}

// ORIG 0x081b0298：同 cancel 但不退款（tx->confirm() 仅自移除）。
void TransactionManager::confirm(unsigned int id)
{
    Transaction* tx = getTransaction(id);
    if (tx == 0)
        return;
    tx->confirm();
    tx->getUser()->removeTransaction(tx);
    delete tx;
}

// ORIG 0x081b04cc：前缀 ++ 后返回新值。
unsigned int TransactionManager::getNextTransactionID()
{
    return ++TransactionManager::tx_id_;
}

}  // namespace cash
}  // namespace nexon

