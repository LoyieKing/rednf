// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CSpecialItemRoutingManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CSpecialItemRoutingManager.cpp
// ORIG 依据：docs/class_func_reports/CSpecialItemRoutingManager/StartRouting.md
//   + objdump 0x0860a9d4/0x0860b45c/0x0860b4c2 逐指令忠实转写。
#include "CSpecialItemRoutingManager.h"

#include "CDataManager.h"  // G_CDataManager / find_item
#include "CParty.h"        // IsRoutingState/SetStartRouting/SetRoutingItem/
                           // gen_timer_key/GetPartyIndex/send_to_party
#include "CUser.h"         // get_state/GetParty
#include "Inven_Item.h"    // m_addInfo（+0x02 item idx）
#include "PacketGuard.h"
#include "TimerQueue.h"    // G_TimerQueue/TimerEntry/TIMER_MESSAGE

#include "CSpecialItemRoutingManager/CSpecialItemRoutingManager.inc"


// [b28d-64 判词汇总 · 泳道64] 本 TU 1 件：StartRouting 0x0860a9d4
//   102/102 N封顶（P251 候补：ORIG `cmp $0x5,%eax; cmp $0xc,%eax` 寄存器直比 +
//   两 nop 岛 vs 我方具名 state 落槽 -0x14 无岛；语义等价 ⇒ N82/N7）。
// [M5A_DispLink] 102/104：ORIG 0x0860aa25 `mov %eax,%ebx`（item idx 驻留跨 call）+
//   0x0860aa36 `mov %eax,-0xc(%ebp); cmpl $0x0,-0xc(%ebp)`（具名 CItem* 槽直比）⇒ 旧形
//   （具名 dataMgr + 返回值直接 `== 0`）多 2 条（sete 链 + 求值顺序倒置）；改链式调用。

#include "CSpecialItemRoutingManager/StartRouting.inc"


#include "CSpecialItemRoutingManager/insertTimer.inc"


#include "CSpecialItemRoutingManager/cancelTimer.inc"


#include "CSpecialItemRoutingManager/ProcessGiveUp.inc"


#include "CSpecialItemRoutingManager/dtor_CSpecialItemRoutingManager.inc"


#include "CSpecialItemRoutingManager/ProcessTimeOut.inc"

#include "CSpecialItemRoutingManager/ArriveMemberSelection.inc"

