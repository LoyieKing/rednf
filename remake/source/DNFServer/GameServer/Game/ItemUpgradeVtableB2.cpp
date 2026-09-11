#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ItemUpgradeVtableB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CUser.h"
#include "CDataManager.h"
#include "CItem.h"
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CItemAmplifier.h"
#include "CStackableItem.h"
#include "CGlobalEffectManager.h"   // s_globalEffectManager_ 完整类型
#include "CEquipItem.h"
#include "CGuildServerProxy.h"    // CEventBase / CEventManager / stADDINFO
#include "CInventory.h"           // CInventory / Store / CValueStatistic
#include "DB_ReqItemUpgradeLog.h"
#include "PvPTypes.h"             // CMTRand（+0x4e8 randInt）
#include "EventUpgradeJarInfo.h"  // Event_Upgrade_Jar
#include "CCubeStatistic.h"
#include "DisJoint.h"
#include "LogManager.h"
#include "CSystemTime.h"
#include "CUserProc.h"            // APSystem::CUserProc
#include "CEnvironment.h"
// WongWork::CItemUpgradeTable（CItemUpgrade 表区子对象；ProcUpgrade 取表直调
// 其 getUpgradeInfoTable W 0x0854b382，禁 inline 视图代返）。
#include "CItemGenerator.h"
// MSG_ITEM_UPGRADE 完整类型（CItemUpgrade_Separate.h 仅前向声明；ProcUpgrade
// 需具名读 m_upgradeType/m_targetSlot/m_targetItemIdx/m_ticketSlot/
// m_materialSlot/m_25/m_27/m_28）。
#include "PacketTypes.h"

#include <cmath>
#include <cstring>
// std::max（_getAdditionalDisjointItem 0x08546983 模板实调）
#include <algorithm>
#include <utility>
#include <vector>

extern int itemPriceRates_[3];    // B 0x094f7bfc（InitScriptProducers_4.cpp 定义）

// B 0x094f7778（nm 尺寸 8）。源码此前无任何定义/写点；唯一读者
// GetItemUpgradeSuccessRate 以 fildl 取低 32 位。按 int 定义并置零。
extern int g_alchemistInfo[2];   // B 0x094f7778（定义于 CExpertJobList.cpp int[2]）


namespace expert_job
{

// [b28d-solo-ItemUpgradeVtableB2] ContinuousItemInfo 同布局具名视图：
// ORIG IsExistContinuousEffectItem 0x085d12a2 `call deEv; mov (%eax),%edx;
// mov 0xc(%ebp),%eax; cmp %eax,%edx` 是**两段装载**比较 ⇒ 字段与 int 形参
// 异型（P248 档③/P274）；权威头 ContinuousItemInfo::m_itemIdx 为 int 且跨 TU
// 消费点众多（改头需全消费段联判），故本 TU 以同布局视图（unsigned 首字段）
// 表达，零头改动、零 ABI/布局风险。
struct ContinuousItemIdxView
{
    unsigned int m_itemIdx;      // +0x00（同 ContinuousItemInfo::m_itemIdx）
    unsigned int m_expireTime;   // +0x04
};

// ---- CAlchemist 静态成员（此前无声明；仅本 TU 使用）----
class CAlchemist
{
public:
    static bool IsExistContinuousEffectItem(CUser* user, int itemId);
    static float GetItemUpgradeSuccessRate();
};

}  // namespace expert_job

#include "ItemUpgradeVtableB2/IsExistContinuousEffectItem.inc"


#include "ItemUpgradeVtableB2/GetItemUpgradeSuccessRate.inc"


#include "ItemUpgradeVtableB2/getItemSellPrice.inc"


#include "ItemUpgradeVtableB2/IsReinforceTicket.inc"


#include "ItemUpgradeVtableB2/GetReinforceRate.inc"


#include "ItemUpgradeVtableB2/IsAmplifyReinforceTicket.inc"


#include "ItemUpgradeVtableB2/GetAmplifyReinforceRate.inc"


#include "ItemUpgradeVtableB2/GetReinforceVaule.inc"


#include "ItemUpgradeVtableB2/GetAmplifyReinforceValue.inc"


#include "ItemUpgradeVtableB2/DisJointResult.inc"


#include "ItemUpgradeVtableB2/GetResult.inc"


// ============================================================================
// CReduceUpgradeItemPay —— 强化减费重复事件（CEventBase 派生；与
// CItemUpgrade.cpp 内同形内联定义，弱符号合并）。
// ORIG GetReduceRate W 0x854b56e：返回 +0x04 short 经 cwtl 符号扩展。
// ============================================================================
class CReduceUpgradeItemPay : public CEventBase
{
public:
    int GetReduceRate() { return (short)(m_addInfo.m_info0); }
};

// [b28d-solo-ItemUpgradeVtableB2] P49/P167：原匿名 namespace 的
// upgradeRand/randIntBound/embeddedTable/embeddedUpgradeTable 四个 inline
// helper 已全部删除——-O0 下它们是**真实 call**（ORIG 无的 EXTRA 符号），
// 调用点改为 m_rand 字段直读 / 基类 getUpgradeInfoTable() 成员直调后
// p167 差集归零（missing_in_game 40→36）。
namespace WongWork
{

#include "ItemUpgradeVtableB2/_getUpgradeCountByUpgradeItem.inc"


#include "ItemUpgradeVtableB2/_getUpgradeRateByUpgradeItem.inc"


#include "ItemUpgradeVtableB2/_CheckTargetUsableLv.inc"


#include "ItemUpgradeVtableB2/_ProcPortableUpgradeItem.inc"


#include "ItemUpgradeVtableB2/_DoProcUpgrade.inc"


#include "ItemUpgradeVtableB2/ProcUpgrade.inc"


#include "ItemUpgradeVtableB2/_getAdditionalDisjointItem.inc"


#include "ItemUpgradeVtableB2/_ProcPenalty.inc"


}  // namespace WongWork

#include "ItemUpgradeVtableB2/getUpgradeProbIncrease.inc"

