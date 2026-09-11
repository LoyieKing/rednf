// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WongWorkDeathTower_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// （见下方注释）须置于全部 include 前：首个带入 CItemGenerator.h 的链
//   （CDeathTower.h→CParty.h→…）展开时即需这两个名字。
class CMonster;
namespace WongWork { struct stGenerateRefClearReward_t; }

// ============================================================================
// df_game_r 还原 —— WongWork::CDeathTower 嵌套类（G4 批次，WongWorkDeathTower_A）
//
// 本 TU 承载：
//   · CPacketHandler 全部包构造族（ORIG T 0x0845fc6c..0x084605a1）及
//     C1/D1（ORIG W 0x08469a8c / 0x08469aa0，转发 PacketGuard）；
//   · CDungeonMgr C1/C2、D1/D2（ORIG W 0x082a74b0 / 0x082a74b6：空函数）；
//   · CPlayData D1/D2（ORIG W 0x82a7572：空函数）、generateRewardExp
//     （ORIG T 0x08462538）、generateRewardItem（ORIG T 0x08461fc0）；
//   · 生命周期闭环补件：CPlayData C1（W 0x82a7538）、stRewardItem_t C1
//     （W 0x82a7502，逐 Inven_Item() 构造）—— 显式定义以匹配 ORIG 弱符号。
// 类声明见 CDeathTower.h（唯一完整定义点）；CGameManager.h 仅 #include 该头。
// ============================================================================

#include "CDeathTower.h"

#include "CDungeon.h"
#include "CMap.h"

// CItemGenerator.h 自身顺序缺口的前置（其 _generateMonseterSpecificItem 参数用
// CMonster、_generateRandom/SpecificItem 私有签名在自身前置声明行之前引用
// stGenerateRefClearReward_t；完整定义在 CClearReward.cpp，见该头注释）。
class CMonster;
namespace WongWork { struct stGenerateRefClearReward_t; }
#include "CDataManager.h"          // G_CDataManager / GetChannelScript
#include "GlobalData.h"            // GlobalData::s_event_manager
#include "CGuildServerProxy.h"     // class CEventManager / class CEventBase
#include "CBurningFatigueEvent.h"
#include "CExpDoubleEvent.h"
#include "CPCRoomBurningEvent.h"
#include "CServerEvent.h"
#include "CEnvironment.h"          // G_CEnvironment / get_server_group / get_channel_no
#include "CItemGeneratorMgr.h"
#include "CInventory.h"
#include "CDeathTowerRanking.h"
#include "LogManager.h"

namespace {

// [AnonLane2 2026-09-11 匿名轴] 旧 `kFile_DeathTower` 已删：ORIG 全库零命中，
// ORIG 侧仅匿名 .rodata 串，3 处使用点内联同值字面量 `"DeathTower.cpp"`。
// [N3 2026-09-11 P139] ORIG 该 TU 发射局部静态
//   `_ZZN8WongWork11CDeathTower9CPlayData17generateRewardExpEiijP6CPartyRNS0_11CDungeonMgrEE19__PRETTY_FUNCTION__`
//   @0x08c64020（size 0x7f，objdump -s 实读内容 =
//   "void WongWork::CDeathTower::CPlayData::generateRewardExp(int, int, unsigned int,
//    CParty*, WongWork::CDeathTower::CDungeonMgr&)"）；旧形手写 static const char[]
//   串内容相同但**不发射该符号**（计入 missing_in_game）⇒ 改用 __PRETTY_FUNCTION__
//   （串内容由声明拼写决定，与本处签名逐字节一致）。

} // namespace

namespace WongWork {

#include "WongWorkDeathTower_A/init.inc"


#include "WongWorkDeathTower_A/setUseSkillMaterial.inc"


#include "WongWorkDeathTower_A/getUseSkillMaterialTime.inc"


#include "WongWorkDeathTower_A/initMonsterInfo.inc"


#include "WongWorkDeathTower_A/dieMonster.inc"


// ==============================================================================
// 生命周期补件（ORIG W）
// ==============================================================================

#include "WongWorkDeathTower_A/stRewardItem_t.inc"


#include "WongWorkDeathTower_A/CPlayData.inc"


#include "WongWorkDeathTower_A/dtor_CPlayData.inc"


#include "WongWorkDeathTower_A/CDungeonMgr.inc"
#include "WongWorkDeathTower_A/dtor_CDungeonMgr.inc"

#include "WongWorkDeathTower_A/CPacketHandler.inc"
#include "WongWorkDeathTower_A/dtor_CPacketHandler.inc"

// ==============================================================================
// CPacketHandler 包构造族（put_* 为基类 InterfacePacketBuf 直调）
// ==============================================================================

#include "WongWorkDeathTower_A/appendPickupMoney.inc"


#include "WongWorkDeathTower_A/appendPickupItem.inc"


#include "WongWorkDeathTower_A/makeCommonError.inc"


#include "WongWorkDeathTower_A/makeDropItemError.inc"


#include "WongWorkDeathTower_A/makeDropItemNotify.inc"


#include "WongWorkDeathTower_A/makeDropItem.inc"


#include "WongWorkDeathTower_A/makeFinishLoading.inc"


#include "WongWorkDeathTower_A/makeDieState.inc"


#include "WongWorkDeathTower_A/makeDisplayRangking.inc"


#include "WongWorkDeathTower_A/makeDisplayReward.inc"


#include "WongWorkDeathTower_A/makeDisplayEPLP.inc"


#include "WongWorkDeathTower_A/makeUpdateItemList.inc"


#include "WongWorkDeathTower_A/generateRewardExp.inc"


#include "WongWorkDeathTower_A/generateRewardItem.inc"


} // namespace WongWork
