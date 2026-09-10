// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CQuickPartyRewardManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— QuickParty::CQuickPartyRewardManager 独立 TU。
// 权威符号（ORIG df_game_r nm 实证，地址见各函数）：
//   CQuickPartyRewardManager C1 0x0826d688 T / D1 0x0826d69c T
//   init                    0x0826d6b0 T
//   set_rand_buff_total_prob 0x0826d91a T
//   IRewardType 族 ctor/check W 与六个 process T —— 逐报告转写。
// 类布局与 vtable 链证据见 CQuickPartyRewardManager.h 头注。

#include "CQuickPartyRewardManager.h"

#include "GameTypes.h"   // cMyTrace
#include "CParty.h"      // get_member_count / increase_dungeon_coin_count /
                         // increase_ancient_dungeon_ticket

#include <cmath>
#include <new>

namespace QuickParty
{

// ---- RewardCardGold（T 0x0826d070）----
// result.m_value00 *= (levels[成员数-2].m_value4 / 100.0f + 1.0f)，截断取整。
// ORIG 反汇编：fnstcw 置 RC=0 + fistpl（截断）；lrintf 为就近偶舍入，语义不符。
#include "CQuickPartyRewardManager/process.inc"
}

// ---- LevelData / LevelDataHell ctor（ORIG W 0x08a6ef72 / 0x08a6ef8a：
// LevelData：m_field0=0、m_value4=-1；LevelDataHell：两 int 全置 -1；
// dtor 空）。此处强定义供 EventRewardQuickPartyB2 等 TU 以外部引用解析
// （W 与 T 链接可共存，语义一致）。----
#include "CQuickPartyRewardManager/LevelData.inc"

#include "CQuickPartyRewardManager/LevelDataHell.inc"
#include "CQuickPartyRewardManager/QuickPartyData.inc"
}

// ORIG 0x826dd52：逆序析构 map → BasicReward → 两 vector<int>（成员自动）。
#include "CQuickPartyRewardManager/dtor_QuickPartyData.inc"

#include "CQuickPartyRewardManager/IBasicReward.inc"
}

#include "CQuickPartyRewardManager/IRandomwReward.inc"
}

// ---- HellPartyDifficulty（T 0x0826d116）----
// 命中槽位：result.value00=hell[idx].value4、result.value04=hell[idx].field0（交叉写）。
#include "CQuickPartyRewardManager/process_1.inc"
}

// ---- KillMonsterExp（T 0x0826d198）----
// map[m_type] 命中后按等级档放大 result.m_value00（同就近偶舍入）。
#include "CQuickPartyRewardManager/process_2.inc"
}

// ---- EquipItemDropRate（T 0x0826d2aa）----
// 命中后直接写 result.m_rate14 浮点比率（无舍入）。
#include "CQuickPartyRewardManager/process_3.inc"
}

// ---- CointLimitCount（T 0x0826d4ac）----
// result.m_value00 -= levels[idx].m_value4；差额经 CParty 计入地下城硬币计数。
#include "CQuickPartyRewardManager/process_4.inc"
}

// ---- ReturnAncientDungeonTicket（T 0x0826d598）----
// 仅调 CParty::increase_ancient_dungeon_ticket（不触 result/m_data）。
#include "CQuickPartyRewardManager/process_5.inc"
}
// ---- QuestItemDropRate（T 0x0826d398）----
// map[m_type] 命中且 (成员数-2) 为有效档位时，result.m_flag18 按
// flag * (1 + levels[idx].m_value4 / 100.0f) 就近偶舍入放大。
#include "CQuickPartyRewardManager/process_6.inc"
}

// ---- DimensionInoutCount（T 0x0826d5ac）----
// 命中档位后把 levels[idx].m_value4 写入 result.m_value00，
// 并将该值计入 party 的次元进出计数。
#include "CQuickPartyRewardManager/process_7.inc"
}


// ---- 具体类 ctor（W，type 仅 RandomwReward 族存储于 +0x08）----

#include "CQuickPartyRewardManager/RewardCardGold.inc"
}

#include "CQuickPartyRewardManager/HellPartyDifficulty.inc"
}

#include "CQuickPartyRewardManager/KillMonsterExp.inc"
}

#include "CQuickPartyRewardManager/EquipItemDropRate.inc"
}

#include "CQuickPartyRewardManager/CointLimitCount.inc"
}

#include "CQuickPartyRewardManager/ReturnAncientDungeonTicket.inc"
}

// ============================ CQuickPartyRewardManager ============================

#include "CQuickPartyRewardManager/CQuickPartyRewardManager.inc"

#include "CQuickPartyRewardManager/dtor_CQuickPartyRewardManager.inc"
#include "CQuickPartyRewardManager/init.inc"
}

// ORIG 0x0826d91a：按列求和后写回 m_probs[row][col]——row 声明在外层、循环后泄漏
// 值 11（ORIG 写回段载 -0x8 运行时形 shl3/sub/add，非 0x61 常量折叠，P171 照抄）。
#include "CQuickPartyRewardManager/set_rand_buff_total_prob.inc"
}
// ORIG T 0x0826d8a2（报告 getRequireQuickPartyMinCount.md）：
// type∈[0,1]、diff∈[0,3] 越界返回 -1；命中查 m_minCounts[type][diff]
// （寻址 +0x1a0 + type*0x10 + diff*4）。
#include "CQuickPartyRewardManager/getRequireQuickPartyMinCount.inc"
}


// ORIG T 0x0826d860（报告 get_random_buff_prob.md）：type∈[0,11] 且
// dungeon∈[0,6] 时返回 m_probs[type][dungeon]，否则返回 0。
// ORIG 仅上界比较（cmp $0xb/jg、cmp $0x6/jle），负值与原版一致落入查表路径。
#include "CQuickPartyRewardManager/get_random_buff_prob.inc"
}

}  // namespace QuickParty
