// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCharacterDayEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CCharacterDayEvent —— 角色生日事件实现。
// 函数逐段对齐 docs/class_func_reports/CCharacterDayEvent/ 下报告：
//   CCharacterDayEvent.md(C1 0x0810a4c8) / ~CCharacterDayEvent.md(D1 0x0810a4ee) /
//   ~CCharacterDayEvent_0810a51e.md(D0 0x0810a51e) / StartEvent.md(0x0810a53c) /
//   StartEvent_0810a58e.md(0x0810a58e) / EndEvent.md(0x0810a5ec)。
// GetEventJob(0x08697886) 无独立报告（nm 定址 objdump：movzwl 0xa 直返）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN18CCharacterDayEventC2Ev`（ORIG 0x810a67d）；ctor 已是首强符号。
#include <iostream>

#include "CCharacterDayEvent.h"

#include "GameTypes.h"   // cMyTrace

#include "CCharacterDayEvent/CCharacterDayEvent.inc"

#include "CCharacterDayEvent/dtor_CCharacterDayEvent.inc"

// [b28d-D9] N18a：ORIG movzwl 0xc(%ebp),%eax + mov %eax,%edx 两段；4.4 恒单段（四形探针 a-d 全败）（ORIG 0x0810a58e sz 0x0000005e）
#include "CCharacterDayEvent/StartEvent.inc"

#include "CCharacterDayEvent/StartEvent_1.inc"

#include "CCharacterDayEvent/EndEvent.inc"

#include "CCharacterDayEvent/GetEventJob.inc"
