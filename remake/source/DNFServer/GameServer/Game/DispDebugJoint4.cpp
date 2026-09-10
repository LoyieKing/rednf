// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DispDebugJoint4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— LinkMisc4 批次：Dispatcher 域 2 符号（DispDebugJoint4.cpp）
//
//   1. DisPatcher_DebugCommand::_debugCommandSetLevel(CUser*, int)
//      ORIG _ZN23DisPatcher_DebugCommand21_debugCommandSetLevelEP5CUseri
//      T 0x0858efde sz 0x3db
//      报告 docs/class_func_reports/DisPatcher_DebugCommand/_debugCommandSetLevel.md
//      声明为 DisPatcherCore.h 类内纯追加（本批次）。
//
//   2. 自由函数 DisPatcher_DisJointItem_disjoint —— 桥接转写。
//      ORIG 真实对应静态成员
//      DisPatcher_DisJointItem::_disjoint(CUser*, int, int, ENUM_CMDPACKET,
//                                         CUser*, unsigned short)
//      ORIG _ZN23DisPatcher_DisJointItem9_disjointEP5CUserii14ENUM_CMDPACKETS1_t
//      T 0x081f92ca sz 0xcee
//      报告 docs/class_func_reports/DisPatcher_DisJointItem/_disjoint.md
//      桥接原因：调用方 GmCmds_F.cpp:58 已按自由函数名
//      DisPatcher_DisJointItem_disjoint 声明并禁改；ORIG 静态成员无 this、
//      i386 下与同参数表自由函数 ABI 一致，故在本 TU 以该自由函数承载完整
//      ORIG 逻辑（逐段对照报告汇编/反编译 C 转写）。
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

// ORIG _disjoint 内 logFormat/__PRETTY_FUNCTION__ 原文（报告 0x87b7/0x87bd 行）
static const char g_disjointPrettyFunction[] =
    "static int DisPatcher_DisJointItem::_disjoint(CUser*, int, int, "
    "ENUM_CMDPACKET, CUser*, short unsigned int)";
static const char g_disjointSourceFile[] = "PacketDispatcher_Impl_1.cpp";

// ===========================================================================
// 1. DisPatcher_DebugCommand::_debugCommandSetLevel
//    ORIG 0x0858efde..0x0858f3b9
// ===========================================================================
#include "DispDebugJoint4/_debugCommandSetLevel.inc"
}
