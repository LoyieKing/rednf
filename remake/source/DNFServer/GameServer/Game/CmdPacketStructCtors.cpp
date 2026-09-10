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

// ---- Pause ----
#include "CmdPacketStructCtors/_ReqAdvanceAltarPause.inc"
{
    memset(this, 0, 0xe);
}
#include "CmdPacketStructCtors/_ResAdvanceAltarPause.inc"
{
    memset(this, 0, 0xc);
}

// ---- BuyItem ----
#include "CmdPacketStructCtors/_ReqAdvanceAltarBuyItem.inc"
{
    memset(this, 0, 0x20);
}
#include "CmdPacketStructCtors/_ResAdvanceAltarBuyItem.inc"
{
    memset(this, 0, 0x1c);
}

// ---- SetSlot ----
#include "CmdPacketStructCtors/_ReqAdvanceAltarSetSlot.inc"
{
    memset(this, 0, 0x24);
}
#include "CmdPacketStructCtors/_ResAdvanceAltarSetSlot.inc"
{
    memset(this, 0, 0x1c);
}

// ---- ResetStar ----
#include "CmdPacketStructCtors/_ReqAdvanceAltarResetStar.inc"
{
    memset(this, 0, 0x14);
}
#include "CmdPacketStructCtors/_ResAdvanceAltarResetStar.inc"
{
    memset(this, 0, 0xc);
}

// ---- SummonUnit ----
#include "CmdPacketStructCtors/_ReqAdvanceAltarSummonUnit.inc"
{
    memset(this, 0, 0x18);
}
#include "CmdPacketStructCtors/_ResAdvanceAltarSummonUnit.inc"
{
    memset(this, 0, 0x14);
}

// ---- UpgradeGage ----
#include "CmdPacketStructCtors/_ReqAdvanceAltarUpgradeGage.inc"
{
    memset(this, 0, 0x18);
}
#include "CmdPacketStructCtors/_ResAdvanceAltarUpgradeGage.inc"
{
    memset(this, 0, 0x14);
}

// ---- ExchangeSlot ----
#include "CmdPacketStructCtors/_ReqAdvanceAltarExchangeSlot.inc"
{
    memset(this, 0, 0x30);
}
#include "CmdPacketStructCtors/_ResAdvanceAltarExchangeSlot.inc"
{
    memset(this, 0, 0x24);
}

// ---- GetAchievementReward ----
#include "CmdPacketStructCtors/_ReqAdvanceAltarGetAchievementReward.inc"
{
    memset(this, 0, 0x14);
}
#include "CmdPacketStructCtors/_ResAdvanceAltarGetAchievementReward.inc"
{
    memset(this, 0, 0xc);
}

// ---- EventDungeon ClearRoom / DestoryObject ----
#include "CmdPacketStructCtors/_STReqEventDungeonClearRoom.inc"
{
    memset(this, 0, 0x14);
}
#include "CmdPacketStructCtors/_STResEventDungeonClearRoom.inc"
{
    memset(this, 0, 0xc);
}
#include "CmdPacketStructCtors/_STReqEventDungeonDestoryObject.inc"
{
    memset(this, 0, 0x18);
}
#include "CmdPacketStructCtors/_STResEventDungeonDestoryObject.inc"
{
    memset(this, 0, 6);   // ORIG 字面量（< sizeof(8)，忠实复刻）
}

}  // namespace CMDPacketStruct
