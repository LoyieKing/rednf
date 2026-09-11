// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPartyDepsC2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "CPartyDepsC2/increase_ancient_dungeon_ticket.inc"


#include "CPartyDepsC2/isDungeonDifficultyInfromNotice.inc"


#include "CPartyDepsC2/isGuildParty.inc"


#include "CPartyDepsC2/isPremiumGoldCardReward.inc"


#include "CPartyDepsC2/make_blood_dungeon_info.inc"


#include "CPartyDepsC2/make_blood_dungeon_info_blank.inc"


#include "CPartyDepsC2/move_map.inc"


#include "CPartyDepsC2/send_loadingcheck_msg.inc"


#include "CPartyDepsC2/set_random_reward.inc"


#include "CPartyDepsC2/useItem.inc"


#include "CPartyDepsC2/use_skill.inc"

