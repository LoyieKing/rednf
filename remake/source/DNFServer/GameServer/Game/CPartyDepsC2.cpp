// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPartyDepsC2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CPartyDepsC2.cpp —— CParty 第三组独占恢复（2026-08-24，PartyDepsC2）
//
// 覆盖 11 个 ORIG 符号（全部经 docs/class_func_reports/CParty/ 对应报告
// ## 汇编 + ## 反编译 C 完整读取后转写；未命名 thunk 均以 nm 定位为真实 W 符号）：
//   increase_ancient_dungeon_ticket  T 0x0859bc9a（increase_ancient_dungeon_ticket.md）
//   isDungeonDifficultyInfromNotice  T 0x085be8a0（isDungeonDifficultyInfromNotice.md）
//   isGuildParty                     T 0x085b49d0（isGuildParty.md）
//   isPremiumGoldCardReward          T 0x085b11ee（isPremiumGoldCardReward.md）
//   make_blood_dungeon_info          T 0x0859e0d2（make_blood_dungeon_info.md）
//   make_blood_dungeon_info_blank    T 0x0859e480（make_blood_dungeon_info_blank.md）
//   move_map                         T 0x085a8f4e（move_map.md）
//   send_loadingcheck_msg            T 0x085b1fe0（send_loadingcheck_msg.md）
//   set_random_reward                T 0x0859b8fa（set_random_reward.md）
//   useItem                          T 0x085a7800（useItem.md）
//   use_skill                        T 0x085a77d8（use_skill.md）
// ============================================================================

#include <string.h>
#include <vector>

#include "CParty.h"
                 // CGameManager.h 先于 CDungeon.h：其 QuickParty.h 完整定义
                 // 需先于 CDungeon.h 的 CQuickPartySystemManager 兜底声明
#include "CGameManager.h"
#include "PacketGuard.h"
#include "TimerQueue.h"
#include "Secu_HackLogCheckByParty.h"
#include "PacketTypes.h"         // MSG_MOVE_MAP 完整类型
#include "CCommonStatisticsMgr.h" // GetInstanceCommonStatisticsMgr/IncreaseQuantity
#include "CDungeon.h"
#include "CDataManager.h"
#include "CInventory.h"         // insertItemIntoInventory 完整类型
#include "CWorldMap.h"
#include "GameWorld.h"
#include "CBloodDungeonRewardFirstEvent.h"   // GetGold W 0x085bff1a（真实类，本批次闭环）
#include "CBloodDungeonRewardSecondEvent.h"  // GetGold W 0x085bff26
 #include "CGuildServerProxy.h"   // CEventManager / CEventBase（IsEventing vptr+0x34 槽13）
 #include "GlobalData.h"
 #include "CMailBoxHelper.h"
 #include "RDARScriptStringManager.h"
 #include "CBattle_Field.h"
#include "W9_EventSmall.h"       // CCharacBloodDungeon（GetEnterCount/GetWeeklyEnterCount）

// [b28d-59] ORIG 0x085b2002/0x085b2009 以内存读引用 _ZL37CONST_SINGLE_MAP_LOADING_
// WAIT_TIMEOUT / _ZL36CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT（非立即数）⇒
// static volatile const int 阻止 4.4 常量折叠（同 CDeathTower.cpp 0x8467122 先例）。
static volatile const int CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT = 0x78;
static volatile const int CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT = 0x5a;

// ---------------------------------------------------------------------------
// increase_ancient_dungeon_ticket —— ORIG T 0x0859bc9a
// 报告：docs/class_func_reports/CParty/increase_ancient_dungeon_ticket.md
// ---------------------------------------------------------------------------
#include "CPartyDepsC2/increase_ancient_dungeon_ticket.inc"
}

// ---------------------------------------------------------------------------
// isDungeonDifficultyInfromNotice —— ORIG T 0x085be8a0
// 报告：docs/class_func_reports/CParty/isDungeonDifficultyInfromNotice.md
// ---------------------------------------------------------------------------
#include "CPartyDepsC2/isDungeonDifficultyInfromNotice.inc"
}

// ---------------------------------------------------------------------------
// isGuildParty —— ORIG T 0x085b49d0
// 报告：docs/class_func_reports/CParty/isGuildParty.md
// ---------------------------------------------------------------------------
#include "CPartyDepsC2/isGuildParty.inc"
}

// ---------------------------------------------------------------------------
// isPremiumGoldCardReward —— ORIG T 0x085b11ee
// 报告：docs/class_func_reports/CParty/isPremiumGoldCardReward.md
//   数据经 m_serverParameter(+0x68) m_premiumGoldCard4894(+0x4894) m_conds[i]
//   （+0x08 起，步长 0x10）：count=m_conditionCount（short @+0x04，0x4900），
//   元素 min=max=prob=reward 与原 ORIG 平铺 int 偏移 0x4904/+0x4910 一致。
// ---------------------------------------------------------------------------
#include "CPartyDepsC2/isPremiumGoldCardReward.inc"
}

// ---------------------------------------------------------------------------
// make_blood_dungeon_info —— ORIG T 0x0859e0d2
// 报告：docs/class_func_reports/CParty/make_blood_dungeon_info.md
// ---------------------------------------------------------------------------
#include "CPartyDepsC2/make_blood_dungeon_info.inc"
}

// ---------------------------------------------------------------------------
// make_blood_dungeon_info_blank —— ORIG T 0x0859e480
// 报告：docs/class_func_reports/CParty/make_blood_dungeon_info_blank.md
// ---------------------------------------------------------------------------
#include "CPartyDepsC2/make_blood_dungeon_info_blank.inc"
}

// ---------------------------------------------------------------------------
// move_map —— ORIG T 0x085a8f4e
// 报告：docs/class_func_reports/CParty/move_map.md
// ---------------------------------------------------------------------------
#include "CPartyDepsC2/move_map.inc"
}

// ---------------------------------------------------------------------------
// send_loadingcheck_msg —— ORIG T 0x085b1fe0
// 报告：docs/class_func_reports/CParty/send_loadingcheck_msg.md
// 常量沿用 ORIG 局部静态实证值（同 source/Community/CommonConstants.h）：
//   CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT=0x78、CONST_PARTY_...=0x5a
// ---------------------------------------------------------------------------
#include "CPartyDepsC2/send_loadingcheck_msg.inc"
}

// ---------------------------------------------------------------------------
// set_random_reward —— ORIG T 0x0859b8fa
// 报告：docs/class_func_reports/CParty/set_random_reward.md
// ---------------------------------------------------------------------------
#include "CPartyDepsC2/set_random_reward.inc"
}

// ---------------------------------------------------------------------------
// useItem —— ORIG T 0x085a7800
// 报告：docs/class_func_reports/CParty/useItem.md
// ---------------------------------------------------------------------------
#include "CPartyDepsC2/useItem.inc"
}

// ---------------------------------------------------------------------------
// use_skill —— ORIG T 0x085a77d8
// 报告：docs/class_func_reports/CParty/use_skill.md
// ---------------------------------------------------------------------------
#include "CPartyDepsC2/use_skill.inc"
}
