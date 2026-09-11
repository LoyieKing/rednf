// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CFatigueAttendance/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 疲劳签到事件 CFatigueAttendance 实现。
// 报告缺失（docs/class_func_reports/CFatigueAttendance 不存在），按 §3.2
// 兜底：nm 定址（C1 0x081173ea / D1 0x08117406 / D0 0x08117436 /
// StartEvent 0x08117454 / StartEvent(Word_Param) 0x08117470 /
// EndEvent 0x081174fe，全部 W 弱符号）+ objdump 区间逐指令转写；
// __PRETTY_FUNCTION__ 字符串与 fmt 字符串取自 .rodata（nm 符号
// CFatigueAttendance::StartEvent(Word_Param)::__PRETTY_FUNCTION__ @0x08b4f000、
// ::EndEvent()::__PRETTY_FUNCTION__ @0x08b4f040、fmt @0x08b4de30/0x08b4de5f）。
#include "CFatigueAttendance.h"

#include "GameTypes.h"             // cMyTrace

#include "CFatigueAttendance/CFatigueAttendance.inc"


#include "CFatigueAttendance/dtor_CFatigueAttendance.inc"


#include "CFatigueAttendance/StartEvent.inc"


#include "CFatigueAttendance/StartEvent_1.inc"


#include "CFatigueAttendance/EndEvent.inc"

