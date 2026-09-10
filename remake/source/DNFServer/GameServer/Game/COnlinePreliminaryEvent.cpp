// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 COnlinePreliminaryEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 在线预选赛事件 COnlinePreliminaryEvent 实现。
// 报告：docs/class_func_reports/COnlinePreliminaryEvent/ 全部 6 个方法。
#include "COnlinePreliminaryEvent.h"

#include "GameWorld.h"             // G_GameWorld / GetChannelType
#include "OnlinePreliminary.h"     // online_preliminary::COnlinePreliminary::SetMatchAble
#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x081bb490：仅基类构造 + vptr 置位。
#include "COnlinePreliminaryEvent/COnlinePreliminaryEvent.inc"
{
}

// ORIG 0x081bb4ac / 0x081bb4dc：空体 + 虚析构由编译器生成同形。
#include "COnlinePreliminaryEvent/dtor_COnlinePreliminaryEvent.inc"
{
}

// ---------------------------------------------------------------------------
// ORIG 0x081bb4fa：置事件标志、打日志；频道类型为 0xe 时开放匹配。
#include "COnlinePreliminaryEvent/StartEvent.inc"
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x15, 0);
    tr("COnlinePreliminaryEvent Start!");
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        online_preliminary::COnlinePreliminary::SetMatchAble(true);
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x081bb56e：同 StartEvent()（忽略 param）。
#include "COnlinePreliminaryEvent/StartEvent_1.inc"
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1f, 0);
    tr("COnlinePreliminaryEvent Start!");
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        online_preliminary::COnlinePreliminary::SetMatchAble(true);
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x081bb5e2：清事件标志；频道类型为 0xe 时关闭匹配。
#include "COnlinePreliminaryEvent/EndEvent.inc"
{
    SetEventFlag(false);
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        online_preliminary::COnlinePreliminary::SetMatchAble(false);
    }
}
