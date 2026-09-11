// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ExpertJobOnlyMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CExpertJob.h"

#include "CBattle_Field.h"     // CBattle_Field::SetAssignNPCByPotion（UseDelilah）
#include "CDataManager.h"      // G_CDataManager / GetExpertJobScript / find_item
#include "CParty.h"            // GetParty/get_user（+0xb24 内嵌战场区）
#include "CPrivateStoreMgr.h"  // private_store 单例 / IsBusyPrivateStore
#include "CStackableItem.h"    // m_int2d0（APC 药水持续秒）
#include "CSystemTime.h"       // CSystemTime::getCurSec
#include "InstanceRentalSystem.h"  // CheckRentableItem（OnEnchantByBead 租赁双查）
#include "GameTypes.h"         // cMyTrace
#include "GlobalData.h"        // s_systemTime_
#include "PacketGuard.h"
#include "SkillSlot.h"      // SkillSlot::delete_skill
#include "GameWorld.h"      // G_GameWorld / send_all

// CExpertJobList.cpp 定义的专家炼金信息表（B 0x094f7778，8 字节；
// [1] 即 ORIG 高半区 _DAT_094f777c）。
extern int g_alchemistInfo[2];

namespace expert_job
{

namespace
{

// 自身扩展数据槽（charac_expand 槽 0）快捷读取。b28d-solo：P49 必须用**宏**——
// -O0 下 static inline helper 会发射真实 call 符号（ORIG 无该符号，属发明符号），
// 且每个使用点须独立重发 GetCharacExpandData 调用（P48，ORIG 逐处 call 实证）。
#define GetSelfExpand(user) \
    reinterpret_cast<CCharacExpertJob*>((user)->GetCharacExpandData( \
        (ENUM_CHARAC_EXPAND_TYPE)0))

}  // namespace

// ===========================================================================
// expert_job::CAlchemist
// ===========================================================================

#include "ExpertJobOnlyMgr/CDollController.inc"


#include "ExpertJobOnlyMgr/UseDelilah.inc"


#include "ExpertJobOnlyMgr/UseRevivalItem.inc"


#include "ExpertJobOnlyMgr/GetQuestItemDropRate.inc"


#include "ExpertJobOnlyMgr/CheckUsableStackableItem.inc"


#include "ExpertJobOnlyMgr/GetAssaultPrevent.inc"


#include "ExpertJobOnlyMgr/OnUseStackableItem.inc"


}  // namespace expert_job

// ===========================================================================
// expert_job::CDisjointer（分解术士三接口）
// ===========================================================================

namespace expert_job
{

#include "ExpertJobOnlyMgr/OnEnterDisjointStore.inc"


#include "ExpertJobOnlyMgr/OnRepairDisjointMachine.inc"


#include "ExpertJobOnlyMgr/OnUpgradeDisjointMachine.inc"


}  // namespace expert_job

// ===========================================================================
// expert_job::CDollController —— 抽象派生（ORIG vtable @0x8cc08e0：4 业务槽
// 全 __cxa_pure_virtual + D1/D0）。ctor 仅基类构造+vptr 置位，dtor 链基类析构，
// 由类定义自然生成；完整类布局建议见 decl 文件。
// ===========================================================================

namespace expert_job
{

#include "ExpertJobOnlyMgr/dtor_CDollController.inc"
{
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CExpertJobMgr
// ===========================================================================

namespace expert_job
{

#include "ExpertJobOnlyMgr/OnLeaveUser.inc"


#include "ExpertJobOnlyMgr/IsAvailableRequestPeer.inc"


#include "ExpertJobOnlyMgr/OnRequestPeer.inc"


#include "ExpertJobOnlyMgr/OnResponsePeer.inc"


#include "ExpertJobOnlyMgr/OnEnchantByBead.inc"


#include "ExpertJobOnlyMgr/OnGiveup.inc"


}  // namespace expert_job

// ===========================================================================
// expert_job::CCharacExpertJob 补充
// ===========================================================================

namespace expert_job
{

#include "ExpertJobOnlyMgr/SendExpertJobInfo.inc"


#include "ExpertJobOnlyMgr/set_disjoint_machine_grade.inc"
{
    alter();
    m_enchantCount = value;   // +0x0c
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CDisjointMachine::include【objdump】
// ORIG W 0x85d3de6 sz 0x20：this+0x04 与 other+0x04 两矩形转调
// collision_detection::Rectangle::include（W 0x85cb492）。
// ===========================================================================

namespace expert_job
{

#include "ExpertJobOnlyMgr/include.inc"


}  // namespace expert_job
