#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPartyLink2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <algorithm>
#include <string.h>

#include "QuickParty.h"       // 必须先于 CDungeon.h/CParty.h（守卫惯例，与 CParty.cpp 同）
#include "CParty.h"
#include "CDataManager.h"     // m_rankSystem / m_aiCharacterList / CAICharacterList
#include "MonsterIndependentDrop.h"  // CAICharacter 完整镜像（getPvPWinPoint 真实调用）
#include "CUser.h"            // CUser/CUserCharacInfo/CHackAnalyzer/cUserHistoryLog
#include "CDungeon.h"         // CDungeon::IsEnterEachMap / get_exp_weight / get_index
#include "CMap.h"             // CMap 完整类型（GetCurrentMap->m_f0）
#include "WarField.h"         // map_monster 完整定义
#include "CBattle_Field.h"    // CBattle_Field 完整类型（bf 子对象视图）
#include "CBattle_Field_shared.h"  // STKillMonsterData
#include "BattleData.h"       // BattleData 完整类型（bd 子对象视图）
#include "CExpandEquipslot.h"  // GetCharacExpandData 返回类型完整化（上行 CData）
#include "ActiveStaticInfo.h" // ActiveStaticInfo::getHpMax
#include "GameRand.h"         // get_rand_int
#include "LogManager.h"       // cMyTrace
#include "GlobalData.h"       // s_event_manager
#include "CGuildServerProxy.h"// CEventManager::GetRepeatEvent / CEventBase
#include "ServiceRestrictManager.h"  // ServiceRestrictManager / RestrictCategory
#include "Arad_DataManager.h" // ARAD::Singleton<T> 权威模板
#include "BlueMarble.h"       // BlueMarble::getBuff
#include "PvPTypes.h"         // BlueMarbleBuffType::T
#include "BestClearTime.h"    // BestClearTime::setBestClearTime
#include "CSecretShop.h"      // secretshop::CSecretShop::SendSecretShopItemList
#include "SECRET_SHOP_DATA.h" // secretshop::SALE_INFO
#include "HistoryLog.h"       // HistoryLog::WriteKillMob
#include "Secu_HackLogCheckByParty.h"  // Secu_HackLogCheckByParty::DieMob / MSG_MONSTER_DIE
#include "InterfacePacketBuf.h"
#include "GameEnums.h"        // ENUM_DUNGEON_CLEAR_CONDITION_TYPE
#include "CPartyExpBonusEvent.h"   // CPartyExpBonusEvent::getBonusRate（ORIG W 0x85bfe1a）

namespace
{

// 子对象视图（与 CParty.cpp / CPartyDepsD2.cpp 同惯例）。
// P49：-O0 下 inline 助手仍发射真实 call 符号（p167 EXTRA
// _ZN12_GLOBAL__N_1{bf,bd,traceHack}EP6CParty），而 ORIG 均为内联子对象访问
// ⇒ 改宏：零符号、逐点展开，语义不变。
#define LINK2_BF(p)        ((p)->m_battleField)
#define LINK2_BD(p)        ((p)->m_battleData)
// [b28d-solo-CParty2] CParty.h 字段化：+0x210 区为具名成员 m_traceMobDieHack。
#define LINK2_TRACEHACK(p) ((p)->m_traceMobDieHack)

}  // namespace

#include "CPartyLink2/convertAPCRoleType2MonsterRoleType.inc"


#include "CPartyLink2/setBestClearTime.inc"


#include "CPartyLink2/SendSecretShopItemList.inc"


#include "CPartyLink2/kill_monster.inc"

