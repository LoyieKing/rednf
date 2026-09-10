// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LevelUpBefore70LvEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— LevelUpBefore70LvEvent（70 级前升级 repeat 事件）。
// 全部函数逐指令对照 docs/class_func_reports/LevelUpBefore70LvEvent/*.md
// 的 ## 汇编 与 ## 反编译 C 转写（报告地址见各函数头注释）：
//   C1 0x0811ff60 / D1 0x0811ff7c / D0 0x0811ffac / StartEvent() 0x0811ffca /
//   StartEvent(Word_Param) 0x0812001c / EndEvent() 0x0812006e
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN22LevelUpBefore70LvEventC2Ev`（ORIG 0x81200ff）；ctor 已是首强符号。
#include <iostream>

#include "LevelUpBefore70LvEvent.h"

// ORIG C1 0x0811ff60：CEventBase::CEventBase() + vptr 设置，无自身字段。
#include "LevelUpBefore70LvEvent/LevelUpBefore70LvEvent.inc"
{
}

// ORIG D1 0x0811ff7c：vptr 设置 + CEventBase::~CEventBase() + 删除标志检查
// （mov $0x0,%eax; test %al,%al; je —— gcc 虚析构标准形态）。
#include "LevelUpBefore70LvEvent/dtor_LevelUpBefore70LvEvent.inc"
{
}

// ORIG 0x0811ffca：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x12,0)。
// 日志名用 __FUNCTION__（ORIG .rodata 符号 _ZZN22...E12__FUNCTION__ 实证）。
#include "LevelUpBefore70LvEvent/StartEvent.inc"
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x12, 0);
    trace("LevelUpBefore70Lv Event start!");
}

// ORIG 0x0812001c：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x18,0)；
// Word_Param 参数未使用（Ghidra C 仅 this 参与）。
#include "LevelUpBefore70LvEvent/StartEvent_1.inc"
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x18, 0);
    trace("LevelUpBefore70Lv Event start! [monitor request]");
}

// ORIG 0x0812006e：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x1e,0)。
// ORIG W 0x0812011c：恒返 1（该事件常驻生效）。
#include "LevelUpBefore70LvEvent/IsEventing.inc"
{
    return 1;
}

#include "LevelUpBefore70LvEvent/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 0x1e, 0);
    trace("LevelUpBefore70Lv Event end!");
}
