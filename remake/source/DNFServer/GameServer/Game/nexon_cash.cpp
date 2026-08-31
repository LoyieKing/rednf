// ============================================================================
// nexon::cash 域实现 —— socket / ByteBuffer / NCashMessage 组。
// 逐符号按 df_game_r ORIG 反汇编转写；地址见各函数注释。
// Transaction/UserBalance/TransactionManager/NCashImpl/NexonCash 见文件后段
// （由 NexonTxnTranscribe / NexonImplTranscribe 批次结论落地）。
// ============================================================================
#include "nexon_cash.h"

#include "GameTypes.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <algorithm>
#include <ctime>
#include <map>
#include <vector>
#include <cstdlib>
#include <new>
#include <string.h>
#include <wchar.h>
#include <limits>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// ORIG 0x0807e190/0x0807e8e0 为 PLT 死引用：_ZN10Encryption7Adler32EPcj 与
// _ZN10Encryption21EncodeDressStringWideEPKwPwm 在 df_game_r 中仅是 GLOBAL UND
//（无任何 T/W 定义、无重定位解析目标），chargeCash 调用分支从未可执行。
namespace Encryption
{
unsigned int Adler32(char* data, unsigned int length);                      // _ZN10Encryption7Adler32EPcj
void EncodeDressStringWide(const wchar_t* in, wchar_t* out,
                           unsigned long param);                            // _ZN10Encryption21EncodeDressStringWideEPKwPwm

__attribute__((weak)) unsigned int Adler32(char*, unsigned int) { return 0; }
__attribute__((weak)) void EncodeDressStringWide(const wchar_t*, wchar_t*,
                                                 unsigned long) {}
}  // namespace Encryption

namespace nexon {
namespace cash {

TCPSocket::TCPSocket()
    : m_socket(INVALID_SOCKET), m_sendRetryCount(0)
{
    memset(&m_peerAdrs, 0, sizeof(m_peerAdrs));   // ORIG memset(this+0x14,0,4)
    m_peerPort = 0;
}
// ORIG 0x081af50e (sz 0x0f)：反汇编体仅把参数 port 存入栈局部并返回，
// 不触及 this、无任何字段写入（弱符号内联残留形态）。忠实保留为空操作。
TCPSocket::TCPSocket(const char* host, unsigned short port)
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
    if (m_socket == INVALID_SOCKET)
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
        if (static_cast<int>(m_sendRetryCount) > static_cast<int>(TCPSocket::MAX_SEND_RETRY))  // MAX_SEND_RETRY
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
    // ORIG：mov m_socket; cmp $0xffffffff; 裸比较无 sete（结果丢弃）。
    (void)how;
    (void)(m_socket == INVALID_SOCKET);
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

// 返回 setsockopt()==0。ORIG 反汇编：结果先存栈变量再 cmpl+sete 返回。
bool TCPSocket::setOptNagle(bool on)
{
    int flag = on ? 1 : 0;
    int ret = ::setsockopt(m_socket, 6, 1, &flag, sizeof(flag));
    return ret == 0;
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
    if (peer.m_socket < 0 || peer.m_socket == INVALID_SOCKET)
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
const unsigned int TCPSocket::MAX_SEND_RETRY;  // R 0x08bb49e0（类内 =0x64 的类外定义，发射 R 符号）


// ===========================================================================
// UDPSocket
// ===========================================================================

// ORIG 0x081aed2c：m_socket = INVALID_SOCKET; m_port=0。
UDPSocket::UDPSocket()
    : m_socket(INVALID_SOCKET), m_port(0)
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
    if (m_socket != INVALID_SOCKET)
        return false;
    m_socket = ::socket(2, 2, 0);
    if (m_socket == INVALID_SOCKET)
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

// ORIG 0x081af184：if (m_socket == INVALID_SOCKET)return; close(); m_socket=-1; m_port=0;
void UDPSocket::close()
{
    if (m_socket == -1)
        return;
    ::close(m_socket);
    m_socket = INVALID_SOCKET;
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

// ORIG 0x081acba0：strlen → 超 ushort::max 抛 invalid_argument("string length
// too long: must shorter than max(unsigned short)")；length()+len 不超容量时
// 写 ushort 长度前缀（大端）+ strncpy(len 字节，不含 NUL）+ 游标前进 strlen。
ByteBuffer& ByteBuffer::operator<<(const char* value)
{
    int len = strlen(value);
    if (len > (int)std::numeric_limits<unsigned short>::max())
        throw std::invalid_argument("string length too long: must shorter than max(unsigned short)");
    if (length() + len <= m_capacity)
    {
        *this << (unsigned short)len;
        strncpy(m_writePtr, value, len);
        m_writePtr += strlen(value);
    }
    return *this;
}

// ORIG 0x081acd1e：len=wcslen；字节数=len*2；超 0xffff 抛 invalid_argument；
// 空间足够则写 ushort 长度前缀（大端）+ wcsncpy + 游标前进 len*2。
ByteBuffer& ByteBuffer::operator<<(const wchar_t* value)
{
    size_t len = wcslen(value);
    size_t bytes = len * 2;
    if (bytes > (size_t)std::numeric_limits<unsigned short>::max())
        throw std::invalid_argument("string too long");
    if (static_cast<unsigned int>(length() + bytes) <= m_capacity)
    {
        *this << static_cast<unsigned short>(bytes);
        wcsncpy(reinterpret_cast<wchar_t*>(m_writePtr), value, len);
        m_writePtr += bytes;
    }
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
    : m_magic('U'), m_bodyLength(0)
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

// ===========================================================================
// 外部计费加密接口。ORIG 中亦为未定义引用：
//   U _ZN10Encryption7Adler32EPcj / U _ZN10Encryption21EncodeDressStringWideEPKwPwm
// 由计费库链接期提供，此处仅声明。
// ===========================================================================

namespace {

// chargeCash 的函数作用域静态工作缓冲（ORIG b 符号实证）：
//   data@0x09405640 / wide_data@0x9405a40 / enc_data@0x94069e0 / send_data@0x9407980
struct ChargeCashBuffers
{
    char     data_[1000];
    wchar_t  wideData_[2000];
    wchar_t  encData_[2000];
    char     sendData_[1000];
};
ChargeCashBuffers& ccBufs()
{
    static ChargeCashBuffers bufs;   // 等价 ORIG 四个独立函数级 static
    return bufs;
}


namespace {

// NCashImpl 内嵌的出包消息子对象 @this+0x248（布局常量，见头注释）。
inline NCashMessage& outMsgOf(NCashImpl* self)
{
    return *reinterpret_cast<NCashMessage*>(
        reinterpret_cast<char*>(self) + 0x248);
}

}  // namespace

inline void traceRecvError(int errCode, const char* plain)
{
    cMyTrace trace(__FUNCTION__, 0x303, 0);
    trace("NCashImpl::chargeCash Recv Error!! packet_id:%d, data:%s",
          errCode, plain);
}

}  // namespace

// ===========================================================================
// NCashImpl
// ===========================================================================

// ORIG 0x081acfdc：子对象构造顺序与字段初始化见头注释。
NCashImpl::NCashImpl(const char* serverAdrs, short port, unsigned char paramB,
                     unsigned short fieldC, short fieldD)
    : m_socket(), m_inMsg(), m_outMsg()
{
    // ORIG 0x081acfdc：子对象构造（m_socket@+0x20、m_inMsg@+0x40、m_outMsg@+0x248）
    // 后依次写 m_requestID=0、m_paramB、m_fieldC、m_fieldD、m_connected=0、
    // strcpy(m_serverAdrs, serverAdrs)（无 NULL 检查/预清零）、m_port。
    m_requestID = 0;
    m_paramB = paramB;
    m_fieldC = fieldC;
    m_fieldD = fieldD;
    m_connected = false;
    strcpy(m_serverAdrs, serverAdrs);
    m_port = port;
}

void NCashImpl::_increaseRequestID()                 // ORIG 0x081aea32
{
    ++m_requestID;
}

bool NCashImpl::checkCoupon()     { return false; }   // ORIG 0x081aea0a xor eax,eax
bool NCashImpl::useCoupon()       { return false; }   // ORIG 0x081aea14
bool NCashImpl::checkCafeCoupon() { return false; }   // ORIG 0x081aea1e
bool NCashImpl::useCafeCoupon()   { return false; }   // ORIG 0x081aea28

// ORIG 0x081aea46：MSG_WAITALL(0x100) 预读头并解码，随后转 recv_body；
// 断线/错误时 printf + m_connected=false + close + 返回 2。
int NCashImpl::recv_msg()
{
    unsigned char* buf = m_inMsg.data();
    int fd = m_socket.getHandle();

    ssize_t n = ::recv(fd, buf, 3, 0x100);   // ORIG: 预读 3 字节头
    if (n == 0)
    {
        printf("[%s] connection lost: %s\n", __FUNCTION__,
               strerror(errno));
        m_connected = false;
        m_socket.close();
        return 2;
    }
    if (n < 0)
    {
        printf("[%s] recv error: %s\n", __FUNCTION__, strerror(errno));
        m_connected = false;
        m_socket.close();
        return 2;
    }

    m_inMsg.decode_header();
    return recv_body();
}

// ORIG 0x081aeb3c：按 body_length 继续收 body 区（flags 同 0x100）。
int NCashImpl::recv_body()
{
    unsigned short bodyLen = m_inMsg.body_length();
    unsigned char* body = m_inMsg.body();
    int fd = m_socket.getHandle();

    ssize_t n = ::recv(fd, body, bodyLen, 0x100);
    if (n == 0)
    {
        printf("[%s] connection lost: %s\n", __FUNCTION__, strerror(errno));
        m_connected = false;
        m_socket.close();
        return 2;
    }
    if (n < 0)
    {
        printf("[%s] recv error: %s\n", __FUNCTION__, strerror(errno));
        m_connected = false;
        m_socket.close();
        return 2;
    }
    return 0;
}

// ORIG 0x081aec2c：encode(totalLen) → send(data, length()+3 视图全长, 0x100)；
// 发送不全则 printf + close + 2。
int NCashImpl::send_msg(unsigned int totalLen)
{
    outMsgOf(this).encode_header(totalLen);
    unsigned int len = outMsgOf(this).length();
    unsigned char* buf = outMsgOf(this).data();
    int fd = m_socket.getHandle();

    ssize_t n = ::send(fd, buf, len, 0x100);
    if (static_cast<unsigned int>(n) != len)
    {
        printf("send error: send bytes: %d, message length: %d, error: %s\n",
               static_cast<int>(n), static_cast<int>(len), strerror(errno));
        m_connected = false;
        m_socket.close();
        return 2;
    }
    return 0;
}

// ORIG 0x081ad080：未连接则重开重连；返回连接态。
bool NCashImpl::reconnect()
{
    if (!m_connected)
    {
        m_socket.close();
        m_socket.open();
        m_connected = m_socket.connect(m_serverAdrs,
                                       static_cast<unsigned short>(m_port));
    }
    return m_connected;
}

// ORIG 0x081ad0e6（sret）：ByteBuffer(out.body(),0x200)<<pid<<flag<<requestID。
ByteBuffer NCashImpl::set_data_header(unsigned char packetId, unsigned char flag)
{
    ByteBuffer out(reinterpret_cast<char*>(outMsgOf(this).body()), 0x200);
    out << packetId;
    out << flag;
    _increaseRequestID();
    out << m_requestID;
    return out;
}

// ORIG 0x081ad168：从入包 body 读 pid/flag/requestId 三元组。
void NCashImpl::get_data_header(unsigned char& a, unsigned char& b,
                                unsigned int& c)
{
    ByteBuffer in(0, 0);
    in.wrap(reinterpret_cast<char*>(m_inMsg.body()), m_inMsg.body_length());
    in >> a;
    in >> b;
    in >> c;
}

// ORIG 0x081ad1e0：switch(err-5) 跳转表分支（默认 16）。
int NCashImpl::handle_cash_error(int err)
{
    int result = 0x10;
    // ORIG 跳转表（0x8bb42d4）逐项：case0=err5→r3"E_GIFT_LIMIT_OVER Remain cash"、
    // case1=err6→r4"NO REMIN CASH"、case2=err7→default(r0x10)、case3=err8→r5"E_GIFT_NOT_READY
    // error"、case4=err9→r6"E_GIFT_UNDER_AGE error"、case5=err10→r7"Undefined cash error"。
    switch (err - 5)
    {
    case 1:      // 置于 case0 前，使块1=r4（NO REMIN）、块2=r3（GIFT_LIMIT）匹配 ORIG 块序
        result = 4;
        printf("[%s] NO REMIN CASH\n", __FUNCTION__);
        break;
    case 0:
        result = 3;
        printf("[%s] E_GIFT_LIMIT_OVER Remain cash\n", __FUNCTION__);
        break;
    case 3:
        result = 5;
        printf("[%s] E_GIFT_NOT_READY error\n", __FUNCTION__);
        break;
    case 4:
        result = 6;
        printf("[%s] E_GIFT_UNDER_AGE error\n", __FUNCTION__);
        break;
    case 5:
        result = 7;
        printf("[%s] Undefined cash error\n", __FUNCTION__);
        break;
    case 2:      // 与 default 同流（跳转表同项）
    default:
        result = 0x10;
        printf("[%s] Undefined cash error: %u\n", __FUNCTION__,
               static_cast<unsigned>(err));
        break;
    }
    return result;
}

// ORIG 0x081ad2c0：0..3 静默；default 打印；恒返回 16。
int NCashImpl::handle_coupon_error(int err)
{
    int result = 0x10;
    switch (err)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        break;
    default:
        printf("[%s] Invalid coupon packet_id: %u\n", __FUNCTION__,
               static_cast<unsigned>(err));
        break;
    }
    return result;
}

// ORIG 0x081ad316：手组包头（不走 set_data_header！仅自增 requestID 不写入）。
int NCashImpl::checkEntrance(const char* userName, char& outStatus)
{
    if (!reconnect())
        return 2;

    unsigned char typeByte = static_cast<unsigned char>(m_fieldD);

    ByteBuffer out(reinterpret_cast<char*>(outMsgOf(this).body()), 0x200);
    out << static_cast<unsigned char>(0x03);
    out << static_cast<unsigned char>(0x01);
    out << typeByte;
    out << userName;
    _increaseRequestID();  // ORIG: 自增但不入包

    int rc = send_msg(out.length());
    if (rc != 0)
        return rc;

    rc = recv_msg();
    if (rc != 0)
        return rc;

    ByteBuffer in(0, 0);
    in.wrap(reinterpret_cast<char*>(m_inMsg.body()), m_inMsg.body_length());
    unsigned char ackPid = 0;
    unsigned char statusByte = 0;
    in >> ackPid;
    in >> statusByte;
    outStatus = static_cast<char>(statusByte);
    printf("[%s] CheckEntrace: %d\n", __FUNCTION__,
           static_cast<int>(outStatus));  // ORIG 格式串 typo 忠实保留
    return 0;
}

// ORIG 0x081ad49e：packet_id=0 应答 err==0 后读余额。
int NCashImpl::getBalance(const char* userId, int& balanceOut)
{
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x00);
    msg << userId;

    int rc = send_msg(msg.length());
    if (rc != 0)
        return rc;

    rc = recv_msg();
    if (rc != 0)
        return rc;

    unsigned char ackPid = 0, errCode = 0;
    unsigned int reqId = 0;
    get_data_header(ackPid, errCode, reqId);
    if (errCode != 0)
        return handle_cash_error(errCode);

    ByteBuffer in(0, 0);
    in.wrap(reinterpret_cast<char*>(m_inMsg.body()), m_inMsg.body_length());
    in >> balanceOut;
    printf("[%s] Remain cash: %d\n", __FUNCTION__, balanceOut);
    return 0;
}

// ORIG 0x081ad5c2。
int NCashImpl::purchase(unsigned long long productSeq, int cashType,
                        int cashAmount, const char* param1, const char* param2,
                        NXTransaction& trOut)
{
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x01);
    msg << static_cast<short>(m_fieldD);
    msg << productSeq;
    msg << cashType;
    msg << cashAmount;
    msg << param1;
    msg << param2;
    msg << m_paramB;
    msg << static_cast<short>(m_fieldC);

    int rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    unsigned char ackPid = 0, errCode = 0;
    unsigned int reqId = 0;
    get_data_header(ackPid, errCode, reqId);
    if (errCode != 2)
        return handle_cash_error(errCode);

    ByteBuffer in(0, 0);
    in.wrap(reinterpret_cast<char*>(m_inMsg.body()), m_inMsg.body_length());
    in >> trOut.m_type;
    in >> trOut.m_id;
    printf("[%s] PURCHASE OK, db_id: %u, transaction_id: %u\n", __FUNCTION__,
           static_cast<unsigned>(trOut.m_type),
           trOut.m_id);
    return 0;
}

// ORIG 0x081ad7ae：subCmd∈{60,62} 时 streamType 替换为 subCmd，其余同上。
int NCashImpl::purchase(unsigned long long productSeq, int cashType,
                        int cashAmount, const char* param1, const char* param2,
                        NXTransaction& trOut, short subCmd)
{
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x01);
    short streamType;
    if (subCmd == 0x3c || subCmd == 0x3e)
        streamType = subCmd;
    else
        streamType = static_cast<short>(m_fieldD);
    msg << streamType;
    msg << productSeq;
    msg << cashType;
    msg << cashAmount;
    msg << param1;
    msg << param2;
    msg << m_paramB;
    msg << static_cast<short>(m_fieldC);

    int rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    unsigned char ackPid = 0, errCode = 0;
    unsigned int reqId = 0;
    get_data_header(ackPid, errCode, reqId);
    if (errCode != 2)
        return handle_cash_error(errCode);

    ByteBuffer in(0, 0);
    in.wrap(reinterpret_cast<char*>(m_inMsg.body()), m_inMsg.body_length());
    in >> trOut.m_type;
    in >> trOut.m_id;
    printf("[%s] PURCHASE OK, db_id: %u, transaction_id: %u\n", __FUNCTION__,
           static_cast<unsigned>(trOut.m_type),
           trOut.m_id);
    return 0;
}

// ORIG 0x081ad9c4：两段附加串位于 m_paramB/m_fieldC 之后。
int NCashImpl::gift(unsigned long long productSeq, int cashType,
                    int cashAmount, const char* targetUserId,
                    const char* message, const char* extra1,
                    const char* extra2, NXTransaction& trOut)
{
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x07);
    msg << static_cast<short>(m_fieldD);
    msg << productSeq;
    msg << cashType;
    msg << cashAmount;
    msg << targetUserId;
    msg << message;
    msg << m_paramB;
    msg << static_cast<short>(m_fieldC);
    msg << extra1;
    msg << extra2;

    int rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    unsigned char ackPid = 0, errCode = 0;
    unsigned int reqId = 0;
    get_data_header(ackPid, errCode, reqId);
    if (errCode != 2)
        return handle_cash_error(errCode);

    ByteBuffer in(0, 0);
    in.wrap(reinterpret_cast<char*>(m_inMsg.body()), m_inMsg.body_length());
    in >> trOut.m_type;
    in >> trOut.m_id;
    printf("[%s] PURCHASE OK, db_id: %u, transaction_id: %u\n", __FUNCTION__,
           static_cast<unsigned>(trOut.m_type),
           trOut.m_id);
    return 0;
}

// ORIG 0x081adbd4：err!=3 时 handle_cash_error 结果被丢弃仍打印成功行返 0。
int NCashImpl::confirm(unsigned char trFlag, unsigned int trId)
{
    if (trId == 0)
        return 0;
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x03);
    msg << trFlag;
    msg << trId;

    int rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    unsigned char ackPid = 0, errCode = 0;
    unsigned int reqId = 0;
    get_data_header(ackPid, errCode, reqId);
    if (errCode != 3)
        handle_cash_error(errCode);  // ORIG 忽略返回值
    printf("[%s] E_PURCHASE_CONFIRM\n", __FUNCTION__);
    return 0;
}

// ORIG 0x081add04：与 confirm 同构，packet_id=4、__FUNCTION__="cancel"。
int NCashImpl::cancel(unsigned char trFlag, unsigned int trId)
{
    if (trId == 0)
        return 0;
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x04);
    msg << trFlag;
    msg << trId;

    int rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    unsigned char ackPid = 0, errCode = 0;
    unsigned int reqId = 0;
    get_data_header(ackPid, errCode, reqId);
    if (errCode != 3)
        handle_cash_error(errCode);  // ORIG 忽略返回值
    printf("[%s] E_PURCHASE_CONFIRM\n", __FUNCTION__);  // 共用格式串
    return 0;
}

// ORIG 0x081ade34：无 reconnect 前置；组"%s|%i|%i|%s|%s|%s|%s|%i" 明文 →
// mbstowcs → Adler32(宽串字节) → EncodeDressStringWide(key=0x1b1bde64) →
// wcstombs 回 ASCII 密文入包 + 协议键 qword + adler qword。
// snprintf token-to-slot 错位处标【推断】（见批次报告 artifact://58982 复核指引）。
int NCashImpl::chargeCash(char* sessionKey, unsigned char h1, unsigned char h2,
                          int iv, char* pA, char* pB, char* pC, char* pD,
                          int code, NXTransaction& trOut)
{
    ChargeCashBuffers& bufs = ccBufs();
    memset(bufs.data_, 0, sizeof(bufs.data_));
    memset(bufs.wideData_, 0, sizeof(bufs.wideData_));
    memset(bufs.encData_, 0, sizeof(bufs.encData_));
    memset(bufs.sendData_, 0, sizeof(bufs.sendData_));

    ByteBuffer msg = set_data_header(0x02, 0x01);

    (void)iv;
    (void)code;  // ORIG varargs 保留区【推断】：token 消费只覆盖前 8 个

    snprintf(bufs.data_, sizeof(bufs.data_),
             "%s|%i|%i|%s|%s|%s|%s|%i",
             sessionKey,
             static_cast<int>(h1),
             static_cast<int>(h2),
             reinterpret_cast<char*>(static_cast<size_t>(iv)),
             pA, pB, pC, pD);

    size_t len = strlen(bufs.data_) + 1;
    mbstowcs(bufs.wideData_, bufs.data_, len);

    size_t wlen = wcslen(bufs.wideData_);
    unsigned int adler = Encryption::Adler32(
        reinterpret_cast<char*>(bufs.wideData_),
        static_cast<unsigned int>(wlen * 2));

    Encryption::EncodeDressStringWide(bufs.wideData_, bufs.encData_,
                                      0x1b1bde64u);

    size_t enclenChars = wcslen(bufs.encData_);
    wcstombs(bufs.sendData_, bufs.encData_, enclenChars);

    msg << bufs.sendData_;
    msg << 0x00000000F3D8266FULL;         // [-0x28/-0x24] 帧 key【推断】
    msg << static_cast<unsigned long long>(adler);

    int rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    unsigned char ackPid = 0, errCode = 0;
    unsigned int reqId = 0;
    get_data_header(ackPid, errCode, reqId);

    if (errCode != 2)
    {
        traceRecvError(static_cast<int>(errCode), bufs.data_);
        return static_cast<int>(errCode);
    }

    ByteBuffer in(0, 0);
    in.wrap(reinterpret_cast<char*>(m_inMsg.body()), m_inMsg.body_length());
    in >> trOut.m_type;
    in >> trOut.m_id;
    printf("[%s] CHARGE_CASH OK, db_id: %u, transaction_id: %u\n", __FUNCTION__,
           static_cast<unsigned>(trOut.m_type), trOut.m_id);
    return 0;
}

// ===========================================================================
// NexonCash（薄包装）
// ===========================================================================

NexonCash::NexonCash(const char* serverAdrs, short port, char paramB,
                     short fieldC, short fieldD)
{
    m_impl = new NCashImpl(serverAdrs, port,
                           static_cast<unsigned char>(paramB),
                           static_cast<unsigned short>(fieldC), fieldD);
}

// ORIG 0x081ae250：free(m_impl)，非 delete —— 忠实保留（ORIG free@plt）。
NexonCash::~NexonCash()
{
    free(m_impl);
}

int NexonCash::getBalance(const char* adrs, int& balance)
{ return m_impl->getBalance(adrs, balance); }

int NexonCash::purchase(unsigned long long sn, int a, int b,
                        const char* c, const char* d, NXTransaction& tx)
{ return m_impl->purchase(sn, a, b, c, d, tx); }

int NexonCash::purchase(unsigned long long sn, int a, int b,
                        const char* c, const char* d, NXTransaction& tx,
                        short extra)
{ return m_impl->purchase(sn, a, b, c, d, tx, extra); }

int NexonCash::gift(unsigned long long sn, int a, int b, const char* c,
                    const char* d, const char* e, const char* f,
                    NXTransaction& tx)
{ return m_impl->gift(sn, a, b, c, d, e, f, tx); }

int NexonCash::confirm(const NXTransaction& tx)
{ return m_impl->confirm(tx.m_type, tx.m_id); }

int NexonCash::cancel(const NXTransaction& tx)
{ return m_impl->cancel(tx.m_type, tx.m_id); }

int NexonCash::checkCoupon()     { return m_impl->checkCoupon(); }
int NexonCash::useCoupon()       { return m_impl->useCoupon(); }
int NexonCash::checkCafeCoupon() { return m_impl->checkCafeCoupon(); }
int NexonCash::useCafeCoupon()   { return m_impl->useCafeCoupon(); }

int NexonCash::charge(char* data, unsigned char a, unsigned char b,
                      int encLen, char* encData, char* sendBuf, char* wideBuf,
                      char* more, int count, NXTransaction& tx)
{ return m_impl->chargeCash(data, a, b, encLen, encData, sendBuf, wideBuf,
                            more, count, tx); }

int NexonCash::checkEntrance(const char* adrs, char& out)
{ return m_impl->checkEntrance(adrs, out); }

}  // namespace cash
}  // namespace nexon
