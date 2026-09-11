#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 AvatarConvertServer/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ AvatarConvertServer + ARAD::SCRIPT::AvatarConvert 实现。
// 逐指令转写依据 docs/class_func_reports/ARAD__SCRIPT__AvatarConvert/*.md 与
// docs/class_func_reports/AvatarConvertServer/{AvatarConvertServer,~AvatarConvertServer,
// Test,Load,Destroy}.md（ORIG 地址见各函数注释）。
#include <cstring>
#include <new>

#include <cstdio>
#include <string>

#include "AvatarConvertServer.h"
#include "GameTypes.h"      // cMyTrace
// ----------------------------------------------------------------------------
// [b28d-E03 2026-09-10 本波复核] 四数 102/182/1/5 不变（4 件维持封顶）：
//   · StatisticsEmblemCreate 115/112：P231 纯 register 试形（itemId 升格 callee-saved）
//     实测**无效已回退**；残差 = 实参升格时刻 P75 + N94/N4 索引分解 + N22。
//   · CheckCondition 293/292：N18a 两段装 callee-saved（`mov 0x4(%eax),%eax;
//     mov %eax,%ebx` 4.4 恒折单段）。
//   · Convert 610/604：N18a/N82（p167 CLEAR）。
//   · ARAD::SCRIPT::AvatarConvert::loadScript NEAR：N7 寄存器角色（路由 446 后端已启用）。
//   · `_ZNSt20__uninitialized_copyILb0EE…move_iterator…` 实例：N89 库头族。
// ----------------------------------------------------------------------------
// [L3-b 2026-09-11 本波核账] 四数 102/182/1/5 不变（4 件仍封顶），落地 4 项：
//   · 串轴真 bug（Convert）：ORIG 0x08192ca5（makeRequest 第3参）与
//     0x08192d2c/0x08192d34（insert_event_items 的 PKc/PKc）三条立即数都是
//     0x8b9a913 = .rodata 空串 ""（前两条 0x8b9a912/0x8b9a913 连续 NUL）；
//     旧源码发明 "AVATAR CONVERT" ⇒ 已改 `(char*)""` 与 `"", ""`。
//   · Convert 三个 codegen 试形（实测读数，非推断）：
//     ② `insertSlot` 具名局部内联 ⇒ 4.4 发 ORIG 同形的 `shr $0x1f,%eax;
//        test %al,%al`（消 `mov %eax,槽; cmpl $0,槽; jns` 槽往返）；
//     ③ 三个 `int`（slotIdx/slotOut/extra）的 `=0` 清零点移到 `s` 物化之后
//        （ORIG 0x08192df4 先 begin()/const_iterator 拷贝构造，0x08192e21 才清）⇒
//        归一 diff 191→188 行；④ `isRechargeStone` 去具名 bool（P183）⇒ 188→181；
//     ⑤ `if (resultItem->is_stackable()){…} else {FAIL0}` 正向包裹 ⇒ 消 `xor $1`
//        物化且 FAIL0 后置，与 ORIG 0x08192940 `test %al,%al; je 0x08192a30` 同形
//        ⇒ 181→**171**（单本件累计 217→171，our 604→602）。
//   · 回滚（负结论）：cMyTrace 三实参（charac_no/m_slot/m_selectNo）具名前置
//     ⇒ our 603→609 且归一 diff 188→190（4.4 -O0 具名局部走栈槽，复现不了
//     ORIG 0x08192f13..0x08192f30 的 ebx/edi/esi 跨 ctor 保活）⇒ N 族。
//   · 其余残差全为 4.4/4.1 族：帧差（原 0x14c/我方 0x130）、ORIG 多 `push %edi`、
//     N18a（`mov 0x4(%eax),%eax; mov %eax,%ebx` 两段装）、4.1 实参升格槽往返
//     （ORIG 有 / 我方无，如 0x08192xxx `mov %eax,(%stk); mov (%stk),%eax`）、
//     disp 分解（`mov idx,%edx; add $8,%edx` vs 4.4 `lea 8(%eax),%edx`）。
// ----------------------------------------------------------------------------
#include "DNFLexWrapper.h"  // loadRDARScriptFile/unloadRDARScriptFile/ScanType/
                            // ScanInt(bool*)/ScanTypeOrInt（真实符号声明）

// ---- 布局断言（ORIG 反汇编实证；std::map=0x18 与 ORIG 一致）----
typedef char avatar_convert_size_assert[sizeof(ARAD::SCRIPT::AvatarConvert) == 0x18 ? 1 : -1];
typedef char avatar_convert_getinfo_size_assert[sizeof(ARAD::SCRIPT::AvatarConvert::GetInfo) == 0x8 ? 1 : -1];
typedef char avatar_convert_result_size_assert[sizeof(ARAD::SCRIPT::AvatarConvert::ConvertResult) == 0x24 ? 1 : -1];
typedef char avatar_convert_server_size_assert[sizeof(AvatarConvertServer) == 0x1c ? 1 : -1];

namespace ARAD
{
namespace SCRIPT
{

#include "AvatarConvertServer/AvatarConvert.inc"


#include "AvatarConvertServer/dtor_AvatarConvert.inc"


#include "AvatarConvertServer/loadScript.inc"


#include "AvatarConvertServer/clearScript.inc"


#include "AvatarConvertServer/getConvertResult.inc"


}  // namespace SCRIPT
}  // namespace ARAD

#include "AvatarConvertServer/AvatarConvertServer.inc"


#include "AvatarConvertServer/dtor_AvatarConvertServer.inc"


#include "AvatarConvertServer/Load.inc"


#include "AvatarConvertServer/Destroy.inc"


#include "AvatarConvertServer/Test.inc"


// ============================================================================
// LinkClosureB.AradServers 追加段：
//   StatisticsAvatarDisjoint ORIG T 0x08193136（StatisticsAvatarDisjoint.md 转写）
//   StatisticsEmblemCreate  ORIG T 0x081931dc（StatisticsEmblemCreate.md + 汇编，
//     包布局 Packet_Emblem_Create_Statistic：PacketHeader(0x17a4,0x36)+count@0xa
//     +grades@0xe[5]+addInfo@0x22[5]；ORIG packet 类 C1 W @0x08110bdc 归
//     PacketTypes 域，此处按字节布局组装，不重复建模。）
//   CheckCondition          ORIG T 0x08192212（CheckCondition.md 全文转写）
//   Convert                 ORIG T 0x081926de（Convert.md 全文转写）
// ============================================================================

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "CAvatarItemMgr.h"
#include "CDataManager.h"       // G_CDataManager / find_item / ENUM_CMDPACKET
#include "LogManager.h"
#include "CStatisticServerProxy.h"
#include "CServerProxyMgr.h"
#include "GlobalData.h"
#include "AvatarRechargeServer.h"

#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MoreTypes.h"          // arad::SigAradAvatarConvert 完整定义
#include "Packet_Avater_Disjoint_Statistic.h"  // 统计包权威布局（ORIG W C1 0x08110bae）
#include "Packet_Emblem_Create_Statistic.h"    // 统计包权威布局（ORIG W C1 0x08110bdc）
// DB_UpdateAvatarHistory —— ORIG makeRequest T @0x0844536a。
// 权威定义在 CompoundDeathLCB.cpp（LinkClosureB.CompoundDeath 批），
// 此处仅经 DB_UpdateAvatarHistory.h 引用，避免多重定义。
#include "DB_UpdateAvatarHistory.h"

#include "AvatarConvertServer/StatisticsAvatarDisjoint.inc"


#include "AvatarConvertServer/StatisticsEmblemCreate.inc"


#include "AvatarConvertServer/CheckCondition.inc"


#include "AvatarConvertServer/Convert.inc"

