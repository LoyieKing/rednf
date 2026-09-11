// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 nexon_cash/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "nexon_cash/expire.inc"


#include "nexon_cash/TCPSocket.inc"

#include "nexon_cash/TCPSocket_1.inc"


#include "nexon_cash/dtor_TCPSocket.inc"


#include "nexon_cash/open.inc"


#include "nexon_cash/bind.inc"
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
    // [P6AuditOldN.P6S2 2026-09-11 · §6.5 串审计] ORIG .rodata @0x08b6cad0 逐字节
    //   = "succeeded in binding TCP socket port #%d\n"。
    printf("succeeded in binding TCP socket port #%d\n", static_cast<int>(port));
    return true;
}

#include "nexon_cash/listen.inc"


#include "nexon_cash/send.inc"


#include "nexon_cash/recv.inc"


#include "nexon_cash/getHandle.inc"


#include "nexon_cash/shutdown.inc"


#include "nexon_cash/close.inc"


#include "nexon_cash/setOptNonBlock.inc"


#include "nexon_cash/setOptReuseAdrs.inc"


#include "nexon_cash/setOptLinger.inc"


#include "nexon_cash/setOptNagle.inc"
{
    int flag = on ? 1 : 0;
    int ret = ::setsockopt(m_socket, 6, 1, &flag, sizeof(flag));
    return ret == 0;
}

#include "nexon_cash/connect.inc"
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

#include "nexon_cash/connect_nonb.inc"
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
        // [S4NexonProto] 负结论：ORIG 0x081afc03 段的「bool 物化」形
        // （`mov $1,%eax; jmp; mov $0,%eax; test %al,%al; je`）在 4.4 下不可造——
        // 已试 3 形：`&&` 裸形 / `static_cast<bool>(...)` / `static_cast<int>(...) != 0`，
        // 三者均产短路形（`jns/je` 直跳 + 内联 return 块）。第一段（5 条件）
        // 同理（`shr $0x1f` 符号位物化 vs 4.4 `cmpl $0; jns`）。
        // 另：ORIG 0x081afada `mov -0x2c(%ebp),%ecx`（len）在 `lea -0x3c(%ebp)`
        // 之前 = 4.1 的纯右→左实参求值序；4.4 恒为「arg2(lea)/arg1/arg3」固定序
        // （探针 /tmp/probe/argo.cpp A1..A3 三形同序；getsockopt 五实参同理
        // A4/A5：sock→&optLen→&soError）。属前端行为，源码侧不可造。
        if (result < 0 && errno != EINTR)
            return false;
        // [S4NexonProto] ORIG 0x081afc36 `jle 0x081afc86`（条件假跳**函数尾部**
        // 的失败岛 +0x1ee）⇒ 源码是 if/else 形：then 块 fallthrough、else 块
        // 落函数尾（旧形 `if (result <= 0) return false;` + 平铺令 4.4 把该
        // return 块放中间：`jg +0x193; mov $0; jmp end`，岛序与 ORIG 不符）。
        if (result > 0)
        {
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
        else
        {
            return false;
        }
    }

    memcpy(&m_peerAdrs, &adrs.sin_addr, 4);
    m_peerPort = adrs.sin_port;
    return true;
}

#include "nexon_cash/accept.inc"
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

#include "nexon_cash/getPeerAdrs.inc"


#include "nexon_cash/getPeerPort.inc"


#include "nexon_cash/setOptResizeSendBuf.inc"


#include "nexon_cash/setOptResizeRecvBuf.inc"


#include "nexon_cash/SetRecvBufSize.inc"
{
    msRecvBufSize = size;
}

#include "nexon_cash/SetSendBufSize.inc"
{
    msSendBufSize = size;
}

#include "nexon_cash/pollReadEvent.inc"
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

#include "nexon_cash/pollWriteEvent.inc"
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

#include "nexon_cash/pollErrorEvent.inc"
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

#include "nexon_cash/UDPSocket.inc"


#include "nexon_cash/dtor_UDPSocket.inc"


#include "nexon_cash/open_1.inc"


#include "nexon_cash/bind_1.inc"
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

#include "nexon_cash/bind_2.inc"
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
            puts("Cannot assign requested address");
            break;
        case 0:
            break;
        default:
            printf("Could not bind UDP receive port. Error= %d , strerror = %s\n",
                   e, strerror(e));
            break;
        }
        return false;
    }
    if (nonBlock)
        setOptNonBlock();
    return true;
}

#include "nexon_cash/send_1.inc"
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

#include "nexon_cash/recv_1.inc"
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

#include "nexon_cash/getHandle_1.inc"


#include "nexon_cash/close_1.inc"


#include "nexon_cash/setOptNonBlock_1.inc"


#include "nexon_cash/setOptResizeSendBuf_1.inc"
{
    if (size <= 0)
        return false;
    int ret = ::setsockopt(m_socket, 1, 7, &size, sizeof(size));
    if (ret < 0)
        return false;
    return true;
}

#include "nexon_cash/setOptResizeRecvBuf_1.inc"
{
    if (size <= 0)
        return false;
    int ret = ::setsockopt(m_socket, 1, 8, &size, sizeof(size));
    if (ret < 0)
        return false;
    return true;
}

#include "nexon_cash/pollReadEvent_1.inc"
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

#include "nexon_cash/pollWriteEvent_1.inc"
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

#include "nexon_cash/pollErrorEvent_1.inc"
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

#include "nexon_cash/Transaction.inc"


#include "nexon_cash/execute.inc"


#include "nexon_cash/confirm.inc"


#include "nexon_cash/cancel.inc"


#include "nexon_cash/expired.inc"


#include "nexon_cash/id.inc"


#include "nexon_cash/getUser.inc"


// ===========================================================================
// UserBalance
// ===========================================================================

#include "nexon_cash/balance.inc"


#include "nexon_cash/balance_1.inc"


#include "nexon_cash/addNewTransaction.inc"


#include "nexon_cash/removeTransaction.inc"


// ===========================================================================
// TransactionManager
// ===========================================================================

int TransactionManager::tx_id_ = 0;             // B 0x09407d90

#include "nexon_cash/instance.inc"


#include "nexon_cash/TransactionManager.inc"
#include "nexon_cash/dtor_TransactionManager.inc"

#include "nexon_cash/addTransaction.inc"


#include "nexon_cash/removeTransaction_1.inc"


#include "nexon_cash/getTransaction.inc"


#include "nexon_cash/cancel_1.inc"


#include "nexon_cash/confirm_1.inc"


#include "nexon_cash/getNextTransactionID.inc"


// ===========================================================================
// NexonCash（薄包装）
//
// [GI 拆分] NCashImpl 的定义已迁至 NXCash.cpp；本 TU 对 new NCashImpl 只见
// 声明 ⇒ 4.4 保守生成 new-expression EH cleanup（delete + _Unwind_Resume 岛，
// ORIG 0x081ae21b-0x081ae232 共 9 insns），与拆分前同形。
// ===========================================================================

#include "nexon_cash/NexonCash.inc"


#include "nexon_cash/dtor_NexonCash.inc"


#include "nexon_cash/getBalance.inc"


#include "nexon_cash/purchase.inc"


#include "nexon_cash/purchase_1.inc"


#include "nexon_cash/gift.inc"


#include "nexon_cash/confirm_2.inc"


#include "nexon_cash/cancel_2.inc"


#include "nexon_cash/checkCoupon.inc"
#include "nexon_cash/useCoupon.inc"
#include "nexon_cash/checkCafeCoupon.inc"
#include "nexon_cash/useCafeCoupon.inc"

#include "nexon_cash/charge.inc"


#include "nexon_cash/checkEntrance.inc"


}  // namespace cash
}  // namespace nexon
