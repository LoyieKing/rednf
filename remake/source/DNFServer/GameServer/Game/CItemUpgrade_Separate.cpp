// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CItemUpgrade_Separate/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CServerProxyMgr.h"         // CServerProxyMgr<T>::GetServerProxy 模板定义
#include "CItemUpgrade_Separate.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CItem.h"
#include "PacketTypes.h"     // MSG_ITEM_UPGRADE 完整类型（具名字段访问）
#include "Inven_Item.h"
#include "CEquipItem.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CGuildServerProxy.h"
#include "CInventory.h"
#include "GameWorld.h"
#include "PvPTypes.h"                 // CMTRand 完整类型（0x9c8）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_.findString

#include <cstdio>
#include <cstring>
#include <string>

#include "CItemUpgrade_Separate/CItemUpgrade_Separate.inc"
{
    // [LaneG 2026-09-11 实验·四形对照，维持 N93/N82 槽向封顶] ORIG 0x0811e007
    // `lea -0x24(%ebp),%esi`（time 结果槽深）vs range -0x20（浅）。本轮新增三形
    // 实测：①range 声明后置到 new 之后——读数逐值不变（nd 3）；②range 声明提到
    // 函数头第一句——逐值不变（nd 3）；③具名 seed+range 两句式（声明与赋值分离）
    // ——槽仍 -0x20/-0x24 未换向且 lea 推迟到实参装载（nd 3→4，劣化，已回滚）。
    // 连同 K2F-1 的「具名 seed 合并式 / 声明序交换」两形，共六形均不可翻 ⇒ 封顶。
    m_randRange = 100000;
    unsigned long range;
    m_rand = new CMTRand(static_cast<unsigned long>(time(0)));
    for (int i = 0; i < 100; ++i)
    {
        range = m_randRange;
        m_rand->randInt(range);
    }
}

namespace
{

// [AnonLane round8 2026-09-11] 本匿名 namespace 的 4 条 only-NEW 已清（ORIG 全库无同名符号）：
//  · `FloatBits kMatEpsilon`（`_ZN12_GLOBAL__N_111kMatEpsilonE`，d 段）→ ORIG 侧是匿名 .rodata
//    位模式 0x33d6bf95；实测与 `1e-7f` 的 IEEE 位模式逐位相同 ⇒ 使用点内联 `1e-7f`；
//  · `const float kGoldBase`(9840.0f)/`kRoundHalf`(0.5f) → ORIG 匿名 .rodata
//    （DAT_08b575b0 / DAT_08e122e8）⇒ 使用点内联字面量；
//  · `truncateToInt`（`_ZN12_GLOBAL__N_113truncateToIntEf`，t）本 TU **零调用点**且 ORIG 无该
//    符号（x87 fnstcw/fldcw 截断形在 ORIG 各处内联）⇒ 删定义（P154 无引用发明符号）。

}  // namespace

// ============================ POD 生命周期 ============================

#include "CItemUpgrade_Separate/GenuineDamageUpgradeInfo.inc"
{
    m_a = 0;
    m_b = 0;
    m_levelRate = 0.0f;     // +0x08（ctor 清零；import 写 level/10000f）
    m_goldRate = 0.0f;      // +0x0c；+0x10 保持未初始化（ORIG 同）
}

#include "CItemUpgrade_Separate/OutLineEFFECTInfoTemp.inc"
{
    m_a = 0;
    m_b = 0;
    m_c = 0;
    m_c = 0;                // ORIG 对 +0x8 连续写两次（死存储忠实保留）
}

namespace WongWork
{

#include "CItemUpgrade_Separate/STUpgradeInfo.inc"


// ============================ IItemUpgrade ============================

#include "CItemUpgrade_Separate/IItemUpgrade.inc"

#include "CItemUpgrade_Separate/dtor_IItemUpgrade.inc"

#include "CItemUpgrade_Separate/item_upgrade_system.inc"


// ============================ GenuineDamageUpgradeTable ============================

}  // namespace WongWork

#include "CItemUpgrade_Separate/GenuineDamageUpgradeTable.inc"

#include "CItemUpgrade_Separate/dtor_GenuineDamageUpgradeTable.inc"

#include "CItemUpgrade_Separate/getGenuineUpgradeDamage.inc"


#include "CItemUpgrade_Separate/getGenuineUpgradeMaterialCnt.inc"


#include "CItemUpgrade_Separate/getEffectInfo.inc"
{
    static OutLineEFFECTInfoTemp temp;

    if (m_maxLevel < key)
    {
        return temp;
    }

    std::map<int, OutLineEFFECTInfoTemp>::iterator it =
        m_effects.find(key);
    std::map<int, OutLineEFFECTInfoTemp>::const_iterator cit(it);
    if (cit != m_effects.end())
    {
        return cit->second;
    }
    return temp;
}

// ============================ CItemUpgrade_Separate ============================

namespace WongWork
{

#include "CItemUpgrade_Separate/dtor_CItemUpgrade_Separate.inc"


#include "CItemUpgrade_Separate/LoadUpgradeScript.inc"


#include "CItemUpgrade_Separate/_GetPenaltyType.inc"


#include "CItemUpgrade_Separate/_GetUpgradeInfo.inc"


#include "CItemUpgrade_Separate/_CheckCondition.inc"


#include "CItemUpgrade_Separate/_DoProcUpgrade.inc"


#include "CItemUpgrade_Separate/_NoticeUpgrade.inc"


#include "CItemUpgrade_Separate/ProcUpgrade.inc"


}  // namespace WongWork

// ============================ UpgradeSeparateInfo ============================

#include "CItemUpgrade_Separate/IncUpgradeSeparate.inc"


// ============================ CUser 公会公告 ============================

#include "CItemUpgrade_Separate/makeGuildItemSeparateUpgradMessage.inc"

