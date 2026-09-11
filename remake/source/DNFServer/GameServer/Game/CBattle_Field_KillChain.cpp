// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBattle_Field_KillChain/<Func>.inc（不参与编译、内部不含 #include）。
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

#include <list>
#include <map>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h         // CBattle_Field/MapInfo/map_item/map_monster
#include "CDungeon.h"              // get_dimension_possible/get_standard_level/…
#include "CDataManager.h"          // G_CDataManager/find_item/find_monster
#include "CGuildServerProxy.h"    // CEventBase / CEventManager::GetRepeatEvent
                                   //   CItemGenerator/CItemGeneratorMgr/CLuckPoint
#include "CItemGeneratorMgr.h"
#include "CSystemTime.h"           // CSystemTime::getCurSec
#include "GlobalData.h"            // s_systemTime_/s_event_manager
#include "LogManager.h"            // LogManager::logFormat
#include "InterfacePacketBuf.h"    // PacketGuard 打包接口
#include "Inven_Item.h"
#include "CEquipItem.h"            // getAvatarPeriod/getAvatarSocket
#include "CItemAmplifier.h"        // stAmplifyOption_t::getAbilityType/Value
#include "GamePODTypes.h"          // stAvatarEmblemInfo_t
#include "BlueMarble.h"            // BlueMarble::getBuff
#include "PvPTypes.h"              // BlueMarbleBuffType
#include "CUserCharacInfo.h"       // get_charac_job/get_charac_level
#include "TimerQueue.h"            // G_TimerQueue/InsertTimer/TIMER_MESSAGE

// CEventBase 虚槽 +0x34 = IsEventing(CUser*)（ORIG _MakeDropItems vcall 实证）。

namespace
{

// [AnonLane2 2026-09-11 匿名轴] 曾在此的 TU 级手写 prett 串
// `s_bfkc_pretty_kill_hell` / `s_bfkc_pretty_check_blood` 已删：两处使用点早在
// P139 改造中改回 __PRETTY_FUNCTION__（ORIG 局部静态
// `_ZZN13CBattle_Field28KillHellPartyGroupMonsterCntE…E19__PRETTY_FUNCTION__`
// @0x8c233a0 size 0x50），旧数组沦为死数据但仍发射（-fkeep-static-consts 默认开）
// ⇒ 删除；ORIG 全库对两名零命中（.o 级 nm 复核 0）。
// G_CDataManager()->getBlueMarbleScript() 返回 void*；ORIG 0x0830b6fa/0x0830b70d
// 为单条 `mov 0x68(%eax)`/`mov 0x6c(%eax)` disp 读（float 系数）。取址算术
// （char*+0x68）在 4.4 恒多一条 add ⇒ 类型化视图（P58，trivial、零符号发射）。
struct BfkcBlueMarbleScriptView
{
    char m_pad00[0x68];
    float m_f1;      // +0x68
    float m_f2;      // +0x6c
};

}  // namespace

#include "CBattle_Field_KillChain/KillHellPartyGroupMonsterCnt.inc"


#include "CBattle_Field_KillChain/checkKilledSpawnBloodMonsters.inc"


#include "CBattle_Field_KillChain/send_elevator_time_packet.inc"


#include "CBattle_Field_KillChain/getItemDropDungeonType.inc"


#include "CBattle_Field_KillChain/setPremiumDropRate.inc"


#include "CBattle_Field_KillChain/_MakeNotiPacketDieMonster.inc"


#include "CBattle_Field_KillChain/_MakeDropItems.inc"

