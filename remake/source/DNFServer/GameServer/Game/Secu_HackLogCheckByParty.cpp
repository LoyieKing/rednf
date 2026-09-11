// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Secu_HackLogCheckByParty/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Secu_HackLogCheckByParty.h"
#include "CParty.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CMap.h"
#include "CInventory.h"
#include "LogManager.h"
// ============================================================================
// df_game_r Secu_HackLogCheckByParty 还原（G2-5 批次，自包含 TU）
// 逐函数对照 docs/class_func_reports/Secu_HackLogCheckByParty.md 与 ORIG 反汇编。
// 布局（ORIG ctor/dtor/Init 推导，sizeof 0x14，CParty +0x1abc）：
//   +0x00 Secu_DungeonAverageCheck 基类（0x10：CParty* + vector 占位）
//   +0x10 CParty* m_party2
// 类型声明统一于 Secu_HackLogCheckByParty.h（唯一声明点）。
// ============================================================================

// ============================================================================
// 实现
// ============================================================================

#include "Secu_HackLogCheckByParty/Init.inc"


#include "Secu_HackLogCheckByParty/Secu_HackLogCheckByParty.inc"


#include "Secu_HackLogCheckByParty/dtor_Secu_HackLogCheckByParty.inc"


#include "Secu_HackLogCheckByParty/DieMob.inc"


#include "Secu_HackLogCheckByParty/CheckMoveMapFitInPathHackCnt.inc"



#include "Secu_HackLogCheckByParty/startDungeon.inc"


#include "Secu_HackLogCheckByParty/dungeonClear.inc"


#include "Secu_HackLogCheckByParty/dungeonFail.inc"

