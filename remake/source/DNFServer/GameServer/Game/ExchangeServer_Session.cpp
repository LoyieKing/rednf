// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ExchangeServer_Session/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r exchange_server::CSession 全方法（OnlyOrig_Exchange 批；
// TUID ExchangeServer_Session.cpp，需登记 CMake）
//
// 反汇编快照 /tmp/es_disasm.txt 行 869-1745 区段逐指令转写。
//
//   布局锚点：CSession sizeof = sizeof(CNetwork<95000,450000>) + 8
//     +0x851a0 m_triggerEventType   +0x851a4 m_channelNo
//
// IExchangeServer 虚槽序（vtable 派生 0x8c7a1d0 / 抽象 0x8c7a1e8）：
//   +0 DispatchPacket(CSession*,char*,int) / +4 UnregisterExchangeServer(int)
//   +8 AllocSession() / +12 FreeSession(CSession*) / +16 DispatchMessage(Stream*)
// 全局 exchange_server_E(0x943d9f4, B) 在 ORIG 直接承载 IExchangeServer*：
// 五函数切片实证全部虚调用均为「全局指针值直作 this、*P 作 vptr」单层解引
// （Listen 0x84907e0 / Connect 0x84908a9 / OnConnect 0x84905da /
//   Parsing 0x849016a / OnAccept），无二次解引层。
//
// 无 BLOCKED 函数。
// ============================================================================

#include "ExchangeServer.h"
#include "CNetwork.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "LogManager.h"
#include "GlobalData.h"           // GlobalData::s_systemTime_（get_ms_tick）
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
// ---- get_ms_tick（ORIG _Z11get_ms_tickv @0x086b1b2f；报告
//      docs/class_func_reports/_lt_global_gt_/get_ms_tick.md。唯一 owner=本 TU，
//      已与宿主 OnlyOrig_Exchange 协调登记。）----
long long get_ms_tick()
{
    return (long long)GlobalData::s_systemTime_.getCurTickCount();
}

namespace exchange_server
{


// ORIG：虚调用点全部内联装载全局（mov 0x943d9f4,%eax），无 helper call。
// 用宏保持 -O0 内联形（P49：static 函数 -O0 生真实 call，与 ORIG 不符）。
#define ES() ((IExchangeServer*)CSession::exchange_server_)

// ---------------------------------------------------------------------------
// ctor — ORIG 0x848ffd0 size 0x53：
// 基类构造后清零 +0x851a0/+0x851a4、+0xc 置 1。
// [b28-L11 翻案 b22判词·N] ORIG 基类 ctor 后于栈帧建匿名 CNetwork<95000,450000>
// dead 临时（lea -0x851a8;C1;D1）撑 sub $0x851b8 巨型帧。dead temp 直书 +
// ~CNetwork()/CNetwork() throw()（P216）后 4.4 零 EH 代价、帧精确复现；
// +0xc 单字节写用 P82 具名视图折叠单条 disp（旧取址形展 add+movb 两条）。
// ---------------------------------------------------------------------------
// P82 具名尾部视图：ORIG `movb $0x1,0xc(%eax)` 单条 disp 写（int 字段低字节）。
// `*(char*)&m_field0c` 取址形 4.4 展成 add+movb 两条；视图成员访问折叠单条。
namespace {
struct NetFlagView
{
    char m_pad00[0xc];   // +0x00..+0x0b
    char m_flag0c;       // +0x0c（CNetwork::m_field0c 低字节，ORIG movb 实证）
};
}  // namespace

#include "ExchangeServer_Session/CSession.inc"
}

// D1 — ORIG 0x8495662 size 0x13，仅调基类析构（隐式空体一致）。
#include "ExchangeServer_Session/dtor_CSession.inc"
}

#include "ExchangeServer_Session/GetSocket.inc"
}

#include "ExchangeServer_Session/SetChannelNo.inc"
}

#include "ExchangeServer_Session/GetChannelNo.inc"
}

#include "ExchangeServer_Session/SetTriggerSessionEventType.inc"
}

#include "ExchangeServer_Session/AddTriggerSessionEventType.inc"
}

#include "ExchangeServer_Session/GetTriggerSessionEventType.inc"
}

// ---------------------------------------------------------------------------
// Parsing — ORIG 0x8490074 size 0x29a
// ---------------------------------------------------------------------------
// [b28d-68 修法] ORIG 0x08490074。
// ①控制流按 ORIG 重写为 do{...}while(parsingLength>5)（ORIG 0x084900b5 packetSize=0
//   后 jmp 循环体、0x0849020c `jg` 回跳）；memcpy 无条件执行（ORIG 0x084900df 直落，
//   非 else 分支）——删我方发明的 `m_recvOffset==0 && parsingLength<=5 → return 1`
//   与 `m_recvOffset==0 → packetSize=0` 两条分支（ORIG 无）；
// ②DispatchPacket 返回值存 int moved，`if (moved != 0)` 才 trace（ORIG 0x08490195
//   `mov %eax,-0xc` / `cmpl $0x0` / `je`），替换我方 bool dispatched 取反；
// ③logFormat 第三参改 __PRETTY_FUNCTION__（ORIG 0x8c79d00 串实证 =
//   "bool exchange_server::CSession::Parsing(int)"），0x18d 段补 recvLen 实参
//   （ORIG 7 参：0x14(%esp)=recvLen、0x18(%esp)=packetSize；原 "DispatchPacket()"
//   第三参为发明字符串）。
// 残差 N104/EH：4.4 对 try 内仅 nothrow memmove 的 EH 边整体消除（探针
// /tmp/d68probe/eh.o、eh2.o 三形实证：无 __cxa_begin_catch/_Unwind_Resume），
// 4.1 保留 ⇒ p167 的 5 项 EH MISSING 属跨版本结构性不可清。
#include "ExchangeServer_Session/Parsing.inc"
}

// ---------------------------------------------------------------------------
// OnRecv — ORIG 0x849030e size 0x55
// ---------------------------------------------------------------------------
#include "ExchangeServer_Session/OnRecv.inc"
}

// ---------------------------------------------------------------------------
// OnSend — ORIG 0x8490364 size 0x8a
// ---------------------------------------------------------------------------
#include "ExchangeServer_Session/OnSend.inc"
}
// OnError — ORIG 0x84903ee size 0x70
// ---------------------------------------------------------------------------
#include "ExchangeServer_Session/OnError.inc"
}

// ---------------------------------------------------------------------------
// OnAccept — ORIG 0x849045e size 0x174
// ---------------------------------------------------------------------------
// [b28d-D33 N7/N109 封顶] ORIG 0x0849045e：94=94 条；push esi/ebx 顺序 + 帧
// 0x70 vs 0x84（callee-saved 角色 + 16k+4 量化）。p167 CLEAR 13=13，语义等价。
#include "ExchangeServer_Session/OnAccept.inc"
}

// ---------------------------------------------------------------------------
// OnConnect — ORIG 0x84905d2 size 0xfe
// ---------------------------------------------------------------------------
#include "ExchangeServer_Session/OnConnect.inc"
}

// ---------------------------------------------------------------------------
// Send — ORIG 0x84906d0 size 0x97
// ---------------------------------------------------------------------------
#include "ExchangeServer_Session/Send.inc"
}

// ---------------------------------------------------------------------------
// Listen — ORIG 0x8490767 size 0xc2
// ---------------------------------------------------------------------------
#include "ExchangeServer_Session/Listen.inc"
}

// ---------------------------------------------------------------------------
// Connect — ORIG 0x8490829 size 0xc9
// [N96 清绑 2026-09-09] 删除历史 `register bool failed asm("eax")`（P0 限定② /
// N96 全域禁用；P248 不得恢复）。报告缺失（docs/class_func_reports 无
// exchange_server 目录；同名 CDBConnector/Connect.md 经签名核对为撞名错配，
// 仅用 ORIG 二进制切片，§3.2 兜底并标注）。ORIG 0x849084c
// `cmpl $0x0,-0x10(%ebp); setne %al; test %al,%al; je`——failed 值全程在 %al、
// 无栈槽 ⇒ 按 P0 双推律原码无该具名局部，还原为 `if (ok != 0)` 表达式直判。
// 残差 = N96+N18a 封顶（4.4 对槽比零恒折直跳，不物化 setCC）。
// 读数：清绑前 [identical=3 ae=14 合计17 near=0 diff=3]，本函数 verdict=DIFF
//       orig57/our57；清绑后 [identical=3 ae=13 合计16 near=0 diff=4]，
//       verdict=DIFF orig57/our55（缺 setne+test 两条）。
// ---------------------------------------------------------------------------
#include "ExchangeServer_Session/Connect.inc"
}

// ---------------------------------------------------------------------------
// Disconnect — ORIG 0x84908f2 size 0x9f（P159：函数尾 `mov $0x1,%eax` ⇒ bool true；
// b28-L11 补 return true：gap 1→0；残差 12 全为 vtable+12 FreeSession 虚调用
// N7 寄存器互换（{ecx,eax,edx} 组，4.4 间接 call 恒 %eax，负例表 N7 附）。
// ---------------------------------------------------------------------------
#include "ExchangeServer_Session/Disconnect.inc"
}

}  // namespace exchange_server
