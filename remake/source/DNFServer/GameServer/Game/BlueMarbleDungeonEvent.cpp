// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BlueMarbleDungeonEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 蓝大理石地下城事件 BlueMarbleDungeonEvent 实现。
// 报告：docs/class_func_reports/BlueMarbleDungeonEvent/ 全部 6 个方法已读。
//   C1 0x080dc470 / D1 0x080dc48c / D0 0x080dc4bc / StartEvent 0x080dc4da /
//   StartEvent(Word_Param) 0x080dc52c（完整实现）/ EndEvent 0x080dc57e。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN22BlueMarbleDungeonEventC2Ev`（ORIG 0x80dc60f）；ctor 已是首强符号。
#include <iostream>

#include "BlueMarbleDungeonEvent.h"

#include "GameTypes.h"   // cMyTrace

// ORIG 0x080dc470：仅基类构造 + vptr 置位。
#include "BlueMarbleDungeonEvent/BlueMarbleDungeonEvent.inc"
{
}

// ORIG 0x080dc48c / 0x080dc4bc：空体 + 虚析构由编译器生成同形。
#include "BlueMarbleDungeonEvent/dtor_BlueMarbleDungeonEvent.inc"
{
}

// ORIG 0x080dc4da：SetEventFlag(true) + cMyTrace(__FUNCTION__, 0x16, 0)
//   ("BlueMarbleDungeonEvent Start!")。__FUNCTION__ 名（0x0b 长度符号）。
#include "BlueMarbleDungeonEvent/StartEvent.inc"
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x16, 0);
    trace("BlueMarbleDungeonEvent Start!");
}

// ORIG 0x080dc52c：SetEventFlag(true) + cMyTrace(__FUNCTION__, 0x1c, 0)
//   ("BlueMarbleDungeonEvent Start!")。完整实现，非 thunk。
#include "BlueMarbleDungeonEvent/StartEvent_1.inc"
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x1c, 0);
    trace("BlueMarbleDungeonEvent Start!");
}

// ORIG 0x080dc57e：SetEventFlag(false) + cMyTrace(__FUNCTION__, 0x22, 0)
//   ("BlueMarbleDungeonEvent End!")。
#include "BlueMarbleDungeonEvent/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 0x22, 0);
    trace("BlueMarbleDungeonEvent End!");
}
