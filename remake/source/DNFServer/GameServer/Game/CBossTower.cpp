// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBossTower/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ WongWork::CBossTower 实现（CBossTowerRetry 批次）。
// 全部函数按 docs/class_func_reports/WongWork__CBossTower/ 对应 md 的
// 汇编 + 反编译 C 转写。裸偏移已字段化：
//   基类段（CBossStage，protected）：+0x9e8→m_startTick、+0x9ec→m_endTick、
//     +0x9f0→m_dungeon、+0xa00/+0xa14→m_stageData.m_monsterMap(_M_node_count)、
//     +0xae0→m_stageCur、+0xae4→m_stageMax、+0xae8→m_difficulty；
//   CBossTower 自有段：+0xaf4→m_towerStartTick、+0xaf8→m_towerEndTick、
//     +0xafc→m_prepareFinishFlag、+0xb00→m_dungeonLog；
//   跨类：party+0xcd8→CParty::m_battleField.m_bloodState、user+0x79700→CUser::m_historyLog、
//     manager+0x796cc→CUser::m_gmBossDifficulty。
// getIdx 0x082aa728 读 +0xaf0；setIdx 0x082aa716 写 +0xaf0。
// 虚表 _ZTVN8WongWork10CBossTowerE@0x08b70260 共 11 槽（vptr 相对偏移：
// +0x08=_checkMemberDie、+0x0c=checkAllMemberReady、+0x14=handleLeaveUser）。
#include "CBossTower.h"

#include <algorithm>   // std::min（generateRewardItem 上限链）
#include <cstring>

#include "CParty.h"     // get_user/get_member_count/getMemberNames/m_battleField.m_bloodState(+0xcd8)
#include "CUser.h"      // SendCmdErrorPacket/m_historyLog(+0x79700)/m_gmBossDifficulty
#include "QuickParty.h" // 先于 CDungeon.h：QUICKPARTY_H 已定义则跳过其镜像块
#include "CDungeon.h"   // GetDungeonName/getRandomTowerMaxStage/get_min_level 等
#include "CInventory.h" // gain_money/insertItemIntoInventory/MakeItemPacket/
                        // GetInvenTypeFromItemSpace/eMoneyAddReason/eItemAddReason
#include "CEquipItem.h" // get_endurance（InsertRewardItems 邮件分支）
#include "CGenUniqueNo.h"            // WongWork::CGenUniqueNo::genIPGNo
#include "CMailBoxHelper.h"          // ReqDBSendNewSystemMail/ReqDBSendNewAvatarMail
#include "RDARScriptStringManager.h" // g_scriptStringManager_（邮件文案）

#include "GlobalData.h"      // s_systemTime_
#include "CSystemTime.h"     // getCurTickCount / getCurSec
#include "TimerQueue.h"      // G_TimerQueue / TimerEntry::OBJ_TYPE / TIMER_MESSAGE
#include "CGameManager.h"    // G_CGameManager()->returnBossTower
#include "cHistoryTrace.h"   // 权威声明（原局部最小声明已删除）
#include "PacketGuard.h"        // PacketGuard（回包构造）
#include "CDungeonEntranceLog.h" // GetInstanceDungeonEntranceLog / IncrementDungeonEntrance
#include "CDataManager.h"       // find_dungeon/find_item/get_mob_reward/GetTowerScript
#include "GameTypes.h"          // cMyTrace


namespace WongWork
{
#include "CBossTower/CBossTower.inc"


#include "CBossTower/dtor_CBossTower.inc"


#include "CBossTower/getIdx.inc"


#include "CBossTower/setIdx.inc"


// ============================================================================
// 虚函数实现（WongWork::CBossTower）
// ============================================================================

#include "CBossTower/handleDieUser.inc"


#include "CBossTower/handleLeaveUser.inc"


#include "CBossTower/handleMoveMap.inc"


#include "CBossTower/handleDropItem.inc"


#include "CBossTower/handleKillMonster.inc"


#include "CBossTower/handlePickupItem.inc"


// ============================================================================
// 生命周期方法
// ============================================================================

#include "CBossTower/onFinishStage.inc"


#include "CBossTower/_onPrepareFinishTower.inc"


#include "CBossTower/_destroy.inc"


#include "CBossTower/InitBossTower.inc"


#include "CBossTower/checkStartGameCondition.inc"


#include "CBossTower/handleStartGame.inc"


#include "CBossTower/handleUseItem.inc"


#include "CBossTower/handleFinishLoading.inc"


#include "CBossTower/_onTimerProcess.inc"


#include "CBossTower/handleEPLPCommand.inc"


#include "CBossTower/SendRewardPacket.inc"


#include "CBossTower/SendELPLPacket.inc"


#include "CBossTower/InsertRewardExp.inc"


#include "CBossTower/InsertRewardItems.inc"


#include "CBossTower/onClearBossTower.inc"


#include "CBossTower/generateRewardExp.inc"


#include "CBossTower/generateRewardItem.inc"


#include "CBossTower/onChargeRequireItem.inc"


#include "CBossTower/onReadyTimerCheck.inc"


#include "CBossTower/onReturnVillage.inc"


}  // namespace WongWork
