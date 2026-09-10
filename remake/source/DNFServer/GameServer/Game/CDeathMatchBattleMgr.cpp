// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CDeathMatchBattleMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ---- std::pair 排序比较器（ORIG 085df55f T） ----
#include "CDeathMatchBattleMgr/funDeathMatchKillCntCompare.inc"
}

// ---- CDeathMatchBattleMgr（0x44，权威声明见 CDeathMatchBattleMgr.h） ----

// ==== ORIG 085dee22 ====
#include "CDeathMatchBattleMgr/Reset.inc"
}

// ==== ORIG 085df92c（弱符号） ====
#include "CDeathMatchBattleMgr/SetParent.inc"
}

// ==== ORIG 085dee66 ====
#include "CDeathMatchBattleMgr/OnCreateRoom.inc"
}

// ==== ORIG 085deeca ====
#include "CDeathMatchBattleMgr/OnStart.inc"
}

// ==== ORIG 085def0e ====
#include "CDeathMatchBattleMgr/IncreaseKillCount.inc"
}

// ==== ORIG 085def34 ====
#include "CDeathMatchBattleMgr/IncreaseDeathCount.inc"
}

// ==== ORIG 085df524 ====
#include "CDeathMatchBattleMgr/getKillCount.inc"
}

// ==== ORIG 085df540 ====
#include "CDeathMatchBattleMgr/getDeathCount.inc"
}

// ==== ORIG 085defcc ====
#include "CDeathMatchBattleMgr/getMaxKillCount.inc"
}

// ==== ORIG 085def60 ====
// [b28d-27] NEAR N82附：ORIG 0x085def60；多实参 call 首值 callee-saved 寄存器对调
// （ORIG key→%ebx/idx→%esi，我方相反），语句表达式/具名形实测反劣 ⇒ 封顶。
// p167 CLEAR（3/3），语义闭合。
#include "CDeathMatchBattleMgr/InsertTimerKilledUserForRevive.inc"
}

// ==== ORIG 085df012 ====
// [L10b P292 档①] int→bool（头同步）：ORIG PvP_Room::check_winner 0x085d9950
// `test %al,%al; je` 无 setne。
#include "CDeathMatchBattleMgr/checkGameOverForSinglePlay.inc"
}

// ==== ORIG 085df126 ====
// SIGFIX b28c-L29：ORIG 串 "bool CDeathMatchBattleMgr::checkGameOverForTeamPlay(...)"。
// [b28d-27] DIFF N82：ORIG 0x085df126；p167 CLEAR（1/1），残差为帧/槽与块排布，
// 语义闭合。
#include "CDeathMatchBattleMgr/checkGameOverForTeamPlay.inc"
}

// ==== ORIG 085df48a ====
#include "CDeathMatchBattleMgr/checkWinnerForSinglePlay.inc"
}

// ==== ORIG 085df296 ====
// [b28d-27] DIFF N82/N93：ORIG 0x085df296；p167 CLEAR（4/4），残差为帧/槽与
// 分支块排布，语义闭合。
#include "CDeathMatchBattleMgr/checkWinnerForTeamPlay.inc"
}

// ==== ORIG 085df576（const） ====
// [b28d-27] DIFF N82/N86：ORIG 0x085df576；p167 CLEAR（10/10），残差为 pair 数组
// 临时槽与帧分配，语义闭合。
#include "CDeathMatchBattleMgr/getRanking.inc"
}

// ==== ORIG 085dee78 ====
#include "CDeathMatchBattleMgr/OnJoinRoom.inc"
}

// ==== ORIG 085dee8a ====
#include "CDeathMatchBattleMgr/OnLeaveRoom.inc"
}

// ==== ORIG 085deeae ====
#include "CDeathMatchBattleMgr/OnReady.inc"
}

// ==== ORIG 085dee9c ====
#include "CDeathMatchBattleMgr/OnChangeTeam.inc"
}

// ==== ORIG 085defc6 ====
#include "CDeathMatchBattleMgr/OnFight.inc"
}
