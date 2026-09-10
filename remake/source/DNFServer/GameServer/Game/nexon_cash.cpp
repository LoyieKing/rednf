// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 nexon_cash/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
#include "nexon_cash/expire.inc"
}

#include "nexon_cash/TCPSocket.inc"
}
// ORIG 0x081af50e (sz 0x0f)：反汇编体仅把参数 port 存入栈局部并返回，
// 不触及 this、无任何字段写入（弱符号内联残留形态）。忠实保留为空操作。
#include "nexon_cash/TCPSocket_1.inc"
}

// ORIG 0x081af51e：仅调用 close()。
#include "nexon_cash/dtor_TCPSocket.inc"
}

// ORIG 0x081af532：
//   m_socket = socket(AF_INET=2, SOCK_STREAM=1, IPPROTO_TCP=6);
//   if (m_socket == -1) { printf("Could not create a TCP socket : %d\n",
//        errno); return false; }  // 字符串 @0x08bb4aac
//   setOptLinger(false); setOptNagle(false); return true;
#include "nexon_cash/open.inc"
}

// ORIG 0x081af5b4：setOptReuseAdrs(true)；本地 sockaddr_in{AF_INET,htons(port),
// INADDR_ANY}；bind 失败（结果的符号位）→ close(); return false；
// 成功且 nonBlock → setOptNonBlock()；printf("TCP Port binding success, port
// = %d\n", port)（字符串 @0x08bb4ad0）；return true。
#include "nexon_cash/bind.inc"
}

// ORIG 0x081af674：listen(m_socket, backlog) 符号位失败 → close(); false。
#include "nexon_cash/listen.inc"
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
#include "nexon_cash/send.inc"
}

// ORIG 0x081af76e：
//   if (!buffer || !length) return -1;
//   n = read(...); if (n < 0) { (errno==EAGAIN||errno==EINTR||errno==EAGAIN||
//       errno==0) → return 0（0x081af7b5/7c1/7cd je 共岛 +0x6c；test jne→+0x80）；
//       其余 → return n（原负值，0x081af7ee mov -0xc,%eax） }
//   else if (n == 0) return -1;   // 对端关闭
//   return n;
#include "nexon_cash/recv.inc"
}

#include "nexon_cash/getHandle.inc"
}

// ORIG 0x081af7fe (sz 0xd)：mov 0x8(%ebp),%eax; mov (%eax),%eax;
// cmp $0xffffffff,%eax; pop; ret —— 尾置 return 的 if 被折叠成裸 cmp（技巧6变体）。
#include "nexon_cash/shutdown.inc"
}

// ORIG 0x081af80c：
//   if (m_socket == -1) return;
//   close(m_socket); m_socket=-1; memset(this+0x14,0,4); m_peerPort=0;
#include "nexon_cash/close.inc"
}

// ORIG 0x081af85e：flags=fcntl(F_GETFL); flags|=O_NONBLOCK(0x800);
// fcntl(F_SETFL, flags) 符号位失败→false。
#include "nexon_cash/setOptNonBlock.inc"
}

// ORIG 0x081af8bc：setsockopt(SOL_SOCKET, SO_REUSEADDR=2, &flag(0/1), 4)。
// flag 两语句式：死预置 0（0x081af8c8 movl $0,-0xc）+ 显式 if/else 各支内存存储
// （0x081af8d5 movl $1 / 0x081af8de movl $0），三元写法会被折成寄存器选择形。
#include "nexon_cash/setOptReuseAdrs.inc"
}

// ORIG 0x081af926：linger{l_onoff=(on?1:0), l_linger=0}；
// setsockopt(SOL_SOCKET, SO_LINGER=13, &l, 8)，负结果→false。
#include "nexon_cash/setOptLinger.inc"
}

// 返回 setsockopt()==0。ORIG 反汇编：结果先存栈变量再 cmpl+sete 返回。
#include "nexon_cash/setOptNagle.inc"
}

// ORIG 0x081af9ea：本地 sockaddr_in{AF_INET,inet_addr(host),htons(port)}；
// connect 失败→false；成功则 memcpy(&m_peerAdrs,&sin_addr,4)、
// m_peerPort=sin_port，return true。
// [b28d-D43] P75 实参装载时刻：ORIG 提前 `mov -0xc(%ebp),%ecx`（跨两条 this 装载）vs 4.4 使用点才读（ORIG 0x081af9ea）。
#include "nexon_cash/connect.inc"
}

// ORIG 0x081afa98 (sz 0x230)：非阻塞连接 + select 等待 + SO_ERROR 校验。
// [b28d-D43] P75 实参装载时刻 + 帧差（155/154）（ORIG 0x081afa98）。
#include "nexon_cash/connect_nonb.inc"
}

// ORIG 0x081aff2c：peer.m_socket = accept(m_socket, &peer.m_adrs…?)。
// 细节：accept 的地址缓冲写入 peer+0x04（sin 区域），len=16；
// 结果 <0 或 ==-1 → false；
// memcpy(&peer.m_peerAdrs, peer_adrs+0x08, 4)（即 sin_addr）；
// peer.m_peerPort = *(u16*)(adrs+0x06)（sin_port 原样）；
// peer.setOptNonBlock(); return true。
#include "nexon_cash/accept.inc"
}

// ORIG 0x081affb8：lea eax,[this+0x14]; ret —— 返回成员 m_peerAdrs 的地址。
#include "nexon_cash/getPeerAdrs.inc"
}

#include "nexon_cash/getPeerPort.inc"
}

// ORIG 0x081affd0：具名 ret 槽（-0xc cmpl/jns）+ 两条 dead 数组写
// （0x081affe4 movl $0,-0x10 / 0x081affeB movl $4,-0x14，技巧31 dead 槽撑帧）。
#include "nexon_cash/setOptResizeSendBuf.inc"
}

// ORIG 0x081b0036：与 SendBuf 同形（dead_vals @0x081b004a/0x081b0051）。
#include "nexon_cash/setOptResizeRecvBuf.inc"
}

#include "nexon_cash/SetRecvBufSize.inc"
}

#include "nexon_cash/SetSendBufSize.inc"
}

// 共同骨架（ORIG 三者同构，仅 select 第二/三/四参与超时不同）：
// 清零 fd_set(32*int)，FD_SET(m_socket)，select(nfds=2, ...)，ret !=0。
// [b28d-D43] AE: FD_ZERO→显式循环（pset/i 全头部声明复现 -0x1c/-0x20 槽序）+FD_SET unsigned cast 复现 shr（ORIG 0x081afcc8）。
#include "nexon_cash/pollReadEvent.inc"
}

// [b28d-D43] AE: 同 pollReadEvent（ORIG 0x081afd94）。
#include "nexon_cash/pollWriteEvent.inc"
}

// [b28d-D43] AE: 同 pollReadEvent（ORIG 0x081afe60）。
#include "nexon_cash/pollErrorEvent.inc"
}

int TCPSocket::msRecvBufSize = 0;  // B 0x09407d68
int TCPSocket::msSendBufSize = 0;  // B 0x09407d6c
const unsigned int TCPSocket::MAX_SEND_RETRY;  // R 0x08bb49e0（类内 =0x64 的类外定义，发射 R 符号）


// ===========================================================================
// UDPSocket
// ===========================================================================

// ORIG 0x081aed2c：m_socket = INVALID_SOCKET; m_port=0。
#include "nexon_cash/UDPSocket.inc"
}

// ORIG 0x081aed44：仅调用 close()。
#include "nexon_cash/dtor_UDPSocket.inc"
}

// ORIG 0x081aed58：
//   if (m_socket != -1) return false;
//   m_socket = socket(AF_INET, SOCK_DGRAM(2), 0);
//   if (-1) printf("Could not create a UDP socket : %d\n", errno) → false;
//   else true。字符串 @0x08bb49e4。
#include "nexon_cash/open_1.inc"
}

// ORIG 0x081aedc6：保存主机序端口；清零 m_adrs；
// m_adrs={AF_INET=2, inet_addr(host)}；sin_port=htons(m_port)；
// bind(m_socket,&m_adrs,16)；失败→printf("Udp Port binding fail, %s , %d\n",
// host, port)（@0x08bb4a08）并 false。
// [b28d-D43] AE: m_adrs 具名 sockaddr_in 视图直存 + htons(m_port)（ORIG 0x081aedc6）。
#include "nexon_cash/bind_1.inc"
}

// ORIG 0x081aee82：同上但 INADDR_ANY；失败按 errno 分支：
//   EADDRINUSE(98)→printf("Port %d for receiving UDP is in use\n",port)
//                 （@0x08bb4a28），
//   EACCES(99)→puts("Cannot assign requested address.")（@0x08bb4a50），
//   其他非零→printf("Could not bind UDP receive port. Error= %d , strerror"
//                 "=%s\n"…)（@0x08bb4a70）；errno==0 不打印。
// 失败返回 false；成功且 nonBlock→setOptNonBlock()，true。
#include "nexon_cash/bind_2.inc"
}

// ORIG 0x081af000：
//   if (!data || length<=0) return -1; if (!host) return 0;
//   目标 sockaddr_in{AF_INET,htons(port),inet_addr(host)}；
//   n = sendto(m_socket,data,length,0,&adrs,16)；
//   n<0 且 errno∈{EWOULDBLOCK,EAGAIN,EINTR}→0；否则 n。
#include "nexon_cash/send_1.inc"
}

// ORIG 0x081af0de：
//   if (!buffer || length<=0) return -1;
//   addrlen=16；n=recvfrom(m_socket,buffer,length,0,(sockaddr*)&m_from,&addrlen)
//   —— ORIG 直接以 this+0x18 作为源地址存储（即成员 m_from）。
//   n<0: (errno==EAGAIN||errno==EINTR||errno==EAGAIN)→ return 0（0x081af162 共岛），
//        否则 → return -1（0x081af169）；n>=0 原样返回（0x081af170）。
// [b28d-D43] P75/P52：ORIG 提前 `lea -0x10(%ebp),%ebx`（callee-saved 独占）vs 4.4 推迟到实参装载点（ORIG 0x081af0de）。
#include "nexon_cash/recv_1.inc"
}

#include "nexon_cash/getHandle_1.inc"
}

// ORIG 0x081af184：if (m_socket == INVALID_SOCKET)return; close(); m_socket=-1; m_port=0;
#include "nexon_cash/close_1.inc"
}

// ORIG 0x081aefa2：与 TCPSocket 同构的 F_GETFL|O_NONBLOCK / F_SETFL。
#include "nexon_cash/setOptNonBlock_1.inc"
}

// ORIG 0x081af1b8 / 0x081af210：size<=0→false；
// setsockopt(SOL_SOCKET, SO_SNDBUF/SO_RCVBUF, &size, 4)，负→false。
// 结果为具名 ret 槽（0x081af250 mov %eax,-0xc; cmpl; jns），frame 0x38。
#include "nexon_cash/setOptResizeSendBuf_1.inc"
}

#include "nexon_cash/setOptResizeRecvBuf_1.inc"
}

// ORIG 0x081af268：read 集 select(nfds=2)，超时 {0,5000}微秒，ret!=0。
// [b28d-D43] AE: 同 pollReadEvent（ORIG 0x081af268）。
#include "nexon_cash/pollReadEvent_1.inc"
}

// ORIG 0x081af334：write 集超时 {0,0}。
// [b28d-D43] AE: 同 pollReadEvent（ORIG 0x081af334）。
#include "nexon_cash/pollWriteEvent_1.inc"
}

// ORIG 0x081af400：except 集，超时 {0,0}。
// [b28d-D43] AE: 同 pollReadEvent（ORIG 0x081af400）。
#include "nexon_cash/pollErrorEvent_1.inc"
}

// ===========================================================================
// Transaction
// ===========================================================================

// ORIG 0x081b02ec (sz 0x49)：id=0→getNextID；amount、createdTime=0→time(0)。
#include "nexon_cash/Transaction.inc"
}

// ORIG 0x081b0336：balance < amount（有符号 setl）→ 3；
// 否则 balance(-amount) + addTransaction → 0。ORIG 无二次执行防护，忠实保留。
#include "nexon_cash/execute.inc"
}

// ORIG 0x081b0398：仅出管理器登记表。
#include "nexon_cash/confirm.inc"
}

// ORIG 0x081b03b4：先退款再出表。
#include "nexon_cash/cancel.inc"
}

// ORIG 0x081b1656：time(NULL)-createdTime > 15。
#include "nexon_cash/expired.inc"
}

#include "nexon_cash/id.inc"
}

#include "nexon_cash/getUser.inc"
}

// ===========================================================================
// UserBalance
// ===========================================================================

#include "nexon_cash/balance.inc"
}

#include "nexon_cash/balance_1.inc"
}

// ORIG 0x081b15ca：printf("addNewTx: %p\n", tx)（fmt @0x8bb55c0 原样）+ push_back。
#include "nexon_cash/addNewTransaction.inc"
}

// ORIG 0x081b1530：erase(remove(begin,end,tx), end) 惯用法（ORIG 实例化了
// std::remove 与两参 erase 的 W 符号）。
// [b28d-D43] P23/P75 求值序：ORIG erase 两实参 end,end,begin,remove 序 vs 4.4 remove 提前（ORIG 0x081b1530）。
#include "nexon_cash/removeTransaction.inc"
}

// ===========================================================================
// TransactionManager
// ===========================================================================

int TransactionManager::tx_id_ = 0;             // B 0x09407d90

// ORIG 0x081b0128：guard 单例（ORIG guard@0x9407db8 / obj@0x9407dc0 +
// __cxa_atexit(D1)）。C++98 局部 static 由编译器生成同型代码。
#include "nexon_cash/instance.inc"
}

#include "nexon_cash/TransactionManager.inc"
#include "nexon_cash/dtor_TransactionManager.inc"
#include "nexon_cash/addTransaction.inc"
}

// ORIG 0x081b021c：erase(key)。
#include "nexon_cash/removeTransaction_1.inc"
}

// ORIG 0x081b0444：find == end → NULL。
#include "nexon_cash/getTransaction.inc"
}

// ORIG 0x081b0244：tx 存在→tx->cancel()（退款+自移除）、user 表移除、delete tx。
// 正向 if 包裹（0x081b0263 je 直达 leave +0x51，无尾 jmp/nop 岛）。
#include "nexon_cash/cancel_1.inc"
}

// ORIG 0x081b0298：同 cancel 但不退款（tx->confirm() 仅自移除）。正向 if 同形。
#include "nexon_cash/confirm_1.inc"
}

// ORIG 0x081b04cc：前缀 ++ 后返回新值。
#include "nexon_cash/getNextTransactionID.inc"
}

// ===========================================================================
// NexonCash（薄包装）
//
// [GI 拆分] NCashImpl 的定义已迁至 NXCash.cpp；本 TU 对 new NCashImpl 只见
// 声明 ⇒ 4.4 保守生成 new-expression EH cleanup（delete + _Unwind_Resume 岛，
// ORIG 0x081ae21b-0x081ae232 共 9 insns），与拆分前同形。
// ===========================================================================

// ORIG 0x81ae1b0。
#include "nexon_cash/NexonCash.inc"
}

// ORIG 0x081ae250：free(m_impl)，非 delete —— 忠实保留（ORIG free@plt）。
#include "nexon_cash/dtor_NexonCash.inc"
}

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
