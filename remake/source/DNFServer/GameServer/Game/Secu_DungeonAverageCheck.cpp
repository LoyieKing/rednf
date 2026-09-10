// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Secu_DungeonAverageCheck/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— Secu_DungeonAverageCheck（G2 安全域）
// 声明点：Secu_HackLogCheckByParty.h（唯一声明点；Secu_HackLogCheckByParty :
// Secu_DungeonAverageCheck 为 CParty 子对象 +0x1abc）。
// 语义依据 ORIG setKillMonsterInfo 0x82794d6（逐指令转写，含 ROUND=截断浮点、
// 系数 0.1/0.5/0.05 来自 .rodata 0x8c02770/74/78）。
// ============================================================================
#include "Secu_HackLogCheckByParty.h"

#include <string.h>
#include <algorithm>   // std::max（ORIG 0x08279a12 调 _ZSt3maxIiERKT_S2_S2_ ×2）

#include "CParty.h"
#include "CUser.h"
#include "WarField.h"          // map_monster（m_roleType +0x08 / m_mobIndex +0x0c）
#include "CGameManager.h"      // WongWork::CHackAnalyzer::addServerHackCnt 声明

#include "Secu_DungeonAverageCheck/reset.inc"
}

#include "Secu_DungeonAverageCheck/Secu_DungeonAverageCheck.inc"
}

#include "Secu_DungeonAverageCheck/dtor_Secu_DungeonAverageCheck.inc"
}

#include "Secu_DungeonAverageCheck/SetPartyPtr.inc"
}

#include "Secu_DungeonAverageCheck/startDungeon.inc"
}

// [SweepB] ORIG 0x0827993c：循环体内命中即 return it（sret 拷贝），未命中末尾
// 再求一次 end() 返回（P167 MISSING vector::end×1 的根因，非 break 形）。
std::vector<SecKillMonsterInfo>::iterator
#include "Secu_DungeonAverageCheck/findKillMonsterInfo.inc"
}

#include "Secu_DungeonAverageCheck/setKillMonsterInfo.inc"
}

// ORIG T 0x08279a12：遍历 m_killInfos；每项按
//   limit = ROUND(0.5 + 0.7*m_count) - 4（rodata 0x8c02788=0.5 / 0x8c02780=0.7）
//   计算超限值 m_hackCntE6/0xE7 - max(limit,0)；任一 >0 时对 4 人队伍逐成员
//   上报 SetETC(0xe8/0xe9, job*100+grow1*10+grow2) 与
//   CHackAnalyzer::addServerHackCnt(user, 0xe8/0xe9, 超限值, SetETC 返回值, 0)。
#include "Secu_DungeonAverageCheck/CheckHackTypeEndDungeon.inc"
}

#include "Secu_DungeonAverageCheck/dungeonClear.inc"
}

#include "Secu_DungeonAverageCheck/dungeonFail.inc"
}
