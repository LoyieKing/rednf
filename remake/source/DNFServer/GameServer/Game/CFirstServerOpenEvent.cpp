// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CFirstServerOpenEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r CFirstServerOpenEvent 实现（OrigMiscEvt 批补齐）。
// 声明见 CFirstServerOpenEvent.h。逐函数依据
// docs/class_func_reports/CFirstServerOpenEvent/ 报告转写：
//   C1 0x0811ca78  D1 0x0811ca94  D0 0x0811cac4
//   StartEvent() 0x0811cae2  StartEvent(Word_Param) 0x0811cb34
//   EndEvent 0x0811cb86  IsEventing 0x0811cbd8
// vtable 0x08b530e0 = 基类 14 槽 + 派生新增槽 15（IsEventing 非 const
// 重载，未覆盖基类 const IsEventing）；无派生数据字段（sizeof 0xc）。
// 追踪用 __FUNCTION__（_ZZN21CFirstServerOpenEvent*E12__FUNCTION__）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN21CFirstServerOpenEventC2Ev`（ORIG 0x811cc46）；ctor 已是首强符号。
#include <iostream>

#include "CFirstServerOpenEvent.h"
#include "GameTypes.h"   // cMyTrace

#include "CFirstServerOpenEvent/CFirstServerOpenEvent.inc"

#include "CFirstServerOpenEvent/dtor_CFirstServerOpenEvent.inc"

#include "CFirstServerOpenEvent/IsEventing.inc"

#include "CFirstServerOpenEvent/StartEvent.inc"

#include "CFirstServerOpenEvent/StartEvent_1.inc"

#include "CFirstServerOpenEvent/EndEvent.inc"

