// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ExpertJobLink4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "ExpertJobLink4/getItemSellPrice.inc"
                     bool isRandomOption);

#include "CExpertJob.h"

#include <math.h>

#include "CDataManager.h"      // G_CDataManager / GetExpertJobScript(int) /
                               // GetExpertJobEtcScript
#include "CExpertJobList.h"    // STExpertJobEtcScript
#include "CItem.h"             // GetSellPrice/get_rarity/get_index/get_grade/
                               // IsRandomOption
#include "CItemAmplifier.h"    // stAmplifyOption_t::isIdentified
#include "CPrivateStoreMgr.h"  // private_store::{GetInstancePrivateStoreMgr,
                               // CPrivateStoreMgr::IsBusyPrivateStore}
#include "CUser.h"
#include "DisJoint.h"          // DisJointResult
#include "CStackableItem.h"       // m_int324(+0x324) 拆解券等级门槛
#include "DisPatcherCore.h"       // DisPatcher_DisJointItem（静态成员定义）
#include "GameRand.h"          // get_rand_int
#include "GameWorld.h"         // G_GameWorld 及各查询/移动接口
#include "GameTypes.h"         // cMyTrace
#include "GlobalData.h"        // GlobalData::s_villageObjectMgr
#include "Inven_Item.h"        // Inven_Item::m_amp
#include "PacketGuard.h"

#include "ExpertJobLink4/CAlchemist.inc"



#include "ExpertJobLink4/stItemSelectRate.inc"
{
    m_itemIndex = 0;
    m_rate      = 0;
    m_float8    = 0;
    m_rateMin   = 0;
    m_rateMax   = 0;
}

#include "ExpertJobLink4/DisjointMachineGrade.inc"
                                           unsigned char c)
{
    m_grade  = b;
    m_field1 = a;
    m_field2 = c;
}

#include "ExpertJobLink4/DisjointMachineGrade_1.inc"
{
    m_grade  = 0;
    m_field1 = 0;
    m_field2 = 0;
}

namespace expert_job
{

// ---------------------------------------------------------------------------
// 辅助：ORIG 浮点常量（.rodata 实测）
// ORIG 所有浮点→整型转换均显式置 FPU 控制字 RC=最近偶数（fldcw 0xc<<8，
// 0x85d246f flds+fnstcw+fistpl 实证），即 x87 就近取整（round-to-nearest-even，
// x.5 边界取偶）；以 (int)value 直接转换由编译器生成同一 fistpl 序列。
// 注意不可用 +0.5f 截断：2.5 → RC=even 得 2，+0.5 截断得 3。
// ---------------------------------------------------------------------------
// [b28d-solo-ExpertJobLink4 P49]：static inline helper 在 -O0 下仍发真实 call
// 符号（p167 EXTRA `_ZN10expert_job12_GLOBAL__N_113round_nearestEf` ×6，
// ORIG 全内联展开 fldcw/fistpl）⇒ 改宏形。
#define round_nearest(v) ((int)(v))


#include "ExpertJobLink4/get_additional_item.inc"


#include "ExpertJobLink4/get_disjoint_machine_endurance.inc"


#include "ExpertJobLink4/get_disjoint_machine_grade.inc"


#include "ExpertJobLink4/set_disjoint_machine_endurance.inc"


#include "ExpertJobLink4/dtor_CAlchemist.inc"


#include "ExpertJobLink4/OnTeleportCharacter.inc"


#include "ExpertJobLink4/GetScript.inc"


#include "ExpertJobLink4/GetEtcScript.inc"


#include "ExpertJobLink4/CDisjointMachine.inc"


// ---- CDisjointMachine::operator=（ORIG W 0x085d3ed6：基类 IObject::operator=
//      空操作 + 拷 +0x04..+0x14 与 string/+0x1c/+0x20/+0x24/+0x28，不拷 vptr）----
CDisjointMachine& CDisjointMachine::operator=(CDisjointMachine const& other)
{
    village_object::IObject::operator=(other);   // ORIG W 0x85ccadc 同款空操作
    // b28：结构整体赋值（探针 /tmp/l3/p6.cpp 实证 4.4 出 ORIG 的 dest/src 双缓存
    // 4-dword 连发形：mov 0x4(%edx),%ecx…末条 %edx 自复用，逐字段形则 16 条异形）。
    m_rect = other.m_rect;
    m_user           = other.m_user;
    m_name           = other.m_name;
    m_field1c        = other.m_field1c;
    m_field20        = other.m_field20;
    m_field24        = other.m_field24;
    m_field28        = other.m_field28;
    return *this;
}

#include "ExpertJobLink4/OnCreateDisjointStore.inc"


#include "ExpertJobLink4/get_disjoint_result_.inc"


#include "ExpertJobLink4/getItemSellPrice_1.inc"
                     bool isRandomOption);

#include "ExpertJobLink4/AcquireExpertJob.inc"


#include "ExpertJobLink4/AcquireExpertJob_1.inc"


#include "ExpertJobLink4/FreeExpertJob.inc"


#include "ExpertJobLink4/FreeExpertJob_1.inc"


#include "ExpertJobLink4/get_disjoint_machine_grade_1.inc"


#include "ExpertJobLink4/isMine.inc"


#include "ExpertJobLink4/get_disjoint_cost.inc"


#include "ExpertJobLink4/get_cost.inc"
#include "ExpertJobLink4/get_user.inc"
#include "ExpertJobLink4/get_grade.inc"

// ---------------------------------------------------------------------------
// CDisjointer —— LinkMisc5 收口（vtable 重排后按虚槽落地）
// ---------------------------------------------------------------------------

#include "ExpertJobLink4/send_error.inc"


#include "ExpertJobLink4/check_disjoint_endurance.inc"


#include "ExpertJobLink4/IsAvailableJob.inc"

#include "ExpertJobLink4/Reset.inc"

#include "ExpertJobLink4/OnStartMission.inc"


#include "ExpertJobLink4/OnCloseMission.inc"


#include "ExpertJobLink4/complete_disjoint.inc"


#include "ExpertJobLink4/get_disjoint_result.inc"


// ---------------------------------------------------------------------------

#include "ExpertJobLink4/IsAvailableJob_1.inc"


#include "ExpertJobLink4/Reset_1.inc"


#include "ExpertJobLink4/OnStartMission_1.inc"


#include "ExpertJobLink4/OnCloseMission_1.inc"


#include "ExpertJobLink4/GetPredictResult.inc"


}  // namespace expert_job

// ===========================================================================
// 分解链全局校验函数（LinkMisc5 收口；调用方 DispDebugJoint4.cpp 以 mangled
// 直连，符号与 ORIG 一致）
// ===========================================================================

#include "Arad_DataManager.h"      // ARAD::Singleton<T>
#include "ServiceRestrictManager.h"

// GetPredictDisjointItems（报告 _lt_global_gt_/GetPredictDisjointItems.md，
// ORIG T 0x081f8f8d sret）：无专家职业走 DisJoint 表静态结果向量，
// 否则走分解机实例预测。
std::vector<int>* GetPredictDisjointItems(CUser* user, CItem* item,
                                          Inven_Item& invenItem,
                                          expert_job::CDisjointer* disjointer)
{
    if (disjointer != 0)
        return disjointer->GetPredictResult(user, item, &invenItem);
    else
    {
        return G_CDataManager()->m_disJoint->GetPredictResult(item->get_rarity());
    }
}

#include "ExpertJobLink4/getPredictResultItemTypeCount.inc"


#include "ExpertJobLink4/GetPredictResult_1.inc"



#include "ExpertJobLink4/GetPortableDisjointPossibleLevel.inc"


#include "ExpertJobLink4/_checkDisjointableItem.inc"


#include "ExpertJobLink4/_check_disjoint_need_slot.inc"


#include "ExpertJobLink4/_check_disjointable_item_state.inc"


#include "ExpertJobLink4/_check_disjointable_user_state.inc"

