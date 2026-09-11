// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPartyDepsB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CPartyDepsB2.cpp —— CParty 第二组（独占 TU，2026-08-24）。
// 逐函数依据 docs/class_func_reports/CParty/ 完整报告（## 汇编 + ## 反编译 C）：
//   _Send_TOD_APCInfo.md            ORIG T 0x085bac26
//   _onGetItem.md                   ORIG T 0x085b949c
//   checkDungeonDifficulty.md       ORIG T 0x085be932
//   checkInoutConditionDungeon.md   ORIG T 0x085abc80（三参重载；无参 0x85ac59c 在 CParty.cpp）
//   choose_random_buff.md           ORIG T 0x0859b814
//   die_user.md                     ORIG T 0x085a7828
//   finish_loading.md               ORIG T 0x085b15e0
//   getMemberMaxLevelBeforeDungeon.md ORIG T 0x085b613e
//   get_party_seatno.md             ORIG T 0x085a277a
//   giveup_game.md                  ORIG T 0x085b2baa
// 字段全部使用 CParty.h 具名成员；控制流按 ORIG 汇编形态逐段转写：
// 顺序守卫代替 && 链、单出口析构点、比较树/短路次序一一对应。
// 依赖的跨域叶子实现随本批次落地：
//   QuickParty::CQuickPartyRewardManager::get_random_buff_prob (0x0826d860)
//   EventClassify::CEventScriptMng::process_add_item_count_db  (0x0810bea8)
//   CSpecialItemRoutingManager::ProcessGiveUp                  (0x0860b2c2)
//   TOD_Layer::TOD_Layer / TowerOfDespairMgr::SendAPCInfo      (TowerOfDespairMgr.cpp)

#include <string.h>

#include <algorithm>

#include "GameRand.h"                   // get_rand_int
#include "QuickParty.h"                 // 需先于 CParty.h（守卫次序，同 CParty.cpp）
#include "CParty.h"
#include "CDungeon.h"
#include "BattleData.h"   // finish_loading 装载标志 m_arr7e4（CParty +0xb10）
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "LogManager.h"                 // logFormat
#include "HistoryLog.h"                 // WriteDie / WriteGiveUp
#include "CSystemTime.h"                // GlobalData::s_systemTime_ / getCurSec
#include "Secu_HackLogCheckByParty.h"   // dungeonFail
#include "CDataManager.h"               // G_CDataManager / find_item / get_event_script_mng
#include "CGameManager.h"               // G_CGameManager / GetQuickPartyRewardManager /
                                        // GetSpecialItemRoutingManager
#include "CBattle_Field.h"              // GetDungeon / IsFirstEnterDungeon / check_start_point /
                                        // get_dungeon_diff / m_villageMonsterId / m_battleField.m_dungeonClearTracer
#include "GameWorld.h"                  // G_GameWorld / out_from_dungeon / send_all
#include "CUser.h"                      // Send/make_basic_info/getHackAnalyzer/getMoveSpace/...
#include "CUserCharacInfo.h"            // getCurCharacStamina/getCurCharacNo/... （经 CUser 继承）
#include "CDungeonEntranceLog.h"        // GetInstanceDungeonEntranceLog
#include "GlobalData.h"                 // s_pTowerOfDespairMgr / s_systemTime_
#include "CEventScriptMng.h"            // EventClassify::CEventScriptMng
#include "Statistics.h"                 // Statistics::DungeonFail（static ABI）
#include "CSpecialItemRoutingManager.h" // ProcessGiveUp
#include "TowerOfDespairMgr.h"          // TOD_Layer / TowerOfDespairMgr

#include "CPartyDepsB2/get_party_seatno.inc"


#include "CPartyDepsB2/getMemberMaxLevelBeforeDungeon.inc"


#include "CPartyDepsB2/die_user.inc"


#include "CPartyDepsB2/finish_loading.inc"


#include "CPartyDepsB2/checkDungeonDifficulty.inc"


#include "CPartyDepsB2/_Send_TOD_APCInfo.inc"


#include "CPartyDepsB2/_onGetItem.inc"


#include "CPartyDepsB2/choose_random_buff.inc"


#include "CPartyDepsB2/checkInoutConditionDungeon.inc"


#include "CPartyDepsB2/giveup_game.inc"

