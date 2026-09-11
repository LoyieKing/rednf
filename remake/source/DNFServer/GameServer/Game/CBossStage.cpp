// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBossStage/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CBossStage.h"
#include "WarField.h"   // MapInfo::Clear / map_item / map_monster
#include "GameRand.h"   // get_rand_int

#include <cstring>

#include "CUser.h"                  // CUser/CHackAnalyzer/cUserHistoryLog/ENUM_ERROR
#include "QuickParty.h"         // 先于 CDungeon.h：QUICKPARTY_H 守卫去重
#include "CDungeon.h"               // CDungeon/get_standard_level/RandomList
#include "CMap.h"                   // CMap/_mapMonster/_mapItem 完整类型
#include "CItemGenerator.h"         // WongWork::stGenerateRefData_t/stGenerateResult_t
#include "CGenUniqueNo.h"           // WongWork::CGenUniqueNo::genIPGNo
#include "CParty.h"                 // get_user/checkValidUser/getManager/m_battleField.m_bloodState
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "CDataManager.h"           // find_map/find_item/G_CDataManager
#include "CInventory.h"             // gain_money/delete_item/insertItemIntoInventory
#include "MonsterIndependentDrop.h" // generateIndependentItem/CAICharacter 权威镜像
#include "CMonster.h"               // CMonster::generateIndependentItem
#include "GlobalData.h"             // GlobalData::s_systemTime_
#include "CSystemTime.h"            // getCurTickCount
#include "CSpecialItemRoutingManager.h"  // StartRouting（RoutingPickupItem）
#include "CGameManager.h"               // G_CGameManager
#include "Secu_HackLogCheckByParty.h"   // MSG_MONSTER_DIE 完整定义

namespace WongWork
{

#include "CBossStage/CBossStage.inc"


#include "CBossStage/dtor_CBossStage.inc"


#include "CBossStage/reset.inc"



// ============================================================================
// 新增虚函数（虚表槽 [8]..[11]，紧随 CBossPlay 6 槽之后）
// ============================================================================


#include "CBossStage/handleMoveMap.inc"


#include "CBossStage/handlePickupItem.inc"


#include "CBossStage/handleDropItem.inc"


#include "CBossStage/handleKillMonster.inc"


// ============================================================================
// 非虚方法（逐函数按 docs/class_func_reports/WongWork__CBossStage/ 报告转写）
// 字段速查（均已具名，偏移仅作对照）：
//   m_dungeon(+0x9f0)  m_stageData(MapInfo@+0x9f4；m_mapIndex+0x9f8、
//     m_assignItemIdx+0xa30、m_dropItemCnt+0xa34、m_mapBasisLevel+0xa3c)
//   m_stageCur(+0xae0)  m_stageMax(+0xae4)  m_difficulty(+0xae8)
//   m_expParamA(+0xaec)  基类 CBossPlay::m_useItemCount[4](+0x9c8)
// ============================================================================

#include "CBossStage/moveNextStage.inc"


#include "CBossStage/randomMapIndex.inc"


#include "CBossStage/killMonsterErase.inc"


#include "CBossStage/KillMonsterGetExp.inc"


#include "CBossStage/SendMapInfoPacket.inc"



#include "CBossStage/GoldPickupItem.inc"


#include "CBossStage/RoutingPickupItem.inc"



#include "CBossStage/NormalPickupItem.inc"


#include "CBossStage/RandomPickupItem.inc"


#include "CBossStage/dropItem.inc"


#include "CBossStage/killMonsterDropItem.inc"


#include "CBossStage/consistMap.inc"


#include "CBossStage/RandomConsistMap.inc"


}  // namespace WongWork