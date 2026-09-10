// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_MomijiEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 阿拉德红叶事件 Arad_MomijiEvent 实现（Game 域；Monitor/ 同名类为
// 另一独立模块，与此无关）。
// 报告：docs/class_func_reports/Arad_MomijiEvent/ 全部 6 个方法已读。
//   C1 0x0819e738 / D1 0x0819e754 / D0 0x0819e784 / StartEvent 0x0819e7a2 /
//   StartEvent(Word_Param) 0x0819e7f4（thunk）/ EndEvent 0x0819e80e。
// 注意：StartEvent()/EndEvent() 中 trace 在 SetEventFlag 之前（ORIG 汇编序）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit ⇒
// 本 TU 发射 `_GLOBAL__I__ZN16Arad_MomijiEventC2Ev`（ORIG 0x819e89f）；ctor 已是首强符号。
#include <iostream>

#include "Arad_MomijiEvent.h"

#include "GameTypes.h"   // cMyTrace

// ORIG 0x0819e738：仅基类构造 + vptr 置位。
#include "Arad_MomijiEvent/Arad_MomijiEvent.inc"
{
}

// ORIG 0x0819e754 / 0x0819e784：空体 + 虚析构由编译器生成同形。
#include "Arad_MomijiEvent/dtor_Arad_MomijiEvent.inc"
{
}

// ORIG 0x0819e7a2：cMyTrace(PRETTY, 0x14, 0)("[MOMIJI_EVENT] Event Start.")
// 在前，SetEventFlag(true) 在后。
#include "Arad_MomijiEvent/StartEvent.inc"
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x14, 0);
    trace("[MOMIJI_EVENT] Event Start.");
    SetEventFlag(true);
}

// ORIG 0x0819e7f4（thunk，0x1a）：经 vptr+8 虚调 StartEvent()。
#include "Arad_MomijiEvent/StartEvent_1.inc"
{
    // [b28c-27] N7 封顶：唯一残差=虚调用目标寄存器（ORIG `mov (%eax),%edx; ...;
    // call *%edx` vs 4.4 `mov (%eax),%eax; ...; call *%eax`）；4.4 间接 call 目标
    // 恒 %eax，源码形不可择（探针 /tmp/l22p/vprobe*.cpp）。
    StartEvent();
}

// ORIG 0x0819e80e：cMyTrace(PRETTY, 0x1f, 0)("[MOMIJI_EVENT] Event End.")
// 在前，SetEventFlag(false) 在后。
#include "Arad_MomijiEvent/EndEvent.inc"
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1f, 0);
    trace("[MOMIJI_EVENT] Event End.");
    SetEventFlag(false);
}
