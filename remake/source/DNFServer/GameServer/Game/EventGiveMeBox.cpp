// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EventGiveMeBox/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 台湾 GiveMeBox 事件 EventGiveMeBox 实现。
// 报告：docs/class_func_reports/EventGiveMeBox/ 全部 6 个方法（已完整阅读
// EventGiveMeBox / ~EventGiveMeBox / ~EventGiveMeBox_081649fc /
// StartEvent / StartEvent_08164a6c / EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN14EventGiveMeBox12kRewardItem_E`（ORIG 0x8164b4f）；R 型数据符号为首强符号。
#include <iostream>

#include "EventGiveMeBox.h"

#include "GameTypes.h"             // cMyTrace

// ORIG R 0x08b7cd20（.rodata，4 字节 0x2920918c）：只读静态常量。
const int EventGiveMeBox::kRewardItem_ = 0x2920918c;

#include "EventGiveMeBox/EventGiveMeBox.inc"


#include "EventGiveMeBox/dtor_EventGiveMeBox.inc"


#include "EventGiveMeBox/StartEvent.inc"


#include "EventGiveMeBox/StartEvent_1.inc"


#include "EventGiveMeBox/EndEvent.inc"

