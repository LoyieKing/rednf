// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDimensionActivationEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CDimensionActivationEvent 实现。
// 依据 docs/class_func_reports/CDimensionActivationEvent/*.md + ORIG elf 反汇编。
// 数据面：CDataManager+0x4b18 起 m_dimensionAct（DimensionActivationData）：
//   +0x00 int 日计数、+0x04 map<int,DimensionCoinInfo> m_coinMap。
// <iostream> 引入 _ZStL8__ioinit 动态初始化，键名 _GLOBAL__I__ZN25CDimensionActivationEventC2Ev
// 与 ORIG 0x080eed65 一致（ORIG __static_initialization_and_destruction_0 仅构造 ios_base::Init）。
#include "CDimensionActivationEvent.h"

#include <iostream>
#include <map>
#include "CDataManager.h"
#include "GameTypes.h"   // cMyTrace（定义于 GameUtils.cpp）

#include "CDimensionActivationEvent/CDimensionActivationEvent.inc"


#include "CDimensionActivationEvent/dtor_CDimensionActivationEvent.inc"


#include "CDimensionActivationEvent/StartEvent.inc"


#include "CDimensionActivationEvent/EndEvent.inc"


#include "CDimensionActivationEvent/StartEvent_1.inc"


#include "CDimensionActivationEvent/set_dungeon_play_count.inc"


#include "CDimensionActivationEvent/reset_dungeon_play_count.inc"


#include "CDimensionActivationEvent/checkFreeRevivalCondition.inc"

