#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CmdPacketStructCtors/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CMDPacketStruct.h"

namespace CMDPacketStruct
{

#include "CmdPacketStructCtors/_ReqAdvanceAltarPause.inc"

#include "CmdPacketStructCtors/_ResAdvanceAltarPause.inc"


#include "CmdPacketStructCtors/_ReqAdvanceAltarBuyItem.inc"

#include "CmdPacketStructCtors/_ResAdvanceAltarBuyItem.inc"


#include "CmdPacketStructCtors/_ReqAdvanceAltarSetSlot.inc"

#include "CmdPacketStructCtors/_ResAdvanceAltarSetSlot.inc"


#include "CmdPacketStructCtors/_ReqAdvanceAltarResetStar.inc"

#include "CmdPacketStructCtors/_ResAdvanceAltarResetStar.inc"


#include "CmdPacketStructCtors/_ReqAdvanceAltarSummonUnit.inc"

#include "CmdPacketStructCtors/_ResAdvanceAltarSummonUnit.inc"


#include "CmdPacketStructCtors/_ReqAdvanceAltarUpgradeGage.inc"

#include "CmdPacketStructCtors/_ResAdvanceAltarUpgradeGage.inc"


#include "CmdPacketStructCtors/_ReqAdvanceAltarExchangeSlot.inc"

#include "CmdPacketStructCtors/_ResAdvanceAltarExchangeSlot.inc"


#include "CmdPacketStructCtors/_ReqAdvanceAltarGetAchievementReward.inc"

#include "CmdPacketStructCtors/_ResAdvanceAltarGetAchievementReward.inc"


#include "CmdPacketStructCtors/_STReqEventDungeonClearRoom.inc"

#include "CmdPacketStructCtors/_STResEventDungeonClearRoom.inc"

#include "CmdPacketStructCtors/_STReqEventDungeonDestoryObject.inc"

#include "CmdPacketStructCtors/_STResEventDungeonDestoryObject.inc"


}  // namespace CMDPacketStruct
