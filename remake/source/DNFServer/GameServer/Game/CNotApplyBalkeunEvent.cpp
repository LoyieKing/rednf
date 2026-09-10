// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CNotApplyBalkeunEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 不适用巴尔肯事件 CNotApplyBalkeunEvent 实现。
// 报告：docs/class_func_reports/CNotApplyBalkeunEvent/ 全部 6 个方法。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN21CNotApplyBalkeunEventC2Ev`（ORIG 0x81b9f81）；ctor 已是首强符号。
#include <iostream>

#include "CNotApplyBalkeunEvent.h"

#include "CEnvironment.h"          // G_CEnvironment / m_serverEnvir.m_channelNo
#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x081b9d5c：仅基类构造 + vptr 置位。
#include "CNotApplyBalkeunEvent/CNotApplyBalkeunEvent.inc"

// ORIG 0x081b9d78 / 0x081b9da8：空体 + 虚析构由编译器生成同形。
#include "CNotApplyBalkeunEvent/dtor_CNotApplyBalkeunEvent.inc"

// ---------------------------------------------------------------------------
// ORIG 0x081b9dc6：IsEventing(0) 为假时置事件标志并打日志（带 server 号）。
#include "CNotApplyBalkeunEvent/StartEvent.inc"

// ---------------------------------------------------------------------------
// ORIG 0x081b9e4c：同 StartEvent()（忽略 param）。
#include "CNotApplyBalkeunEvent/StartEvent_1.inc"

// ---------------------------------------------------------------------------
// ORIG 0x081b9ed2：IsEventing(0) 为真时清事件标志并打日志。
#include "CNotApplyBalkeunEvent/EndEvent.inc"
