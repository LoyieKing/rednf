// ============================================================================
// nexon::cash 域实现 —— socket / 事务 / NexonCash 薄包装。
//
// [GI 拆分] 原单 TU 已按 ORIG 的编译单元边界拆出（P327）：
//   · ByteBuffer.cpp  ← nexon::cash::ByteBuffer
//       （GI 键名 _ZN5nexon4cash10ByteBufferlsEPKc，ORIG @0x081acedd）
//   · NXCash.cpp      ← nexon::cash::NCashImpl
//       （GI 键名 _ZN5nexon4cash9NCashImplC2EPKcshts，ORIG @0x081ae511）
//   本文件保留 TCPSocket / UDPSocket / Transaction / UserBalance /
//   TransactionManager / NexonCash。ORIG 侧这几组亦分属独立 TU
//   （TCPSocket.cpp GI=MAX_SEND_RETRY / UserBalance.cpp GI=expire /
//   TransactionManager.cpp GI=tx_id_ 等），其 GI 键名待后续批次拆出。
//
// 逐符号按 df_game_r ORIG 反汇编转写；地址见各函数注释。
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
// [T2-GI · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit（ORIG 实有符号），
// 使本 TU 发射 _GLOBAL__I_（键名见 namespace 内 UserBalance::expire 前移注释）。
#include <iostream>
#include <string.h>
#include <wchar.h>
#include <limits>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

namespace nexon {
namespace cash {

// [T1-GI · P327] UserBalance::expire 前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN5nexon4cash11UserBalance6expireEv`（ORIG 0x081b163a，ORIG CU
// UserBalance.cpp）。仅移函数定义，零语义/布局改动。本 TU 为 TCPSocket / Transaction /
// UserBalance 合并桶，一 TU 一 GI ⇒ 另两条（tx_id_ / MAX_SEND_RETRY）需拆 TU 才可消。
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
// [b28d-D43] P168a/N96：ORIG `cmp $0x64; setg %al; test %al,%al; je` 寄存器 bool vs 4.4 直跳 `jle`（清绑后 P0 合规）+ §6.1 尾对齐 nop（ORIG 0x081af6ae）。
int TCPSocket::send(char* buffer, unsigned int length)
{
    if (buffer == 0 || length == 0)
        return -1;

    int n = static_cast<int>(::write(m_socket, buffer, length));  // 0x081af6e0 write@plt
    int savedErrno = errno;                                       // 0x081af6e8-0x081af6f1

    // ORIG 0x081af6f6 `jg +0xb1`：n>0 块位于函数尾 leave 前岛（技巧23/24 goto 钉块序）
    if (n > 0)
        goto positive;
    if (savedErrno == EAGAIN || savedErrno == EINTR || savedErrno == EAGAIN)
    {
        ++m_sendRetryCount;                                       // 0x081af70a-0x081af716
        // [b28d-D43] P0/N96 清绑：原 register bool over asm("al") 删除（全域禁
        // 绑定）。ORIG 0x081af722 为 setg %al; test %al,%al; je（无槽往返）⇒ 按
        // P0「无槽 = 无具名局部」写裸条件。
        // [b28d-solo-nexon_cash] P225：ORIG `cmp $0x64; setg %al; test %al,%al;
        // je` 物化形 ⇒ 裸 if 在 4.4 折 `jle` 直跳；改 while+break 族
        // （body 末 return）即复现 setg 物化。
        while (m_sendRetryCount > static_cast<int>(TCPSocket::MAX_SEND_RETRY))
        {
            m_sendRetryCount = 0;                                 // 0x081af72c
            return -100;                                          // 0x081af733
        }
        return 0;                                                 // 0x081af73a
    }
    if (savedErrno == 0)                                          // 0x081af741
        return 0;                                                 // 0x081af747
    m_sendRetryCount = 0;                                         // 0x081af74e
    return -1;                                                    // 0x081af758
positive:
    m_sendRetryCount = 0;                                         // 0x081af75f-0x081af762
    return n;                                                     // 0x081af769
}

// ORIG 0x081af76e：
//   if (!buffer || !length) return -1;
//   n = read(...); if (n < 0) { (errno==EAGAIN||errno==EINTR||errno==EAGAIN||
//       errno==0) → return 0（0x081af7b5/7c1/7cd je 共岛 +0x6c；test jne→+0x80）；
//       其余 → return n（原负值，0x081af7ee mov -0xc,%eax） }
//   else if (n == 0) return -1;   // 对端关闭
//   return n;
int TCPSocket::recv(char* buffer, unsigned int length)
{
    if (buffer == 0 || length == 0)
        return -1;

    int n = static_cast<int>(::read(m_socket, buffer, length));
    if (n < 0)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN || errno == 0)
            return 0;   // 0x081af7da 共岛 mov $0
    }
    else
    {
        if (n == 0)
            return -1;   // ORIG 0x081af7e7 n>=0 岛（jns +0x73）
    }
    return n;            // ORIG 0x081af7ee 三路共享尾块（6a/77 jne 同标）
}

int TCPSocket::getHandle() const
{
    return m_socket;
}

// ORIG 0x081af7fe (sz 0xd)：mov 0x8(%ebp),%eax; mov (%eax),%eax;
// cmp $0xffffffff,%eax; pop; ret —— 尾置 return 的 if 被折叠成裸 cmp（技巧6变体）。
void TCPSocket::shutdown(int how)
{
    // 忠实转写：ORIG 本函数未发起 shutdown 系统调用（内联裁剪后的残体）。
    if (m_socket == INVALID_SOCKET)
        return;
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
    memset(&m_peerAdrs, 0, 4);   // ORIG 0x081af832-0x081af84b：真实 memset@plt 调用
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
// flag 两语句式：死预置 0（0x081af8c8 movl $0,-0xc）+ 显式 if/else 各支内存存储
// （0x081af8d5 movl $1 / 0x081af8de movl $0），三元写法会被折成寄存器选择形。
bool TCPSocket::setOptReuseAdrs(bool on)
{
    int flag = 0;
    if (on)
        flag = 1;
    else
        flag = 0;
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
// [b28d-D43] P75 实参装载时刻：ORIG 提前 `mov -0xc(%ebp),%ecx`（跨两条 this 装载）vs 4.4 使用点才读（ORIG 0x081af9ea）。
bool TCPSocket::connect(const char* host, unsigned short port)
{
    struct sockaddr_in adrs;
    memset(&adrs, 0, sizeof(adrs));
    adrs.sin_family = 2;
    adrs.sin_addr.s_addr = inet_addr(host);
    adrs.sin_port = htons(port);

    // [b28d-solo-nexon_cash] P218/P290：len 声明为 int（非 socklen_t/unsigned）
    // 触发 4.4 提前装载 `mov -0xc(%ebp),%ecx`（ORIG 0x081afa3d 序），DIFF→AE。
    int len = sizeof(adrs);
    if (::connect(m_socket, reinterpret_cast<struct sockaddr*>(&adrs), len) < 0)
        return false;

    memcpy(&m_peerAdrs, &adrs.sin_addr, 4);
    m_peerPort = adrs.sin_port;
    return true;
}

// ORIG 0x081afa98 (sz 0x230)：非阻塞连接 + select 等待 + SO_ERROR 校验。
// [b28d-D43] P75 实参装载时刻 + 帧差（155/154）（ORIG 0x081afa98）。
bool TCPSocket::connect_nonb(const char* host, unsigned short port,
                             timeval timeout)
{
    // [b23S3C] P48：ORIG connect 错误链对 errno 每判重取 __errno_location()
    // （4 次独立 call，&& 短路）；成功路径单一共享尾（result==0 跳过 select
    // 块直达 memcpy）——旧形 result==0 早返回造成 EXTRA memcpy+1。
    struct sockaddr_in adrs;
    memset(&adrs, 0, sizeof(adrs));
    adrs.sin_family = 2;
    adrs.sin_addr.s_addr = inet_addr(host);
    adrs.sin_port = htons(port);

    socklen_t len = sizeof(adrs);

    setOptNonBlock();

    int result;
    int local_24 = 0;   // [b23S3C] P31：ORIG +0x6a `movl $0,-0x24` dead 槽
                        // （Ghidra local_24 无消费），缺它使 adrs/len/result
                        // 全体高 4（-0x38 vs -0x3c）。
    result = ::connect(m_socket,
                       reinterpret_cast<struct sockaddr*>(&adrs), len);
    // [b28d-solo-nexon_cash] N 封顶：ORIG 短路首项 `mov; shr $0x1f; test %al,%al;
    // je`（4.1 符号位物化）；4.4 对 result<0 / (unsigned)result>>31 / (result>>31)
    // 均折 `cmpl $0; jns`（N76 族，本次再证）。另 ORIG select 后 if 链的
    // mov$1/jmp/test 汇聚物化（N3 族）与 getsockopt 实参右到左求值序（P75）同为
    // 跨版本结构性差异；已修 FD_ZERO 显式循环/FD_SET unsigned/soError 无预清与
    // 槽序，语义等价。
    if (result < 0 && errno != 0x73 /*EINPROGRESS*/ &&
        errno != EINTR && errno != EAGAIN && errno != EAGAIN)
        return false;
    if (result != 0)
    {
        fd_set writeSet;
        // [b28d-solo-nexon_cash] P212/P30：ORIG 0x081afb74 为显式位循环
        // （i=0..0x1f, pset->fds_bits[i]=0 的 setbe/jne 回边），与 pollReadEvent
        // 已 AE 的形一致；FD_ZERO 宏在 4.4 折 rep stos（N78 反形）。
        unsigned int i;
        fd_set* pset;
        pset = &writeSet;
        for (i = 0; i <= 0x1f; ++i)
            pset->fds_bits[i] = 0;
        // [b28d-solo-nexon_cash] ORIG 0x081afbb0 `shr $0x5` 无符号移位 ⇒ fd
        // 实参须 unsigned（否则 4.4 出 sar+cmovs，N97）。
        FD_SET(static_cast<unsigned int>(m_socket), &writeSet);

        result = ::select(m_socket + 1, 0, &writeSet, 0, &timeout);
        if (result < 0 && errno != EINTR)
            return false;
        if (result <= 0)
            return false;

        // [b28d-solo-nexon_cash] P171：ORIG 0x081afc6a 无 soError 预清
        // （仅 movl $0x4,-0x40 写 optLen），槽序 soError@-0x44/optLen@-0x40。
        // [b28d-solo-nexon_cash] P212：块内 4.4 先声明占浅槽 ⇒ optLen 先声明
        // 得 -0x40、soError 后声明得 -0x44（=ORIG 槽序）。
        socklen_t optLen = 4;
        int soError;
        if (::getsockopt(m_socket, 1 /*SOL_SOCKET*/, 4 /*SO_ERROR*/,
                         &soError, &optLen) < 0)
            return false;
        if (soError != 0)
            return false;
    }

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
    socklen_t len = 16;  // sizeof(sockaddr_in)
    peer.m_socket = ::accept(m_socket, reinterpret_cast<struct sockaddr*>(
                                            &peer.m_sinFamily), &len);
    if (peer.m_socket < 0 || peer.m_socket == INVALID_SOCKET)
        return false;

    // ORIG 0x081aff78：memcpy(dst=&peer.m_peerAdrs(+0x14), src=peer+0x08 单步)。
    memcpy(&peer.m_peerAdrs, &peer.m_sinAddr, 4);
    // ORIG 0x081aff87：movzwl 0x6(%eax) 单步 disp 直读 sin_port（peer+0x06），
    // 0x081aff91 mov %dx,0x18(%eax) 存 m_peerPort。
    peer.m_peerPort = peer.m_sinPort;
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

// ORIG 0x081affd0：具名 ret 槽（-0xc cmpl/jns）+ 两条 dead 数组写
// （0x081affe4 movl $0,-0x10 / 0x081affeB movl $4,-0x14，技巧31 dead 槽撑帧）。
bool TCPSocket::setOptResizeSendBuf(int size)
{
    if (size <= 0)
        return false;
    int dead_vals[2];
    dead_vals[1] = 0;   // ORIG 0x081affe4
    dead_vals[0] = 4;   // ORIG 0x081affeb
    int ret = ::setsockopt(m_socket, 1, 7 /*SO_SNDBUF*/, &size, sizeof(size));
    if (ret < 0)
        return false;
    return true;
}

// ORIG 0x081b0036：与 SendBuf 同形（dead_vals @0x081b004a/0x081b0051）。
bool TCPSocket::setOptResizeRecvBuf(int size)
{
    if (size <= 0)
        return false;
    int dead_vals[2];
    dead_vals[1] = 0;
    dead_vals[0] = 4;
    int ret = ::setsockopt(m_socket, 1, 8 /*SO_RCVBUF*/, &size, sizeof(size));
    if (ret < 0)
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
// [b28d-D43] AE: FD_ZERO→显式循环（pset/i 全头部声明复现 -0x1c/-0x20 槽序）+FD_SET unsigned cast 复现 shr（ORIG 0x081afcc8）。
bool TCPSocket::pollReadEvent() const
{
    fd_set fds;
    struct timeval timeout;
    int ret;
    unsigned int i;
    fd_set* pset;
    pset = &fds;
    for (i = 0; i <= 0x1f; ++i)
        pset->fds_bits[i] = 0;
    FD_SET(static_cast<unsigned int>(m_socket), &fds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    ret = ::select(2, &fds, 0, 0, &timeout);
    if (ret == 0)
        return false;
    return true;
}

// [b28d-D43] AE: 同 pollReadEvent（ORIG 0x081afd94）。
bool TCPSocket::pollWriteEvent() const
{
    fd_set fds;
    struct timeval timeout;
    int ret;
    unsigned int i;
    fd_set* pset;
    pset = &fds;
    for (i = 0; i <= 0x1f; ++i)
        pset->fds_bits[i] = 0;
    FD_SET(static_cast<unsigned int>(m_socket), &fds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    ret = ::select(2, 0, &fds, 0, &timeout);
    if (ret == 0)
        return false;
    return true;
}

// [b28d-D43] AE: 同 pollReadEvent（ORIG 0x081afe60）。
bool TCPSocket::pollErrorEvent() const
{
    fd_set fds;
    struct timeval timeout;
    int ret;
    unsigned int i;
    fd_set* pset;
    pset = &fds;
    for (i = 0; i <= 0x1f; ++i)
        pset->fds_bits[i] = 0;
    FD_SET(static_cast<unsigned int>(m_socket), &fds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    ret = ::select(2, 0, 0, &fds, &timeout);
    if (ret == 0)
        return false;
    return true;
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
// [b28d-D43] AE: m_adrs 具名 sockaddr_in 视图直存 + htons(m_port)（ORIG 0x081aedc6）。
bool UDPSocket::bind(const char* host, unsigned short port)
{
    m_port = port;
    memset(m_adrs, 0, sizeof(m_adrs));
    // [b28d-D43] 具名视图直存（ORIG 0x081aedfc/0x081aee10/0x081aee2b 三条
    // this+disp 单条写；旧 memcpy+局部 addr/netPort 多 11 条）。htons 实参按
    // ORIG 读 m_port（0x081aee19 双段 movzwl），非形参 port。
    m_sin.m_sinFamily = 2;                  // AF_INET
    m_sin.m_sinAddr = inet_addr(host);
    m_sin.m_sinPort = htons(m_port);

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
    // [b28d-D43] 同上具名视图直存。
    m_sin.m_sinFamily = 2;
    m_sin.m_sinAddr = htonl(0);
    m_sin.m_sinPort = htons(m_port);

    if (::bind(m_socket,
               reinterpret_cast<struct sockaddr*>(m_adrs), 16) != 0)
    {
        int e = errno;
        // [b28d-D43] P251：ORIG 0x081aef15 段为 `mov -0xc(%ebp),%eax; cmp
        // $0x62,%eax; je …; cmp $0x63,%eax; je …; test %eax,%eax; je …`
        // （值载寄存器后多路比较链，非内存直比）⇒ 源码是 switch(e)。
        switch (e)
        {
        case 98: /*EADDRINUSE*/
            printf("Port %d for receiving UDP is in use\n",
                   static_cast<int>(port));
            break;
        case 99: /*EACCES*/
            puts("Cannot assign requested address.");
            break;
        case 0:
            break;
        default:
            printf("Could not bind UDP receive port. Error= %d , strerror=%s\n",
                   e, strerror(e));
            break;
        }
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
//   n<0 且 errno∈{EWOULDBLOCK,EAGAIN,EINTR}→0；否则 n。
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
        // [b28 BM11] ORIG 立即数序 = 11,11,4（EWOULDBLOCK==EAGAIN 双写 + EINTR 尾，
        // P171 原码链序照抄）；旧形 11,4,11 与 ORIG cmp 立即数错位。
        if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR)
            return 0;
    }
    return n;
}

// ORIG 0x081af0de：
//   if (!buffer || length<=0) return -1;
//   addrlen=16；n=recvfrom(m_socket,buffer,length,0,(sockaddr*)&m_from,&addrlen)
//   —— ORIG 直接以 this+0x18 作为源地址存储（即成员 m_from）。
//   n<0: (errno==EAGAIN||errno==EINTR||errno==EAGAIN)→ return 0（0x081af162 共岛），
//        否则 → return -1（0x081af169）；n>=0 原样返回（0x081af170）。
// [b28d-D43] P75/P52：ORIG 提前 `lea -0x10(%ebp),%ebx`（callee-saved 独占）vs 4.4 推迟到实参装载点（ORIG 0x081af0de）。
int UDPSocket::recv(char* buffer, int length)
{
    if (buffer == 0 || length <= 0)
        return -1;

    // [b28d-solo-nexon_cash] P218/P290：addrlen 由 socklen_t 改 int + 显式
    // reinterpret_cast<socklen_t*> 触发 4.4 提前 `lea -0x10(%ebp),%ebx`
    // （ORIG 0x081af0ff 序），P75 装载时刻差消除，DIFF→AE。
    int addrlen = 16;
    int n = static_cast<int>(::recvfrom(
        m_socket, buffer, length, 0,
        reinterpret_cast<struct sockaddr*>(m_from),
        reinterpret_cast<socklen_t*>(&addrlen)));
    if (n < 0)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN)
            return 0;
        else
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
// 结果为具名 ret 槽（0x081af250 mov %eax,-0xc; cmpl; jns），frame 0x38。
bool UDPSocket::setOptResizeSendBuf(int size)
{
    if (size <= 0)
        return false;
    int ret = ::setsockopt(m_socket, 1, 7, &size, sizeof(size));
    if (ret < 0)
        return false;
    return true;
}

bool UDPSocket::setOptResizeRecvBuf(int size)
{
    if (size <= 0)
        return false;
    int ret = ::setsockopt(m_socket, 1, 8, &size, sizeof(size));
    if (ret < 0)
        return false;
    return true;
}

// ORIG 0x081af268：read 集 select(nfds=2)，超时 {0,5000}微秒，ret!=0。
// [b28d-D43] AE: 同 pollReadEvent（ORIG 0x081af268）。
bool UDPSocket::pollReadEvent() const
{
    fd_set fds;
    struct timeval timeout;
    int ret;
    unsigned int i;
    fd_set* pset;
    pset = &fds;
    for (i = 0; i <= 0x1f; ++i)
        pset->fds_bits[i] = 0;
    FD_SET(static_cast<unsigned int>(m_socket), &fds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0x1388;
    ret = ::select(2, &fds, 0, 0, &timeout);
    if (ret == 0)
        return false;
    return true;
}

// ORIG 0x081af334：write 集超时 {0,0}。
// [b28d-D43] AE: 同 pollReadEvent（ORIG 0x081af334）。
bool UDPSocket::pollWriteEvent() const
{
    fd_set fds;
    struct timeval timeout;
    int ret;
    unsigned int i;
    fd_set* pset;
    pset = &fds;
    for (i = 0; i <= 0x1f; ++i)
        pset->fds_bits[i] = 0;
    FD_SET(static_cast<unsigned int>(m_socket), &fds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    ret = ::select(2, 0, &fds, 0, &timeout);
    if (ret == 0)
        return false;
    return true;
}

// ORIG 0x081af400：except 集，超时 {0,0}。
// [b28d-D43] AE: 同 pollReadEvent（ORIG 0x081af400）。
bool UDPSocket::pollErrorEvent() const
{
    fd_set fds;
    struct timeval timeout;
    int ret;
    unsigned int i;
    fd_set* pset;
    pset = &fds;
    for (i = 0; i <= 0x1f; ++i)
        pset->fds_bits[i] = 0;
    FD_SET(static_cast<unsigned int>(m_socket), &fds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    ret = ::select(2, 0, 0, &fds, &timeout);
    if (ret == 0)
        return false;
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
// [b28d-D43] P23/P75 求值序：ORIG erase 两实参 end,end,begin,remove 序 vs 4.4 remove 提前（ORIG 0x081b1530）。
void UserBalance::removeTransaction(Transaction* tx)
{
    // [b28d-solo-nexon_cash] P23/P75：ORIG 0x081b1530 求值序 = 外层 erase 的
    // end()(-0x18) → 内层 remove 的 end()(-0x10)/begin()(-0xc) → remove →
    // erase；具名 last 两句式令 4.4 求值序 end/begin/remove/end 反形 ⇒ 必须
    // 写单表达式嵌套形。
    m_transactions.erase(
        std::remove(m_transactions.begin(), m_transactions.end(), tx),
        m_transactions.end());
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
// 正向 if 包裹（0x081b0263 je 直达 leave +0x51，无尾 jmp/nop 岛）。
void TransactionManager::cancel(unsigned int id)
{
    Transaction* tx = getTransaction(id);
    if (tx != 0)
    {
        tx->cancel();
        tx->getUser()->removeTransaction(tx);
        delete tx;
    }
}

// ORIG 0x081b0298：同 cancel 但不退款（tx->confirm() 仅自移除）。正向 if 同形。
void TransactionManager::confirm(unsigned int id)
{
    Transaction* tx = getTransaction(id);
    if (tx != 0)
    {
        tx->confirm();
        tx->getUser()->removeTransaction(tx);
        delete tx;
    }
}

// ORIG 0x081b04cc：前缀 ++ 后返回新值。
unsigned int TransactionManager::getNextTransactionID()
{
    return ++TransactionManager::tx_id_;
}

// ===========================================================================
// NexonCash（薄包装）
//
// [GI 拆分] NCashImpl 的定义已迁至 NXCash.cpp；本 TU 对 new NCashImpl 只见
// 声明 ⇒ 4.4 保守生成 new-expression EH cleanup（delete + _Unwind_Resume 岛，
// ORIG 0x081ae21b-0x081ae232 共 9 insns），与拆分前同形。
// ===========================================================================

// ORIG 0x81ae1b0。
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

bool NexonCash::checkCoupon()     { return m_impl->checkCoupon(); }
bool NexonCash::useCoupon()       { return m_impl->useCoupon(); }
bool NexonCash::checkCafeCoupon() { return m_impl->checkCafeCoupon(); }
bool NexonCash::useCafeCoupon()   { return m_impl->useCafeCoupon(); }

int NexonCash::charge(char* data, unsigned char a, unsigned char b,
                      int encLen, char* encData, char* sendBuf, char* wideBuf,
                      char* more, int count, NXTransaction& tx)
{ return m_impl->chargeCash(data, a, b, encLen, encData, sendBuf, wideBuf,
                            more, count, tx); }

int NexonCash::checkEntrance(const char* adrs, char& out)
{ return m_impl->checkEntrance(adrs, out); }

}  // namespace cash
}  // namespace nexon
