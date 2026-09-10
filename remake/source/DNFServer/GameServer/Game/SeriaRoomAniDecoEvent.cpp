// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 SeriaRoomAniDecoEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <boost/foreach.hpp>       // ORIG：questClearLog 用 BOOST_FOREACH
#include <boost/lexical_cast.hpp>  // ORIG：buyItemLog/questClearLog 用 lexical_cast
#include "GlobalData.h"
#include "W8LT_EventTail.h"
#include "GameTypes.h"            // cMyTrace
#include "GlobalData.h"
#include "CSystemTime.h"
#include "StreamPool.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"
#include "MoreTypes.h"            // SIG_GIVE_GROWTH_CREATURE_EVENT
#include "CUser.h"
#include "../../../Library/Include/Core/SecureStdio.h"   // ss_strcpy                // GrowthCreatureEvent / getGrowthCreatureEventdata
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "CDataManager.h"
#include "MySQL.h"            // MySQL::{set_query,exec,getAffectedRowCount}
#include "CEnvironment.h"     // DB_HANDLE_9 / E_SERVER_GROUP_NONE
#include "CItem.h"
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GameWorld.h"
#include "RDARScriptStringManager.h"
#include "CMailBoxHelper.h"

// ============================ SeriaRoomAniDecoEvent ============================

SeriaRoomAniDecoEvent::SeriaRoomAniDecoEvent() {}       // C1/C2 0x0827b490
SeriaRoomAniDecoEvent::~SeriaRoomAniDecoEvent() {}      // D1/D2 0x0827b4ac

#include "SeriaRoomAniDecoEvent/IsEventing.inc"

#include "SeriaRoomAniDecoEvent/StartEvent.inc"

#include "SeriaRoomAniDecoEvent/EndEvent.inc"

#include "SeriaRoomAniDecoEvent/StartEvent_1.inc"

#include "SeriaRoomAniDecoEvent/StartAction.inc"

#include "SeriaRoomAniDecoEvent/dailyresetData.inc"

// [b28d-solo-W8LT_EventTail] DIFF→NEAR(N7) | ORIG 0x0827b75a | P167 CLEAR（23/23）。
// 修法①：`int index` 改直接写 `item.m_addInfo`（ORIG 0x0827b80b `mov %eax,-0x63`
// = item@-0x65 + 0x02）⇒ 槽集/帧 0x1ac→0x19c 全对齐；②P223 多岛：守卫2/守卫4
// 改负条件 goto + 尾标签串 `L_itemIdx:; L_index:;` ⇒ 复现 ORIG +0x231/+0x234
// 两 nop 中继岛，148=148。残余（N 封顶）：ReqDBSendNewSystemMail 实参中
// `GetServerGroupToPvP()`↔`strlen()` 的 callee-saved 角色互换——ORIG 4.1 分配序
// esi(serverGroup)→edi(strlen)→ebx(characNo)，4.4.6 为 edi→esi→ebx（N109：裸 register
// 与 asm 绑定两形均不翻；两值均无槽往返，P0 禁具名缓存，且 T5 禁为装载序改类型）。
#include "SeriaRoomAniDecoEvent/SendAniDecoItem.inc"

#include "SeriaRoomAniDecoEvent/SendEventInfo.inc"

#include "SeriaRoomAniDecoEvent/CheckEventInfo.inc"

// [b23S3C] ORIG 0x0827ba52 `movzwl -0x22(%ebp)` = deco(sret @-0x24)+2 ⇒ 奖励索引用
// deco_flow(m_field2)，原码误取 m_field0（+0 是 receive_flow 计数）⇒ 真 bug 修正；
// strncpy 后 ORIG 无 `movb $0x0,title+255` ⇒ 删补 NUL 行（与 SendAniDecoItem 同）
// 残余 28 全为帧槽号平移（4.1 把 index 挤在 item 之后 -0x5f、4.4 独立高槽 = N82）、
// N7 虚调用 %edx/%eax 归属、以及 `mov MEM,%eax; test` vs 4.4 `cmpl $0,MEM`（N-88 候选）
// ⇒ 语义已核，调用点级无剩余缺口。
// [b28d-solo-W8LT_EventTail] DIFF→NEAR(N7) | ORIG 0x0827ba52 | P167 CLEAR（15/15）。
// 修法：`int index = src->get_index()` 改为直接写 `item.m_addInfo`（ORIG
// 0x0827bad5 `mov %eax,-0x5f` = item@-0x61 + 0x02）⇒ 槽集/帧全对齐、102=102；
// 亦解释 ORIG 的 unsigned 字段 test 形（P209）。残余（N 封顶，与 SendAniDecoItem
// 同族）：ReqDBSendNewSystemMail 实参中 `GetServerGroupToPvP()`↔`strlen()` 的
// callee-saved 角色互换——ORIG 4.1 esi→edi→ebx，4.4.6 edi→esi→ebx（N109；本 TU
// 已路由 4.4.6 后端，P230 亦不翻；两值无槽往返，P0 禁具名缓存）。
#include "SeriaRoomAniDecoEvent/processEvent.inc"

#include "SeriaRoomAniDecoEvent/makeRequest.inc"

// ============================================================================
// DB_UpdateSeriaRoomAnimationDeco::dispatch —— ORIG T 0x0844adc0（报告
// docs/class_func_reports/DB_UpdateSeriaRoomAnimationDeco/dispatch.md）。
// GetDBHandle(DB_HANDLE_9,0) → >>accId(u32) >>serverGroup(s16) >>characNo(int)
// >>field1(s16) >>field2(s16) → UPDATE receive_flow=field1, deco_flow=field2；
// affected==0 → INSERT（occ_date=now()）；exec 失败返 0。
// ============================================================================
#include "SeriaRoomAniDecoEvent/dispatch.inc"

// [L32 T3] ORIG `correctQuestIds` D 0x09370168 size 0xc：TU 级 file-scope 全局
//   （非 const ⇒ .data；值 {0x1567,0x1568,0x1569}），由 OneADayItemShopEvent::isQuestId
//   0x081ba1e2 线性查表消费。原先误做成函数内 `static const int`（局部符号 + .rodata）
//   ⇒ 提为 TU 级定义。位置在首个强符号（SeriaRoomAniDecoEvent::C2Ev）之后，GI 键名不变。
int correctQuestIds[3] = {0x1567, 0x1568, 0x1569};

// ============================ OneADayItemShopEvent ============================
OneADayItemShopEvent::OneADayItemShopEvent() {}         // C1/C2 0x081b9fb4
OneADayItemShopEvent::~OneADayItemShopEvent() {}        // D1/D2 0x081b9fd0

#include "SeriaRoomAniDecoEvent/StartEvent_2.inc"

#include "SeriaRoomAniDecoEvent/EndEvent_1.inc"

// [b28d-solo-W8LT_EventTail] AE | ORIG 0x081ba03a | P167 CLEAR（2/2）。
// 修法：P227 全表达式临时链（具名 trace 改 `cMyTrace(...)(fmt, a, b)`）⇒
// ORIG 的 `push esi/ebx` + ctor 前两条 movswl 跨 ctor 保活形逐条复现（35=35）。
#include "SeriaRoomAniDecoEvent/StartEvent_3.inc"

// ORIG 0x081ba100：G_GameWorld()->ResetOneADayItemShop()。
#include "SeriaRoomAniDecoEvent/StartAction_1.inc"

#include "SeriaRoomAniDecoEvent/deliveryStart.inc"

#include "SeriaRoomAniDecoEvent/deliveryEnd.inc"

#include "SeriaRoomAniDecoEvent/delivery.inc"

#include "SeriaRoomAniDecoEvent/isQuestId.inc"

// [b28d-solo-W8LT_EventTail] AE | ORIG 0x081ba21e | P167 CLEAR（38/38）。
// 修法：①字段经 m 视图读（非 msg.）；②删 progress/p 具名局部（P0/P183）；
// ③条件 `empty()`→`size() != 0`（p167 MISSING size）；④循环体由 `text = X + sep`
// 改左结合累加 `text = text + X + sep`——补回 p167 MISSING 的 1×`operator+` 与
// 2×`~string`（ORIG 两次 op+ 的临时析构），AST 左结合形与 ORIG 逐条同构；
// ⑤memcpy dest 用具名 `sig.m_data`：4.4 复现 ORIG「先压 size/src、后 lea+add 算
// dest」的装载序（原 `(unsigned char*)&sig + 0x14` 使 4.4 提前算到 %edx）；
// ⑥`sig` 局部加 `__attribute__((aligned(4)))`：ORIG 4.1 给 pack(1) 载荷 4 字节
// 对齐槽（-0xbc），4.4 默认 1 字节对齐（-0xb9）⇒ 补属性后槽基与全帧逐条对齐。
// ⑦memcpy 无补 NUL（P171 照抄）。
#include "SeriaRoomAniDecoEvent/questClearLog.inc"

// [b28d-solo-W8LT_EventTail] AE | ORIG 0x081ba5e8 | P167 CLEAR（25/25）。
// 修法：①字段经 m 视图读；②删 p 具名局部（P0）；③`text += X + "/"` →
// `text = X + sep`（sep=string("/")，P13/P0 构造序在条件前）；④lexical_cast
// 实参直传 payload+2 地址（const&，无具名 value）；⑤删补 NUL（P171）；
// ⑥memcpy dest 改具名 `sig.m_data` ⇒ 复现 ORIG「先压 size/src、后 lea+add 算 dest」
// （原 `(unsigned char*)&sig + 0x14` 使 4.4 提前算到 %edx，P75 残差清零）。
// 依赖：CItemShop::IsOneADayShop char→bool（Main 代落）。
#include "SeriaRoomAniDecoEvent/buyItemLog.inc"

// ==================== DB request 类（W8LT_EventTail.h 声明点） ====================
#include "SeriaRoomAniDecoEvent/makeRequest_1.inc"
