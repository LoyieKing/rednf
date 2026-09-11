// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDeathTowerWinPointEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r CDeathTowerWinPointEvent 实现（OrigMiscEvt 批补齐）。
// 声明见 CDeathTowerWinPointEvent.h。报告缺失，依据 ORIG nm+objdump 转写：
//   C1/C2 0x081170e8 D1/D2 0x0811710e D0 0x0811713e
//   StartEvent() 0x0811700e StartEvent(Word_Param) 0x0811706e
//   EndEvent 0x08117096 getBonusRate W 0x08469dcc
// vtable 0x08b4f3a0 = 基类 14 槽；+0xc m_ratio(unsigned)；
// getBonusRate 经 vptr+0x3c IsEventing 判活 → (double)m_ratio/100.0f。
#include "CDeathTowerWinPointEvent.h"
#include "GameTypes.h"   // cMyTrace

#include "CDeathTowerWinPointEvent/StartEvent.inc"

#include "CDeathTowerWinPointEvent/CDeathTowerWinPointEvent.inc"

#include "CDeathTowerWinPointEvent/dtor_CDeathTowerWinPointEvent.inc"
#include "CDeathTowerWinPointEvent/StartEvent_1.inc"

#include "CDeathTowerWinPointEvent/EndEvent.inc"


#include "CDeathTowerWinPointEvent/getBonusRate.inc"

