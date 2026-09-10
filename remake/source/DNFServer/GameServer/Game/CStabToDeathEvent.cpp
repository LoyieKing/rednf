// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CStabToDeathEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 刺死事件 CStabToDeathEvent 实现。
// 报告：docs/class_func_reports/CStabToDeathEvent/ 全部 6 个方法（已完整阅读
// CStabToDeathEvent / ~CStabToDeathEvent / ~CStabToDeathEvent_0827f554 /
// StartEvent / StartEvent_0827f5c4 / EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN17CStabToDeathEventC2Ev`（ORIG 0x827f6a7）；ctor 已是首强符号。
#include <iostream>

#include "CStabToDeathEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0827f508：仅基类构造 + vptr 置位（无自有成员初始化）。
#include "CStabToDeathEvent/CStabToDeathEvent.inc"
{
}

// ORIG 0x0827f524 / 0x0827f554：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CStabToDeathEvent/dtor_CStabToDeathEvent.inc"
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0827f572：SetEventFlag(true) +
// cMyTrace(&_ZZN17CStabToDeathEvent10StartEventEvE12__FUNCTION__,0x11,0)
// ("CStabToDeathEvent Start!")。
#include "CStabToDeathEvent/StartEvent.inc"
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x11, 0);
    tr("CStabToDeathEvent Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0827f5c4：与 StartEvent() 同体（SetEventFlag(true) + trace，
// file 串为 __FUNCTION__ 局部静态（E12__FUNCTION__），行 0x17）。
#include "CStabToDeathEvent/StartEvent_1.inc"
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x17, 0);
    tr("CStabToDeathEvent Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0827f616：SetEventFlag(false) +
// cMyTrace(&_ZZN17CStabToDeathEvent8EndEventEvE12__FUNCTION__,0x1d,0)
// ("CStabToDeathEvent End!")。
#include "CStabToDeathEvent/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1d, 0);
    tr("CStabToDeathEvent End!");
}
