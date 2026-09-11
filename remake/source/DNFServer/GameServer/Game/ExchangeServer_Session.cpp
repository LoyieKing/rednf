#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ExchangeServer_Session/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

// [WorldLane 2026-09-11 · static 多副本普查] 自共享头 ExchangeServer.h 下沉：
// 唯一使用点在本 TU（145/166 两处）；ORIG `_ZN15exchange_serverL11HEADER_SIZEE`
// = 单份 local r 4B @0x08c79790（仅在 1 TU 发射）⇒ namespace-scope const 放头里
// 会随 include 链被 71 个 TU 各发射一份。此处单份定义与 ORIG 1:1。
const unsigned int HEADER_SIZE = 0x0d;

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


#include "ExchangeServer_Session/dtor_CSession.inc"


#include "ExchangeServer_Session/GetSocket.inc"


#include "ExchangeServer_Session/SetChannelNo.inc"


#include "ExchangeServer_Session/GetChannelNo.inc"


#include "ExchangeServer_Session/SetTriggerSessionEventType.inc"


#include "ExchangeServer_Session/AddTriggerSessionEventType.inc"


#include "ExchangeServer_Session/GetTriggerSessionEventType.inc"


#include "ExchangeServer_Session/Parsing.inc"


#include "ExchangeServer_Session/OnRecv.inc"


#include "ExchangeServer_Session/OnSend.inc"

#include "ExchangeServer_Session/OnError.inc"


#include "ExchangeServer_Session/OnAccept.inc"


#include "ExchangeServer_Session/OnConnect.inc"


#include "ExchangeServer_Session/Send.inc"


#include "ExchangeServer_Session/Listen.inc"


#include "ExchangeServer_Session/Connect.inc"


#include "ExchangeServer_Session/Disconnect.inc"


}  // namespace exchange_server
