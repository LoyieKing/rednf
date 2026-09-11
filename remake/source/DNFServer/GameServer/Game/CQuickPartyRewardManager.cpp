// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CQuickPartyRewardManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "CQuickPartyRewardManager/process.inc"


#include "CQuickPartyRewardManager/LevelData.inc"

#include "CQuickPartyRewardManager/LevelDataHell.inc"

// ============================ QuickPartyData ============================

#include "CQuickPartyRewardManager/QuickPartyData.inc"


#include "CQuickPartyRewardManager/dtor_QuickPartyData.inc"

#include "CQuickPartyRewardManager/IBasicReward.inc"


#include "CQuickPartyRewardManager/IRandomwReward.inc"


#include "CQuickPartyRewardManager/process_1.inc"


#include "CQuickPartyRewardManager/process_2.inc"


#include "CQuickPartyRewardManager/process_3.inc"


#include "CQuickPartyRewardManager/process_4.inc"


#include "CQuickPartyRewardManager/process_5.inc"

#include "CQuickPartyRewardManager/process_6.inc"


#include "CQuickPartyRewardManager/process_7.inc"



// ---- 具体类 ctor（W，type 仅 RandomwReward 族存储于 +0x08）----

#include "CQuickPartyRewardManager/RewardCardGold.inc"


#include "CQuickPartyRewardManager/HellPartyDifficulty.inc"


#include "CQuickPartyRewardManager/KillMonsterExp.inc"


#include "CQuickPartyRewardManager/EquipItemDropRate.inc"


#include "CQuickPartyRewardManager/CointLimitCount.inc"


#include "CQuickPartyRewardManager/ReturnAncientDungeonTicket.inc"


// ============================ CQuickPartyRewardManager ============================

#include "CQuickPartyRewardManager/CQuickPartyRewardManager.inc"

#include "CQuickPartyRewardManager/dtor_CQuickPartyRewardManager.inc"

#include "CQuickPartyRewardManager/init.inc"


#include "CQuickPartyRewardManager/set_rand_buff_total_prob.inc"

#include "CQuickPartyRewardManager/getRequireQuickPartyMinCount.inc"



#include "CQuickPartyRewardManager/get_random_buff_prob.inc"


}  // namespace QuickParty
