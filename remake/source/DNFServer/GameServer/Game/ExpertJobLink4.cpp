// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ExpertJobLink4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// ExpertJobLink4.cpp — expert_job 域 LinkMisc4 批次
// ---- b28d-solo-ExpertJobLink4 判词汇总（11 件全部收口，2026-09-09）----
// AE/IDENTICAL（6 件）：
//   get_disjoint_result 0x082347ec（P23/P134 grade 内联实参 + get_disjoint_result_
//     static 化，ORIG 0x085d234a 实证 static：体首参 0x8(%ebp)=item 无 this）；
//   get_additional_item 0x0896d6ab（P13/P28 双迭代器 + P223 双岛 goto 标签串 +
//     真语义 bug 修正：`randValue >= m_rate` → `m_rate >= randValue`）；
//   CDisjointer::OnCloseMission 0x085d2b74（P23 内联 area + P139 __PRETTY_FUNCTION__
//     + 匿名 Zone 临时实参）；
//   _check_disjointable_user_state 0x081f8a7c（P59 正极性 + P0 全内联 + P47 反写尾块）；
//   _check_disjointable_item_state 0x081f8ba1（P0 具名 script 槽 + P198① err 预清 +
//     P225 while+break setne 物化）；
//   _check_disjoint_need_slot 0x081f8fe3（P214③/P312 三 `= {0}` 数组 rep stos +
//     P157/P183 三次独立下标 + P47 反写 invalid 分支 + `++arr[i]` 形）。
// N 封顶（P167 调用点差集 CLEAR ⇒ 语义完整，仅跨版本 lowering/槽布局）：
//   CAlchemist::OnTeleportCharacter 0x085d1424（N86b/N97：4.4 符号位测试四形恒折
//     `cmpl $0x0; jns`，ORIG `shr $0x1f; test %al,%al`，gap 2）；
//   GetPredictResult 0x085d1f68（N95/N82：槽基整体差 8 + N115 帧差 0x10；271=271
//     指令数相等、p167 47/47 CLEAR）；
//   complete_disjoint 0x085d31a0（N85 x87 比较 fucompp/fnstsw/sahf vs fucomip/fstp
//     + N82 槽基差 8；p167 CLEAR）；
//   get_disjoint_result_ 0x085d234a（N82/N85/P252：具名 float/槽序族；440 vs 424，
//     p167 41/41 CLEAR）；
//   OnCreateDisjointStore 0x085d2d08（N104 EH 尾块相对序 + N93 块级槽方向；
//     帧已对齐 0x60，仅 local/PacketGuard/Zone 三对象槽序互换）。
//
// 主符号（ORIG nm -S 核对）：
//   _ZN10expert_job10CAlchemist19OnTeleportCharacterEP5CUseriiiib
//       expert_job::CAlchemist::OnTeleportCharacter(CUser*, int, int, int, int, bool)
//       T 0x085d1424 sz 0x26f
//   _ZN10expert_job11CDisjointer21OnCreateDisjointStoreEP5CUserPKciii
//       expert_job::CDisjointer::OnCreateDisjointStore(CUser*, char const*, int, int, int)
//       T 0x085d2d08 sz 0x391
//   _ZN10expert_job11CDisjointer20get_disjoint_result_EP5CItemP10Inven_ItemiR14DisJointResultbP5CUser
//       expert_job::CDisjointer::get_disjoint_result_(CItem*, Inven_Item*, int,
//                                                     DisJointResult&, bool, CUser*)
//       T 0x085d234a sz 0x6cc
//   _ZN10expert_job13CExpertJobMgr16AcquireExpertJobE20ENUM_EXPERT_JOB_TYPE
//       expert_job::CExpertJobMgr::AcquireExpertJob(ENUM_EXPERT_JOB_TYPE)
//       T 0x0849e1dc sz 0x1c
//
// 直接依赖符号（被上述主函数调用，报告/objdump 在案，一并忠实转写以闭合链接）：
//   CDisjointer::GetScript / GetEtcScript      T 0x085d1e0a / 0x085d1e3a
//   expert_job::get_additional_item            T 0x0896d6ab
//   CDisjointMachine 六参 ctor / operator=     T 0x085d1ae8 / W 0x085d3ed6
//   CCharacExpertJob::get_disjoint_machine_endurance/grade  W 0x85d3e66/0x85d3e8e
//   stItemSelectRate::stItemSelectRate         W 0x085d3bd6
//   DisjointMachineGrade 三参 ctor             W 0x085d3b9e
//   CAlchemist ctor / D1 / D0                  T 0x085d11b4 / 0x085d11d0 / 0x085d1200
// ============================================================================

// getItemSellPrice 定义于 Store.cpp（ORIG T 0x0898c86e）；仓库暂无共享头声明。
#include "ExpertJobLink4/getItemSellPrice.inc"

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

// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 最前，使 `_GLOBAL__I_` 名等于
// ORIG 的 `_ZN10expert_job10CAlchemistC2Ev`（P327）。全限定定义可写在命名空间块
// 之外 ⇒ 发射序提前。语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
// ---- CAlchemist 生命周期（ctor 报告 expert_job__CAlchemist/CAlchemist.md；
//      dtor D1/D0 0x085d11d0/0x085d1200）----
#include "ExpertJobLink4/CAlchemist.inc"
}


// ---- stItemSelectRate::stItemSelectRate（ORIG W 0x085d3bd6：5×int 清零）----
#include "ExpertJobLink4/stItemSelectRate.inc"
}

// ---- DisjointMachineGrade 三参 ctor（ORIG W 0x085d3b9e）----
// 参数装载实证：+0=m_grade=第2参、+1=m_field1=第1参、+2=m_field2=第3参。
#include "ExpertJobLink4/DisjointMachineGrade.inc"
}

// ---- DisjointMachineGrade 默认 ctor（ORIG W 0x0896dbcc：三字节清零）----
#include "ExpertJobLink4/DisjointMachineGrade_1.inc"
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


// ---- expert_job::get_additional_item（报告 expert_job/get_additional_item.md，
//      ORIG T 0x0896d6ab）：顺序遍历表项写入 out；命中 [min,max] 区间后若剩余
//      随机值 >= 累加概率则返回，否则扣减继续。 ----
#include "ExpertJobLink4/get_additional_item.inc"
}

// ---- CCharacExpertJob 分解机 getter（ORIG W 0x85d3e66 / 0x85d3e8e）----
#include "ExpertJobLink4/get_disjoint_machine_endurance.inc"
}

#include "ExpertJobLink4/get_disjoint_machine_grade.inc"
}

// ORIG W 0x085d3e72：alter() 后写 +0x08。
#include "ExpertJobLink4/set_disjoint_machine_endurance.inc"
}

#include "ExpertJobLink4/dtor_CAlchemist.inc"
}

// ---- CAlchemist::OnTeleportCharacter（报告 OnTeleportCharacter.md，
//      ORIG T 0x085d1424 sz 0x26f）----
#include "ExpertJobLink4/OnTeleportCharacter.inc"
}

// ---- CDisjointer::GetScript / GetEtcScript（ORIG T 0x085d1e0a / 0x085d1e3a：
//      函数级静态缓存单例；ORIG 静态变量 b 0x09472f98 / 0x09472f94）----
#include "ExpertJobLink4/GetScript.inc"
}

#include "ExpertJobLink4/GetEtcScript.inc"
}

// ---- CDisjointMachine 六参 ctor（objdump 0x085d1ae8 忠实转写）：
//      IObject 基类 → vptr → Rectangle 默认构造 → name 默认构造 → 逐字段赋值 →
//      碰撞盒 {x-0x50, y-0x32, 0xa0, 0x64}。 ----
#include "ExpertJobLink4/CDisjointMachine.inc"
}

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

// ---- CDisjointer::OnCreateDisjointStore（报告 OnCreateDisjointStore.md，
//      ORIG T 0x085d2d08 sz 0x391）----
#include "ExpertJobLink4/OnCreateDisjointStore.inc"
}

// ---- CDisjointer::get_disjoint_result_（报告 get_disjoint_result_.md，
//      ORIG T 0x085d234a sz 0x6cc）。 ----
#include "ExpertJobLink4/get_disjoint_result_.inc"
}

// getItemSellPrice 定义于 Store.cpp（ORIG T 0x0898c86e）；仓库暂无共享头声明。
#include "ExpertJobLink4/getItemSellPrice_1.inc"
#include "ExpertJobLink4/AcquireExpertJob.inc"
}

// ---- CExpertJobFactory::AcquireExpertJob（ORIG W 0x0849f5d2）：
//      type==1 → Enchanter 池（this+0x00）；type==3 → Disjointer 池（this+0x30）；
//      取到后先虚调用 Reset()（vtable+0xc 槽）再非虚 SetType(type)。 ----
// b28c-1 N7: 虚调用 callee %eax↔%edx 互换。
#include "ExpertJobLink4/AcquireExpertJob_1.inc"
}

// ---- CExpertJobMgr::FreeExpertJob（ORIG T 0x0849e1f8：转发工厂）----
#include "ExpertJobLink4/FreeExpertJob.inc"
}

// ---- CExpertJobFactory::FreeExpertJob（ORIG W 0x0849f63e）：
//      job 非空时按 GetType() 分派到对应池。 ----
#include "ExpertJobLink4/FreeExpertJob_1.inc"
}

// ---------------------------------------------------------------------------
// CDisjointMachine getter（ORIG W 0x85d3e06 / 0x082347e0 / 0x85d3e12）
// P210：以下 CDisjointer 包装（grade/isMine）必须定义于 get_user/get_grade
// 之前，否则 4.4 继承 callee outgoing 帧塌缩（sub $0x4 vs ORIG $0x18）。
// ---------------------------------------------------------------------------
// get_disjoint_machine_grade（ORIG T 0x85d1f0a：movsbl m_machine.get_grade()）
#include "ExpertJobLink4/get_disjoint_machine_grade_1.inc"
}

// isMine（ORIG W 0x08234854）：m_machine.get_user() == user
#include "ExpertJobLink4/isMine.inc"
}

// get_disjoint_cost（ORIG W 0x085d3e50）
#include "ExpertJobLink4/get_disjoint_cost.inc"
}

#include "ExpertJobLink4/get_cost.inc"
#include "ExpertJobLink4/get_user.inc"
#include "ExpertJobLink4/get_grade.inc"
#include "ExpertJobLink4/send_error.inc"
}

// check_disjoint_endurance（报告 check_disjoint_endurance.md，ORIG T 0x085d1f24）
#include "ExpertJobLink4/check_disjoint_endurance.inc"
}

// IsAvailableJob（ORIG W 0x849f596：恒返 1）
#include "ExpertJobLink4/IsAvailableJob.inc"
#include "ExpertJobLink4/Reset.inc"
#include "ExpertJobLink4/OnStartMission.inc"
}

// OnCloseMission（报告 OnCloseMission.md，ORIG T 0x085d2b74 sz 0x17e）：
// 广播 0xd9 下线包 → 注销村庄对象 → 清专家职业 → 归还池。
#include "ExpertJobLink4/OnCloseMission.inc"
}

// complete_disjoint（报告 complete_disjoint.md，ORIG T 0x085d31a0 sz 0x42d）。
// 耐久/经验结算 + 请求者付费、机主收款 + 机主耐久不足时经 slot1 关闭任务。
#include "ExpertJobLink4/complete_disjoint.inc"
}

// get_disjoint_result（ORIG W 0x082347ec）：以机器等级/归属补全六参版本。
#include "ExpertJobLink4/get_disjoint_result.inc"
}

// ---------------------------------------------------------------------------

// IsAvailableJob（ORIG W 0x849f30c：恒返 1）
#include "ExpertJobLink4/IsAvailableJob_1.inc"
#include "ExpertJobLink4/Reset_1.inc"
}

// OnStartMission（报告 OnStartMission.md，ORIG T 0x0849c7b6）：
// 双方登记到附魔师槽位并各自 SetCurCharacExpertJob(this)。
#include "ExpertJobLink4/OnStartMission_1.inc"
}

// OnCloseMission（报告 OnCloseMission.md，ORIG T 0x0849c822 sz 0x14c）：
// 按 uid 匹配离场方；type∈{1,2} 时向另一方发空 0x9e 包；最后归还池。
#include "ExpertJobLink4/OnCloseMission_1.inc"
}

// ---------------------------------------------------------------------------
// GetPredictResult（报告 GetPredictResult.md，ORIG T 0x085d1f68 sz 0x3e2）。
// 函数级静态结果向量（guard 变量 b 0x09472f80 / 向量 b 0x09472f88）：
//   [0] 机主本人且 rarity>1 → script+0x34（selfService[1]，奖励 A 物品）
//   [1] map 命中项的 stDisjointResult.m_field0（cube 索引）
//   [2..] 附加/大奖物品表中未收录过的 m_itemIndex（std::find 去重）
// ---------------------------------------------------------------------------
#include "ExpertJobLink4/GetPredictResult.inc"
}

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

// ---- DisJoint::getPredictResultItemTypeCount（ORIG T 0x08473148，
//      docs/class_func_reports/DisJoint/getPredictResultItemTypeCount.md）：
//      rarity ∉ [0,5] → 1；否则读 m_additional[rarity].m_count
//      （this+0x5c8+rarity*0x10，P8：Ghidra 的 (param+0x5c)*0x10+8 即 entry
//      起点 m_count，非 +8 字段），非 0 → 2，否则 → 1。 ----
#include "ExpertJobLink4/getPredictResultItemTypeCount.inc"
}

// ---- DisJoint::GetPredictResult（报告 GetPredictResult.md，ORIG T 0x08473116）：
//      rarity ∈ [0,5] → &m_resultVecs[rarity]（this+0x628+rarity*0xc）；
//      越界 → &m_resultVecs[0]（this+0x628）。rarity 按有符号比较（js/jle）。 ----
#include "ExpertJobLink4/GetPredictResult_1.inc"
}


// ORIG W 0x0822b1f4（声明补入 STExpertJobScript.h；定义置本 TU 保持
// expert_job 域内聚）。m_portableDisjointMap(+0x18) find(level)，未命中 -1。
#include "ExpertJobLink4/GetPortableDisjointPossibleLevel.inc"
}

// ---- DisPatcher_DisJointItem::_checkDisjointableItem
// （报告 _checkDisjointableItem.md，ORIG T 0x081fa0f6）：
// 未鉴定且无目标用户 → 0xae；物品缺失 → 0x11；
// IsImpossibleDisjoint（ORIG vptr+0x38 = vtable 槽16）→ 0x13；GetAttachType==2 → 0x11。 ----
// [b28c-31] N7+N18b：虚调用 callee ORIG %edx vs 4.4 %eax（vptr+0x38 槽）；
// 另 GetAttachType 结果 ORIG 回读槽再 `cmp $0x2,%eax`，4.4 恒折 `cmpl $0x2,-0xc`
// 内存直比（P209 附 / N18b）。
#include "ExpertJobLink4/_checkDisjointableItem.inc"
}

// ---- _check_disjoint_need_slot（报告 _check_disjoint_need_slot.md，
//      ORIG T 0x081f8fe3 sz 0x2e7）：预测产物按背包类型分桶统计空位需求，
//      缺口超出第 0 类空槽数则抛 ENUM_ERROR 4。 ----
#include "ExpertJobLink4/_check_disjoint_need_slot.inc"
}

// ---- _check_disjointable_item_state（报告 _check_disjointable_item_state.md，
//      ORIG T 0x081f8ba1 sz 0x3ec）：取源物品快照、拆解券等级门槛与
//      DisPatcher 静态校验，任一失败抛对应 ENUM_ERROR。 ----
#include "ExpertJobLink4/_check_disjointable_item_state.inc"
}

// ---- _check_disjointable_user_state（报告 _check_disjointable_user_state.md，
//      ORIG T 0x081f8a7c sz 0x125）：状态门 + 服务限制 + 保护字段三查。 ----
#include "ExpertJobLink4/_check_disjointable_user_state.inc"
}
