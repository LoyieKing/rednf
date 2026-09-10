// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CAssaultOnOffEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 袭击开关事件 CAssaultOnOffEvent 实现。
// 报告缺失（§3.2 兜底）：ORIG 全部 W 弱符号，nm 定址 + objdump 逐指令转写：
//   C1 0x0811bb80 / D1 0x0811bb9c / D0 0x0811bbcc / StartEvent 0x0811bac2 /
//   StartEvent(Word_Param) 0x0811bb14（thunk）/ EndEvent 0x0811bb2e；
//   vtable 0x08b52288；PRETTY_FUNCTION 与日志字符串 .rodata 实测。
#include "CAssaultOnOffEvent.h"

#include "GameTypes.h"   // cMyTrace

// ORIG 0x0811bb80：仅基类构造 + vptr 置位。
#include "CAssaultOnOffEvent/CAssaultOnOffEvent.inc"
{
}

// ORIG 0x0811bb9c / 0x0811bbcc：空体 + 虚析构由编译器生成同形。
#include "CAssaultOnOffEvent/dtor_CAssaultOnOffEvent.inc"
{
}

// ORIG 0x0811bac2：SetEventFlag(true) + cMyTrace(PRETTY, 0xe, 0)
//   ("Assault On Off Event Start!")。
#include "CAssaultOnOffEvent/StartEvent.inc"
{
    SetEventFlag(true);
    cMyTrace trace(__PRETTY_FUNCTION__, 0xe, 0);
    trace("Assault On Off Event Start!");
}

// ORIG 0x0811bb14（thunk，0x1a）：经 vptr+8 虚调 StartEvent()。
#include "CAssaultOnOffEvent/StartEvent_1.inc"
{
    // [b28c-27] N7 封顶：唯一残差=虚调用目标寄存器（ORIG 用 %edx，4.4 恒 %eax）。
    StartEvent();
}

// ORIG 0x0811bb2e：SetEventFlag(false) + cMyTrace(PRETTY, 0x18, 0)
//   ("Assault On Off Event End!")。
#include "CAssaultOnOffEvent/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x18, 0);
    trace("Assault On Off Event End!");
}
