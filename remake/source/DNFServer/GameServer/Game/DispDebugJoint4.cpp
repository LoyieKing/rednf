#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DispDebugJoint4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <algorithm>

#include "DisPatcherCore.h"     // DisPatcher_DebugCommand（含 MSG_BASE/PacketBuf 域）
#include "CUser.h"              // CUser/CUserCharacInfo/UserQuest/eSendTarget/ENUM_ERROR
#include "CDataManager.h"       // G_CDataManager/find_item/get_level_exp/m_disJoint
#include "DisJoint.h"           // DisJoint::GetResult/DisJointResult
#include "CInventory.h"         // CInventory/Store/G_Store/CValueStatistic/
                                // GetInstanceValueStatistic/GetInvenTypeFromItemSpace
#include "Inven_Item.h"
#include "CItem.h"              // make_item/is_stackable
#include "CExpertJob.h"         // expert_job::CExpertJob/CDisjointer
#include "GameWorld.h"          // G_GameWorld/IsPVPChannel
#include "CCubeStatistic.h"     // GetInstanceCubeStatistic/collectCubeStatistics
#include "CUserProc.h"          // APSystem::CUserProc::ClearActionAndSendtoUser
#include "HistoryLog.h"         // HistoryLog::WriteDisJoint
#include "LogManager.h"
#include "GameTypes.h"          // cMyTrace
#include "PacketGuard.h"

// b22 桥清零（§8 协议，Model22）：原 extern"C" 五桥全清——
//   CDisjointer::{OnStartMission,get_disjoint_result,complete_disjoint} 三桥
//   声明在本 TU 无调用点（真实 _disjoint 逻辑在 DisPatcherCore.cpp:6320/6358/
//   6495 已是成员真调用，CExpertJob.h 权威声明），按 §8-4 删无语用桥声明；
//   CUser::CalcurateUserMaxLevel / CUser::SetMercenaryInfoPacket 二桥转真成员
//   调用（CUser.h:2633-2634 已声明，零新声明）。

// [OurOnly33 符号名轴 2026-09-11] 原手写串 static（g_disjointPrettyFunction /
// g_disjointSourceFile，ORIG 全 ELF 无该 base 名，我方 `_ZL24…`/`_ZL20…` r 段）
// 在本 TU **无任何引用点**（grep 全 source 仅命中此定义；GCC 默认
// -fkeep-static-consts 故其符号此前仍被发射）⇒ 删除。
// ORIG 权威：`_ZZN23DisPatcher_DisJointItem9_disjointEP5CUserii14ENUM_CMDPACKETS1_tE19__PRETTY_FUNCTION__`
// @0x08bcfd20 (r,0x6c=108B) —— 该 prett 符号由 DisPatcherCore/_disjoint.inc 的
// __PRETTY_FUNCTION__ 使用点发射，与这两个未引用手写串无关。

#include "DispDebugJoint4/_debugCommandSetLevel.inc"

