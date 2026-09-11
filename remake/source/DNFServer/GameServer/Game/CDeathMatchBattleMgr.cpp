#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDeathMatchBattleMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// ============================================================================
// df_game_r CDeathMatchBattleMgr（G2-5 批次，PvP_Room +0x69c，尺寸 0x44）
// 逐函数对照 docs/class_func_reports/CDeathMatchBattleMgr.md 与 ORIG 反汇编还原。
// 布局（由 SetParent/IncreaseKillCount/IncreaseDeathCount/OnStart 反汇编推导）：
//   +0x00 PvP_Room* m_pRoom
//   +0x04 int m_killCount[8]     （IncreaseKillCount 用 this+idx*4+4）
//   +0x24 int m_deathCount[8]    （IncreaseDeathCount 用 this+(idx+8)*4+4）
// sizeof == 0x44。
// 返回类型以 ORIG 反汇编/mangling 为准：checkWinner* 为 void（GameStubs 桩的
// int 是错的）；checkGameOver* 返回 0/1（调用点 test %al，声明为 int 与 ORIG
// mov $imm,%eax 形态一致）。
// 跨类调用经权威头（PvP_Room.h/TimerQueue.h/CDeathMatchBattleMgr.h），mangled 名与 ORIG 完全一致。
// ============================================================================

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <utility>

#include "CDeathMatchBattleMgr.h"
#include "PvP_Room.h"
#include "TimerQueue.h"

// cMyTrace 权威声明见 GameTypes.h（经 PvP_Room.h 链包含），本地类已删除（§7）。

#include "CDeathMatchBattleMgr/funDeathMatchKillCntCompare.inc"


// ---- CDeathMatchBattleMgr（0x44，权威声明见 CDeathMatchBattleMgr.h） ----

#include "CDeathMatchBattleMgr/Reset.inc"


#include "CDeathMatchBattleMgr/SetParent.inc"


#include "CDeathMatchBattleMgr/OnCreateRoom.inc"


#include "CDeathMatchBattleMgr/OnStart.inc"


#include "CDeathMatchBattleMgr/IncreaseKillCount.inc"


#include "CDeathMatchBattleMgr/IncreaseDeathCount.inc"


#include "CDeathMatchBattleMgr/getKillCount.inc"


#include "CDeathMatchBattleMgr/getDeathCount.inc"


#include "CDeathMatchBattleMgr/getMaxKillCount.inc"


#include "CDeathMatchBattleMgr/InsertTimerKilledUserForRevive.inc"


#include "CDeathMatchBattleMgr/checkGameOverForSinglePlay.inc"


#include "CDeathMatchBattleMgr/checkGameOverForTeamPlay.inc"


#include "CDeathMatchBattleMgr/checkWinnerForSinglePlay.inc"


#include "CDeathMatchBattleMgr/checkWinnerForTeamPlay.inc"


#include "CDeathMatchBattleMgr/getRanking.inc"


#include "CDeathMatchBattleMgr/OnJoinRoom.inc"


#include "CDeathMatchBattleMgr/OnLeaveRoom.inc"


#include "CDeathMatchBattleMgr/OnReady.inc"


#include "CDeathMatchBattleMgr/OnChangeTeam.inc"


#include "CDeathMatchBattleMgr/OnFight.inc"

