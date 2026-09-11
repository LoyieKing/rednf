// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Secu_DungeonAverageCheck/<Func>.inc（不参与编译、内部不含 #include）。
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

#include <string.h>
#include <algorithm>   // std::max（ORIG 0x08279a12 调 _ZSt3maxIiERKT_S2_S2_ ×2）

#include "CParty.h"
#include "CUser.h"
#include "WarField.h"          // map_monster（m_roleType +0x08 / m_mobIndex +0x0c）
#include "CGameManager.h"      // WongWork::CHackAnalyzer::addServerHackCnt 声明

#include "Secu_DungeonAverageCheck/reset.inc"


#include "Secu_DungeonAverageCheck/Secu_DungeonAverageCheck.inc"


#include "Secu_DungeonAverageCheck/dtor_Secu_DungeonAverageCheck.inc"


#include "Secu_DungeonAverageCheck/SetPartyPtr.inc"


#include "Secu_DungeonAverageCheck/startDungeon.inc"


// [SweepB] ORIG 0x0827993c：循环体内命中即 return it（sret 拷贝），未命中末尾
// 再求一次 end() 返回（P167 MISSING vector::end×1 的根因，非 break 形）。
std::vector<SecKillMonsterInfo>::iterator
#include "Secu_DungeonAverageCheck/findKillMonsterInfo.inc"


#include "Secu_DungeonAverageCheck/setKillMonsterInfo.inc"


#include "Secu_DungeonAverageCheck/CheckHackTypeEndDungeon.inc"


#include "Secu_DungeonAverageCheck/dungeonClear.inc"


#include "Secu_DungeonAverageCheck/dungeonFail.inc"

