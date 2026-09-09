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

namespace Encryption
{
unsigned int Adler32(char* data, unsigned int length);                      // _ZN10Encryption7Adler32EPcj
void EncodeDressStringWide(const wchar_t* in, wchar_t* out,
                           unsigned long param);                            // _ZN10Encryption21EncodeDressStringWideEPKwPwm
// [b28d-Main·检查点22] 删除原先的 `__attribute__((weak))` 空 stub：
//   ① 二者在 ORIG 里是 **U 引用**（`nm $O` = U，由 NEEDED `libnxencryption.so`
//      在运行期提供；0x0807e190/0x0807e8e0 为 PLT 调用点），我方在 Game/ 源码内
//      发射 W 定义 ⇒ 属「我方 .cpp 发明导出符号」（only_new 2 项）。
//   ② 空 stub 运行期返回 0/不写 out，与厂商实现不一致（行为差异）。
//   现由 `toolchains/nxencryption/libnxencryption.so`（厂商真实二进制，已入仓）
//   参与链接：本 TU 只留声明 ⇒ 我方 df_game_r 变成 U + NEEDED libnxencryption.so，
//   与 ORIG readelf/nm 完全同型。
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
// ByteBuffer
// ===========================================================================

// ORIG 0x081ae9e2：start=write=read=buffer; capacity=len。
ByteBuffer::ByteBuffer(char* buffer, unsigned int len)
    : m_start(buffer), m_writePtr(buffer), m_readPtr(buffer), m_capacity(len)
{
}

// [b28d-D43] ORIG 0x081ae9b8：static 成员 + 按值 sret 返回 —— 体 = 单次
// `call ByteBuffer::C1EPcj(sret, buffer, len)` + `mov %ebx,%eax; ret $0x4`，
// 全字段由 ctor 覆盖（无 this 参与）。调用点（0x081ad43b 等）只压
// sret/buffer/len 三参、无 this ⇒ P88/P245 static 判据。旧重建的
// `ByteBuffer&` + 字段直写产 17 条逐字段 store（ORIG 16 条 sret 构造）。
ByteBuffer ByteBuffer::wrap(char* buffer, unsigned int len)
{
    // [b28d-solo-nexon_cash] N 封顶（帧量化跨版本）：ORIG `sub $0x14` vs 4.4
    // `sub $0xc`。实测 4.4 帧增量恒 align16(局部需求)（char[4]/char[8]/int 均
    // 落 $0x1c），$0x14 不可达（P190 附注②/N95 族）。体 16/16 条全等，仅帧
    // sub/add 立即数差，语义等价。
    return ByteBuffer(buffer, len);
}

// 内联通用形态：cap 校验通过则逐字节大端写入，推进 write 游标；
// 空间不足时静默丢弃（游标不动），ORIG 无返回错误通道。

// ORIG 0x081acba0：strlen → 超 ushort::max 抛 invalid_argument("string length
// too long: must shorter than max(unsigned short)")；length()+len 不超容量时
// 写 ushort 长度前缀（大端）+ strncpy(len 字节，不含 NUL）+ 游标前进 strlen。
ByteBuffer& ByteBuffer::operator<<(const char* value)
{
    // [b27L8] ORIG 0x081acc8d 为 setb（无符号比较）⇒ len 无符号；int 形 4.4 出 setl。
    unsigned int len = strlen(value);
    if (len > (unsigned)std::numeric_limits<unsigned short>::max())
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
// [b28d-D43] N82/N89 大函数（124/124）：STL 实例化 + 槽平移（ORIG 0x081acd1e）。
ByteBuffer& ByteBuffer::operator<<(const wchar_t* value)
{
    // [b28d-solo-nexon_cash] P0/P48：ORIG 0x081acd2d 的 wcslen 结果直接
    // `add %eax,%eax`（无具名 len 局部槽），且 0x081ace63 在 wcsncpy 前二次
    // 调 wcslen ⇒ 原码无 len 局部，字节数就地计算。
    size_t bytes = wcslen(value) * 2;
    if (bytes > (size_t)std::numeric_limits<unsigned short>::max())
        throw std::invalid_argument("string too long");
    if (static_cast<unsigned int>(length() + bytes) <= m_capacity)
    {
        *this << static_cast<unsigned short>(bytes);
        wcsncpy(reinterpret_cast<wchar_t*>(m_writePtr), value, wcslen(value));
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

// ORIG 各 lsEx W body 为手写字节反序拷贝（-O0 不展开循环，逐对 movzbl）：
// 具名 src 指针槽 + m_writePtr[i] = src[n-1-i] 逐条赋值。

// ORIG 0x081ae65e（W）：if(length()+2 <= cap) 写 2B 大端；src 槽 -0xc、value 副本 -0x1c。
ByteBuffer& ByteBuffer::operator<<(short value)
{
    const char* src = reinterpret_cast<const char*>(&value);   // 槽位早于 length() 调用（ORIG lea 先于 call）
    if (static_cast<unsigned int>(length() + 2) <= m_capacity)
    {
        m_writePtr[0] = src[1];
        m_writePtr[1] = src[0];
        m_writePtr += 2;
    }
    return *this;
}

// ORIG 0x081acefa（W）：u16 版本，同形。
ByteBuffer& ByteBuffer::operator<<(unsigned short value)
{
    const char* src = reinterpret_cast<const char*>(&value);   // 槽位早于 length() 调用（ORIG lea 先于 call）
    if (static_cast<unsigned int>(length() + 2) <= m_capacity)
    {
        m_writePtr[0] = src[1];
        m_writePtr[1] = src[0];
        m_writePtr += 2;
    }
    return *this;
}

// ORIG 0x081ae6c4（W）：int 大端 4B 逐条；incoming 地址直取（lea 0xc）。
ByteBuffer& ByteBuffer::operator<<(int value)
{
    const char* src = reinterpret_cast<const char*>(&value);   // 槽位早于 length() 调用（ORIG lea 先于 call）
    if (static_cast<unsigned int>(length() + 4) <= m_capacity)
    {
        m_writePtr[0] = src[3];
        m_writePtr[1] = src[2];
        m_writePtr[2] = src[1];
        m_writePtr[3] = src[0];
        m_writePtr += 4;
    }
    return *this;
}

// ORIG 0x081ae74c（W）：u32 版本，同形。
ByteBuffer& ByteBuffer::operator<<(unsigned int value)
{
    const char* src = reinterpret_cast<const char*>(&value);   // 槽位早于 length() 调用（ORIG lea 先于 call）
    if (static_cast<unsigned int>(length() + 4) <= m_capacity)
    {
        m_writePtr[0] = src[3];
        m_writePtr[1] = src[2];
        m_writePtr[2] = src[1];
        m_writePtr[3] = src[0];
        m_writePtr += 4;
    }
    return *this;
}

// ORIG 0x081ae7d4（W）：u64 大端 8B 逐条；value 副本槽 -0x20/-0x1c。
ByteBuffer& ByteBuffer::operator<<(unsigned long long value)
{
    const char* src = reinterpret_cast<const char*>(&value);   // 槽位早于 length() 调用（ORIG lea 先于 call）
    if (static_cast<unsigned int>(length() + 8) <= m_capacity)
    {
        m_writePtr[0] = src[7];
        m_writePtr[1] = src[6];
        m_writePtr[2] = src[5];
        m_writePtr[3] = src[4];
        m_writePtr[4] = src[3];
        m_writePtr[5] = src[2];
        m_writePtr[6] = src[1];
        m_writePtr[7] = src[0];
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

// ORIG 0x081ae8e4（W）：dst 指针槽 -0x4 + 逐字节 m_readPtr[3-i] 直读（无 tmp）。
ByteBuffer& ByteBuffer::operator>>(int& value)
{
    char* dst = reinterpret_cast<char*>(&value);
    dst[0] = m_readPtr[3];
    dst[1] = m_readPtr[2];
    dst[2] = m_readPtr[1];
    dst[3] = m_readPtr[0];
    m_readPtr += 4;
    return *this;
}

// ORIG 0x081ae94e（W）：u32 版本，同形。
ByteBuffer& ByteBuffer::operator>>(unsigned int& value)
{
    char* dst = reinterpret_cast<char*>(&value);
    dst[0] = m_readPtr[3];
    dst[1] = m_readPtr[2];
    dst[2] = m_readPtr[1];
    dst[3] = m_readPtr[0];
    m_readPtr += 4;
    return *this;
}

// ORIG 0x081acf60：length = m_writePtr - m_start。
// 定义置于全部 <<x/>>x 消费者之后：ORIG 各 W body 内 call length 为宽松帧
// （sub $0x18 + 具名 value 槽 -0xc），同 TU 定义前置会收紧 caller 帧（技巧31）。
unsigned int ByteBuffer::length() const
{
    return static_cast<unsigned int>(m_writePtr - m_start);
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
    return &m_wireMagic;  // this+4
}

unsigned char* NCashMessage::body()
{
    return &m_bodyBytes[0];  // this+7
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

// ORIG 0x081ae578（W）：wireMagic='U'; *(u16*)netLen=htons(bodyLength) 指针直存
// （dst 0x081ae585 add $5 单步 + 0x081ae588 mov %eax,%ebx 先载入，技巧27）。
bool NCashMessage::encode_header()
{
    m_wireMagic = 'U';
    *reinterpret_cast<unsigned short*>(m_netLenB) = htons(m_bodyLength);
    return true;
}

// ORIG 0x081ae5ae：bodyLength 参数直存 +0x02 后调用 encode_header()。
void NCashMessage::encode_header(unsigned int bodyLength)
{
    m_bodyLength = static_cast<unsigned short>(bodyLength);
    encode_header();
}

// ORIG 0x081ae5ce（W）：m_bodyLength=ntohs(*(u16*)netLen)（0x081ae5d7 add $5
// 单步后 movzwl 直读，无 copy 槽）；u16 无符号 cmp $0x200 超 → 清零返 false。
bool NCashMessage::decode_header()
{
    m_bodyLength = ntohs(*reinterpret_cast<unsigned short*>(m_netLenB));
    if (m_bodyLength > 0x200)
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
// 外部计费加密接口。ORIG 中亦为未定义引用：
//   U _ZN10Encryption7Adler32EPcj / U _ZN10Encryption21EncodeDressStringWideEPKwPwm
// 由计费库链接期提供，此处仅声明。
// ===========================================================================

// ===========================================================================
// NCashImpl
// ===========================================================================

// [b28 BM11] P210 定义顺序翻案（推翻 b22「不可复现」判词）：NexonCash ctor 定义
// 前置于 NCashImpl ctor 定义之前 ⇒ 4.4 对 new-expression 只见原型，保守生成
// EH cleanup（delete+Unwind_Resume 岛，ORIG 0x081ae21b-0x081ae232 共 9 insns）。
// ORIG 0x81ae1b0。
NexonCash::NexonCash(const char* serverAdrs, short port, char paramB,
                     short fieldC, short fieldD)
{
    m_impl = new NCashImpl(serverAdrs, port,
                           static_cast<unsigned char>(paramB),
                           static_cast<unsigned short>(fieldC), fieldD);
}

void NCashImpl::_increaseRequestID()                 // ORIG 0x081aea32
{
    ++m_requestID;
}

// NCashImpl ctor 定义置于 NexonCash ctor 调用点之后：ORIG 中两者不同 TU，
// new-expression 只见原型 → 保守生成 EH cleanup（delete+Unwind_Resume 岛，
// ORIG 0x081ae21b-0x081ae232）。定义本体原文见 ORIG 0x081acfdc。
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

// NCashImpl 四个 coupon 定义在文件尾 NexonCash 调用点之后：
// ORIG 编译单元内调用点仅见原型 → 宽松帧 sub $0x18（技巧31 扩展）。

// ORIG 0x081aea46（W）：MSG_WAITALL(0x100) 预读头并解码，随后转 recv_body。
// buf → %ebx（0x081aea58 call data 后直落寄存器，技巧2），getHandle 结果免槽
// 直填第1参（0x081aea66-0x081aea7b），n 槽 -0xc，frame 0x24 + pop ebx。
int NCashImpl::recv_msg()
{
    // [b28d-D43] P0/N96 清绑：register asm("ebx") 删除，改实参内联形。
    int n = static_cast<int>(::recv(m_socket.getHandle(), m_inMsg.data(), 3, 0x100));
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
    // [b28 BM11] N96/P0 清绑：register asm("esi")/asm("ebx") 存量删除，改实参
    // 内联形（P200/P226 方向）：body_length→body→getHandle 的 R2L 求值序与
    // ORIG 装载序同构；跨 body call 存活的 ushort 零扩展值由 4.4 升格
    // callee-saved。
    int n = static_cast<int>(::recv(
        m_socket.getHandle(), m_inMsg.body(),
        m_inMsg.body_length(), 0x100));
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

// ORIG 0x081aec2c（W）：encode(totalLen) → 缓存 length→%esi / data→%ebx
// （0x081aec5c/0x081aec6e 直用寄存器，技巧2），getHandle 结果免槽直填第1参
// （0x081aec79-0x081aec8e，技巧37）；n 槽 -0xc；比较对象是**重调用** length()
// （0x081aeca4-0x081aeca9 cmp -0xc,%eax + setne/test，技巧9 bool 形）；
// 错误分支 strerror 结果 → %ebx 后重 call length 填 printf（0x081aecc4-0x081aecd4）。
int NCashImpl::send_msg(unsigned int totalLen)
{
    m_outMsg.encode_header(totalLen);
    // [b28d-D43] P0/N96 清绑：register asm("esi")/asm("ebx") 删除，改实参内联形。
    int n = static_cast<int>(::send(m_socket.getHandle(), m_outMsg.data(),
                                    m_outMsg.length(), 0x100));
    if (n != static_cast<int>(m_outMsg.length()))   // 0x081aeca9 cmp -0xc,%eax
    {
        // [b28d-D43] P0/N96 清绑：err 改内联实参。
        printf("send error: send bytes: %d, message length: %d, error: %s\n",
               n, static_cast<int>(m_outMsg.length()), strerror(errno));
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
    ByteBuffer out(reinterpret_cast<char*>(m_outMsg.body()), 0x200);
    out << packetId;
    out << flag;
    _increaseRequestID();
    out << m_requestID;
    return out;
}

// ORIG 0x081ad168：从入包 body 读 pid/flag/requestId 三元组。
// [b23S3C] P192：sret 返回入包视图对象（NRVO 直构 caller 槽，无额外拷贝）。
// [b28d-D43] NEAR 帧量化：ORIG sub $0x10 vs 4.4 $0xc（P190附注②；体 39/39 全等）（ORIG 0x081ad168）。
ByteBuffer NCashImpl::get_data_header(unsigned char& a, unsigned char& b,
                                      unsigned int& c)
{
    // [b28d-D43] ORIG 0x081ad168 无 in 的 ctor call（wrap 的 sret 即 in 槽，
    // %ebx=get_data_header sret 槽）：直接初始化形令 4.4 把 in 地址传给 wrap。
    // [b28d-solo-nexon_cash] N 封顶（帧量化跨版本）：ORIG `sub $0x10` vs 4.4
    // `sub $0xc`；实测补 4 字节未用局部跳 $0x1c，$0x10 不可达（P190 附注②）。
    // 指令 39/39 全等，仅帧 sub/add 立即数差，语义等价。
    ByteBuffer in = ByteBuffer::wrap(reinterpret_cast<char*>(m_inMsg.body()),
                                     m_inMsg.body_length());
    in >> a;
    in >> b;
    in >> c;
    return in;
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
// case 0..3 → goto 共用出口标号（技巧23/24）：复现 ORIG 0x081ad2d0-0x081ad2ff
// 二分树 dispatch（cmp$1 je / cmp$1 jg / test je / jmp / cmp$2 je / cmp$3 je）
// + 每 case `nop; jmp merge` 岛；直接 break 会被 4.4 折成 jbe 范围形。
int NCashImpl::handle_coupon_error(int err)
{
    int result = 0x10;
    switch (err)
    {
    case 0: goto Lsilent;
    case 1: goto Lsilent;
    case 2: goto Lsilent;
    case 3: goto Lsilent;
    default:
        printf("[%s] Invalid coupon packet_id: %u\n", __FUNCTION__,
               static_cast<unsigned>(err));
        break;
    }
Lsilent:
    return result;
}

// ORIG 0x081ad316：手组包头（不走 set_data_header！仅自增 requestID 不写入）。
// [b28d-solo-nexon_cash] N 封顶：rc 预清/三元组无预清/声明序已归位后，残余
// 仅为 ORIG 0x081ad430 wrap 实参装载寄存器差（ORIG `lea in,%edx; mov body,%eax`
// vs 4.4 `mov body,%edx; lea in,%eax`，P75/N7 跨版本装载调度，our 107 vs
// orig 106），语义等价。
int NCashImpl::checkEntrance(const char* userName, char& outStatus)
{
    if (!reconnect())
        return 2;

    unsigned char typeByte = static_cast<unsigned char>(m_fieldD);

    ByteBuffer out(reinterpret_cast<char*>(m_outMsg.body()), 0x200);
    out << static_cast<unsigned char>(0x03);
    out << static_cast<unsigned char>(0x01);
    out << typeByte;
    out << userName;
    _increaseRequestID();  // ORIG: 自增但不入包

    // [b28d-solo-nexon_cash] P198①：ORIG 0x081ad3a9 预清 rc 后才 call send_msg。
    int rc = 0;
    rc = send_msg(out.length());
    if (rc != 0)
        return rc;

    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b28d-solo-nexon_cash] P212/P171：ackPid/statusByte 先声明占浅槽
    // （ORIG -0x21/-0x22）、in 后声明占深槽 -0x34；ORIG 无 movb $0 预清。
    unsigned char ackPid;
    unsigned char statusByte;
    ByteBuffer in = ByteBuffer::wrap(reinterpret_cast<char*>(m_inMsg.body()),
                                     m_inMsg.body_length());
    in >> ackPid;
    in >> statusByte;
    outStatus = static_cast<char>(statusByte);
    printf("[%s] CheckEntrace: %d\n", __FUNCTION__,
           static_cast<int>(outStatus));  // ORIG 格式串 typo 忠实保留
    return 0;
}

// ORIG 0x081ad49e：packet_id=0 应答 err==0 后读余额。
// [b28d-D43] N82/N95 帧/槽平移（81/104）（ORIG 0x081ad49e）。
int NCashImpl::getBalance(const char* userId, int& balanceOut)
{
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x00);
    msg << userId;

    // [b28d-solo-nexon_cash] P198①：ORIG 0x081ad4de 预清 rc 后才 call send_msg。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;

    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b28d-solo-nexon_cash] 真语义 bug：ORIG 0x081ad58c 直接复用
    // get_data_header 的 sret 对象（-0x34）继续 `in >> balanceOut`（读 body
    // 第 4 个字段）；旧形重新 wrap 入包 body 从头读（字段错位 + 多 3 调用）。
    // P171/P212：三元组无预清；具名 in 承载 sret（与 msg@-0x1c 同布局）。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    if (errCode != 0)
        return handle_cash_error(errCode);

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

    // [b27L8] ORIG send_msg call 前 movl $0x0,-0xc 预清 ⇒ P198① 两句式。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b27L8] ORIG 无 ackPid/errCode/reqId 预清（movb/movl 缺失）⇒ 未初始化声明。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    // [b23S3C] P192：以 sret 返回值续读 trOut（消本函数 C1/wrap/body/
    // body_length EXTRA×4）。
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    if (errCode != 2)
        return handle_cash_error(errCode);

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
    // [b23S3C] 双臂各直推一次（ORIG +0x68/+0x7d 两块内各 call lsEs、jmp 共享
    // 后继）——具名 streamType 合流形少一条 lsEs（P167 实证 MISSING lsEs）。
    if (subCmd == 0x3c || subCmd == 0x3e)
        msg << subCmd;
    else
        msg << static_cast<short>(m_fieldD);
    msg << productSeq;
    msg << cashType;
    msg << cashAmount;
    msg << param1;
    msg << param2;
    msg << m_paramB;
    msg << static_cast<short>(m_fieldC);

    // [b23S3C] P197③：ORIG rc 槽两段式（length 前先 movl $0,-0xc）。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b23S3C] 三元组无预清（ORIG sret call 前无 movb/movl $0，槽仅由
    // get_data_header 填写）。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    // [b23S3C] P192：sret 返回值续读 trOut（消 EXTRA C1/wrap/body/body_length）。
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    if (errCode != 2)
        return handle_cash_error(errCode);

    in >> trOut.m_type;
    in >> trOut.m_id;
    printf("[%s] PURCHASE OK, db_id: %u, transaction_id: %u\n", __FUNCTION__,
           static_cast<unsigned>(trOut.m_type),
           trOut.m_id);
    return 0;
}

// ORIG 0x081ad9c4：两段附加串位于 m_paramB/m_fieldC 之后。
// [b28d-D43] N82 帧：ORIG sub $0x68 vs 4.4 push %ebx+sub $0x74（ORIG 0x081ad9c4）。
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

    // [b28d-solo-nexon_cash] P198①：ORIG 0x081adae2 预清 rc 后才 call send_msg。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b28d-solo-nexon_cash] 真语义 bug（同 getBalance）：ORIG 0x081adb81 复用
    // get_data_header 的 sret 对象（-0x34）继续读 trOut.m_type/m_id；旧形重新
    // wrap 入包 body 从头读（字段错位 + 多 C1/body/body_length/wrap 4 调用）。
    // 三元组无预清（P171），具名 in 承载 sret（msg@-0x1c 同布局）。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    if (errCode != 2)
        return handle_cash_error(errCode);

    in >> trOut.m_type;
    in >> trOut.m_id;
    printf("[%s] PURCHASE OK, db_id: %u, transaction_id: %u\n", __FUNCTION__,
           static_cast<unsigned>(trOut.m_type),
           trOut.m_id);
    return 0;
}

// ORIG 0x081adbd4：err!=3 时 handle_cash_error 结果被丢弃仍打印成功行返 0。
// [b28d-D43] N82/N95 同 cancel（帧/槽平移）（ORIG 0x081adbd4）。
int NCashImpl::confirm(unsigned char trFlag, unsigned int trId)
{
    if (trId == 0)
        return 0;
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x03);
    msg << trFlag;
    msg << trId;

    // [b28d-solo-nexon_cash] P198①：ORIG 0x081adc1d `movl $0x0,-0xc` 预清
    // rc 后才 call send_msg ⇒ 必须两句式；合并初始化式丢预清行。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b28d-solo-nexon_cash] P171：ORIG get_data_header 前无 movb/movl $0
    // 预清（槽仅由被调填写）⇒ 三元组声明不得带初始化。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    // [b28d-solo-nexon_cash] P212：具名 in 承载 get_data_header 的 sret，
    // 令 4.4 槽位与 ORIG 全等（msg@-0x1c / sret@-0x34）。
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    (void)in;
    // [b28d-solo-nexon_cash] 真语义 bug：ORIG 0x081adce3..0x081adcf0 为
    // errCode!=3 → call handle_cash_error; jmp <leave>（跳过 printf 并返回
    // 其错误码）；旧形忽略返回值后继续打印成功行 ⇒ 改为 return。
    if (errCode != 3)
        return handle_cash_error(errCode);
    printf("[%s] E_PURCHASE_CONFIRM\n", __FUNCTION__);
    return 0;
}

// ORIG 0x081add04：与 confirm 同构，packet_id=4、__FUNCTION__="cancel"。
// [b28d-D43] N82/N95 帧/槽平移：set_data_header 返回槽 ORIG -0x1c vs 4.4 -0x2c（ORIG 0x081add04）。
int NCashImpl::cancel(unsigned char trFlag, unsigned int trId)
{
    if (trId == 0)
        return 0;
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x04);
    msg << trFlag;
    msg << trId;

    // [b28d-solo-nexon_cash] P198①：同 confirm（ORIG 0x081add86 预清）。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    unsigned char ackPid, errCode;
    unsigned int reqId;
    // [b28d-solo-nexon_cash] P212：具名 in 承载 get_data_header 的 sret，令 4.4
    // 槽位与 ORIG 全等（msg@-0x1c / sret@-0x34）⇒ AE。
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    (void)in;
    // [b28d-solo-nexon_cash] 真语义 bug：同 confirm（ORIG 0x081ade11 后
    // 0x081ade16 jmp <leave>，errCode!=3 时不打印）。
    if (errCode != 3)
        return handle_cash_error(errCode);
    printf("[%s] E_PURCHASE_CONFIRM\n", __FUNCTION__);  // 共用格式串
    return 0;
}

// ORIG 0x081ade34：无 reconnect 前置；组"%s|%i|%i|%i|%s|%s|%s|%s|%i" 明文 →
// mbstowcs → Adler32(宽串字节) → EncodeDressStringWide(key=0x1b1bde64) →
// wcstombs 回 ASCII 密文入包 + 协议键 qword + adler qword。
// [b28d-solo-nexon_cash] 本波修真 bug 3 处：①4 个函数级 static 缓冲（ORIG 符号
// _ZZ...E4data/E9wide_data/E8enc_data/E9send_data，尺寸 0x3e8/0xfa0/0xfa0/0x3e8）
// 替代 struct+getter（旧形发明符号 + memset 长度 0x1f40 错）；②格式串 9 token
// （iv/code 为 %i，旧形漏传 pD/code 且 iv 当 char*）；③adler 为 unsigned long long
// （8 字节槽 0x081adfdd）。帧/槽已全对齐；N 封顶残余 = 4.1 局部物化/EH 段差
// （P0/N82 族，our 200 vs orig 196，多数为 .o 重定位伪差）。
int NCashImpl::chargeCash(char* sessionKey, unsigned char h1, unsigned char h2,
                          int iv, char* pA, char* pB, char* pC, char* pD,
                          int code, NXTransaction& trOut)
{
    // [b28d-solo-nexon_cash] ORIG 4 个函数级 static 缓冲（nm 实证）：
    // _ZZ...chargeCash...E4data@0x09405640(0x3e8) / E9wide_data@0x9405a40(0xfa0)
    // / E8enc_data@0x94069e0(0xfa0) / E9send_data@0x9407980(0x3e8)，均 b 段。
    // 旧 struct+ccBufs() getter 造发明符号且访问经局部引用缓存；ORIG 每次直压
    // 静态地址立即数 ⇒ 必须函数内 static 声明。
    // [b28d-solo-nexon_cash] nm 尺寸实证：data/send_data 0x3e8、wide_data/enc_data
    // 0xfa0（wchar_t=4B ⇒ wchar_t[1000]；旧 [2000] 使 memset 长度 0x1f40 与 ORIG
    // 的 0xfa0 不符且符号尺寸翻倍）。
    static char data[1000];
    static wchar_t wide_data[1000];
    static wchar_t enc_data[1000];
    static char send_data[1000];

    memset(data, 0, sizeof(data));
    memset(wide_data, 0, sizeof(wide_data));
    memset(enc_data, 0, sizeof(enc_data));
    memset(send_data, 0, sizeof(send_data));

    ByteBuffer msg = set_data_header(0x02, 0x01);

    // [b28d-solo-nexon_cash] ORIG 0x081ade75..0x081ade83：set_data_header 后
    // 立即 3 条 movl（-0x2c=0xe8c3f80b 未消费槽 + -0x28/-0x24=qword key
    // 0x00000000F3D8266F）⇒ 原码为具名局部（P232 dead 标量照样占槽）。
    int deadField = static_cast<int>(0xe8c3f80bu);
    unsigned long long frameKey = 0x00000000F3D8266FULL;
    (void)deadField;

    // [b28d-solo-nexon_cash] 真语义 bug：ORIG 0x081adefa..0x081adf2a 在栈上
    // 构造 27 字节 fmt = "%s|%i|%i|%i|%s|%s|%s|%s|%i"（9 token），并在
    // 0x081adf3c..0x081adf6a 压 9 个变参 sessionKey,h1,h2,iv,pA,pB,pC,pD,code
    // （0x18(%ebp)=iv 与 0x2c(%ebp)=code 均参与）⇒ 旧形把 iv 当 char* cast、
    // 少传 pD/code 且 fmt 仅 8 token（输出串错位）。
    char fmt[] = "%s|%i|%i|%i|%s|%s|%s|%s|%i";
    snprintf(data, sizeof(data), fmt,
             sessionKey,
             static_cast<int>(h1),
             static_cast<int>(h2),
             iv,
             pA, pB, pC, pD,
             code);

    size_t len = strlen(data) + 1;
    mbstowcs(wide_data, data, len);

    // [b28d-solo-nexon_cash] P0/P211：ORIG 0x081adfc4 的 wcslen 结果直接
    // `lea (%eax,%eax,1),%edx`（无 wlen 具名槽）；Adler32 返回值零扩展后存
    // 8 字节槽（0x081adfdd `mov $0x0,%edx` + -0x18/-0x14）⇒ 原码是
    // unsigned long long adler。
    unsigned long long adler = Encryption::Adler32(
        reinterpret_cast<char*>(wide_data),
        static_cast<unsigned int>(wcslen(wide_data) * 2));

    Encryption::EncodeDressStringWide(wide_data, enc_data,
                                      0x1b1bde64u);

    size_t enclenChars = wcslen(enc_data);
    wcstombs(send_data, enc_data, enclenChars);

    msg << send_data;
    msg << frameKey;                      // [-0x28/-0x24] 帧 key
    msg << adler;

    // [b28d-solo-nexon_cash] P198①：ORIG 预清 rc 后才 call send_msg。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b28d-solo-nexon_cash] P171：三元组无预清。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    get_data_header(ackPid, errCode, reqId);

    if (errCode != 2)
    {
        // [b28d-solo-nexon_cash] §4.1/P154：删发明符号
        // _ZN5nexon4cash12_GLOBAL__N_114traceRecvErrorEiPKc（ORIG nm 无此符号）。
        // ORIG 0x081ae105..0x081ae145 为就地具名 cMyTrace 两句式（ctor + op()）。
        cMyTrace trace(__FUNCTION__, 0x303, 0);
        trace("NCashImpl::chargeCash Recv Error!! packet_id:%d, data:%s",
              static_cast<int>(errCode), data);
        return static_cast<int>(errCode);
    }

    ByteBuffer in = ByteBuffer::wrap(reinterpret_cast<char*>(m_inMsg.body()),
                                     m_inMsg.body_length());
    in >> trOut.m_type;
    in >> trOut.m_id;
    printf("[%s] CHARGE_CASH OK, db_id: %u, transaction_id: %u\n", __FUNCTION__,
           static_cast<unsigned>(trOut.m_type), trOut.m_id);
    return 0;
}

// ===========================================================================
// NexonCash（薄包装）
// ===========================================================================

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

bool NCashImpl::checkCoupon()     { return false; }   // ORIG 0x081aea0a xor eax,eax
bool NCashImpl::useCoupon()       { return false; }   // ORIG 0x081aea14
bool NCashImpl::checkCafeCoupon() { return false; }   // ORIG 0x081aea1e
bool NCashImpl::useCafeCoupon()   { return false; }   // ORIG 0x081aea28

int NexonCash::charge(char* data, unsigned char a, unsigned char b,
                      int encLen, char* encData, char* sendBuf, char* wideBuf,
                      char* more, int count, NXTransaction& tx)
{ return m_impl->chargeCash(data, a, b, encLen, encData, sendBuf, wideBuf,
                            more, count, tx); }

int NexonCash::checkEntrance(const char* adrs, char& out)
{ return m_impl->checkEntrance(adrs, out); }

}  // namespace cash
}  // namespace nexon
