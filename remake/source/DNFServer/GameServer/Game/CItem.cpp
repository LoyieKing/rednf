// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CItem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r CItem 还原（I1 批次，2026-08-16）。
// 函数形态以 docs/class_func_reports/CItem.md + ORIG weak 访问器反汇编为准
// （AE 口径：调用目标归一；字段偏移/常量逐条一致）。
// NeedMaterialDataManager 走权威头（完整定义 + 实现落 NeedMaterialDataManager.cpp），
// 本 TU 不再重复声明（§4.4 单一声明点；两处调用 mangled 一致，零行为变化）。

#include "CItem.h"
#include "NeedMaterialDataManager.h"

// [A31-GI · P338/P327] 补 <iostream> 恢复 __ioinit；GetNeedMaterial() 前移为本 TU
// 首个强外部符号 ⇒ GI 键名 `_GLOBAL__I__ZNK5CItem15GetNeedMaterialEv`（ORIG 0x85139cb）。
#include <iostream>

// ===================== 报告函数（CItem.md） =====================

// ORIG 0x085139cb
#include "CItem/GetNeedMaterial.inc"
{
    CItem* p = NeedMaterialDataManager::getInstance()->getNeedMaterial(this);
    if (*(int*)p != -1)
    {
        return p;
    }
    return (CItem*)&m_pair24;
}

#include "CItem/CItem.inc"

#include "CItem/dtor_CItem.inc"

// ORIG 0x08326739 T：GetItemType() ∈ {0xe, 0xf, 0x10}（包类商品）。
#include "CItem/_isPackageItem.inc"
{
    // ORIG 双物化（内层 || 链 mov$1/mov$0 岛 + 外层 test/sete 归一）⇒
    // if(cond)return true;return false; 形（P56）。
    if (item->GetItemType() == 0xf || item->GetItemType() == 0xe ||
        item->GetItemType() == 0x10)
        return true;
    return false;
}

#include "CItem/checkItemUpgradePossible.inc"
{
    return 0;
}

#include "CItem/check_item_routing.inc"
{
    if (isEpicRoutingItem())
    {
        return true;
    }
    return m_rarity > 0;
}

#include "CItem/check_job_type.inc"
{
    if (job > 10)
    {
        return 0;
    }
    int idx = job;
    return m_jobUsable[idx];
}

#include "CItem/isAmplifyPollutionItem.inc"
{
    return true;
}

// ===================== 虚函数（ORIG vtable 0x8c96da8 槽 12..23） =====================

#include "CItem/IsImpossibleUpgrade.inc"
{
    return m_impossibleUpgrade;
}

#include "CItem/IsImpossibleAmplify.inc"
{
    return m_impossibleAmplify;
}

#include "CItem/IsImpossibleDisjoint.inc"
{
    return m_impossibleDisjoint;
}

#include "CItem/IsImpossibleGift.inc"
{
    return m_impossibleGift;
}

#include "CItem/IsImpossibleCharacCargo.inc"
{
    return m_impossibleCharacCargo;
}

#include "CItem/IsPossibleKiriProtection.inc"
{
    return m_possibleKiriProtection;
}

#include "CItem/PeriodAfterUnsealing.inc"
{
    return m_periodAfterUnsealing;
}

#include "CItem/IsRandomOption.inc"
{
    return 0;
}

#include "CItem/GetRandomOptionGrade.inc"
{
    return 0;
}

#include "CItem/IsRandomOptionUnseal.inc"
{
    return 0;
}

// ===================== 非虚访问器（ORIG weak） =====================

#include "CItem/GetItemName.inc"
{
    return m_str2c.c_str();
}

#include "CItem/GetAttachType.inc"
{
    return m_attachType;
}

#include "CItem/is_stackable.inc"
{
    return m_stackable;
}

#include "CItem/get_index.inc"
{
    return m_index;
}

#include "CItem/get_grade.inc"
{
    return m_grade;
}
#include "CItem/GetSellPrice.inc"
{
    return m_field30;
}

#include "CItem/get_price.inc"
{
    return m_field0c;
}

#include "CItem/getItemGroupName.inc"
{
    return m_itemGroupName;
}

#include "CItem/getUsableLevel.inc"
{
    return m_usableLevel;
}

#include "CItem/get_rarity.inc"
{
    return m_rarity;
}

#include "CItem/getUsablePeriod.inc"
{
    return m_usablePeriod;
}

#include "CItem/getExpirationDate.inc"
{
    return m_expirationDate;
}

#include "CItem/GetQuestItemDropBouns.inc"
{
    return m_questBonus;
}

#include "CItem/GetItemIconName.inc"
{
    return m_strb0.c_str();
}

#include "CItem/IsEnableWorld.inc"
{
    if (m_setd4.find((int)type) != m_setd4.end())
    {
        return true;
    }
    return false;
}

// ORIG 0x822c8ea：subset 检查。遍历 other->m_setd4，若任一世界类型本对象
// 未启用则 false；全部启用（含空集）则 true。
#include "CItem/IsEnableWorld_1.inc"
{
    for (std::set<int>::const_iterator it = other->m_setd4.begin();
         it != other->m_setd4.end(); ++it)
    {
        if (!IsEnableWorld((ENUM_WORLD_TYPE)*it))
        {
            return false;
        }
    }
    return true;
}

// ===================== 缺失弱访问器（ORIG weak） =====================

#include "CItem/isEpicRoutingItem.inc"
{
    return m_bool79;
}

// ORIG 0x8150f18（weak）：movzbl 0xbd(%eax) — 返回 +0xbd 路由标志
#include "CItem/IsRoutingItem.inc"
{
    return m_isRoutingItem;
}

#include "CItem/check_low_level.inc"
{
    return m_usableLevel <= level;
}

#include "CItem/IsHiddenOption.inc"
{
    return m_boolF8;
}

// ORIG 0x828b5b4：（movzbl 0x60(%eax); sete %al）即 this+0x60 字节 == 1
#include "CItem/isPackagable.inc"
{
    return m_char60 == 0x1;
}

#include "CItem/isExpertJobUsable.inc"
{
    if (m_vec104.empty())
    {
        return true;
    }
    for (std::vector<STItemScript::SEXPERTJOB::SUSABLE>::const_iterator it =
             m_vec104.begin();
         it != m_vec104.end(); it++)
    {
        if (it->m_field0 == type)
        {
            if (it->m_field4 <= level)
            {
                return true;
            }
            return false;
        }
        if (it->m_field0 == ENUM_EXPERT_JOB_TYPE_5 &&
            type != ENUM_EXPERT_JOB_TYPE_0)
        {
            if (it->m_field4 <= level)
            {
                return true;
            }
            return false;
        }
    }
    return false;
}

#include "CItem/GetExpertJobCompoundMaterialVariation.inc"
{
    return m_floatFC;
}

#include "CItem/GetExpertJobCompoundRateVariation.inc"
{
    return m_float100;
}

#include "CItem/GetExpertJobCompoundResultVariation.inc"
    STItemScript::SEXPERTJOB::stExpertCompoundResultVariation& out) const
{
    out = m_compound110;
}

#include "CItem/GetExpertJobSelfDisjointBigWinRate.inc"
{
    return m_float118;
}

#include "CItem/GetExpertJobSelfDisjointResultVariation.inc"
{
    return m_float11c;
}

#include "CItem/GetExpertJobAdditionalExp.inc"
    STItemScript::SEXPERTJOB::stExpertJobAdditionalExp& out) const
{
    out = m_additionalExp120;
}

#include "CItem/GetFinishPointPrice.inc"
{
    return m_field64;
}

#include "CItem/get_need_skill.inc"
{
    return m_int58;
}

#include "CItem/getFootControlRateLimit.inc"
{
    return m_int5c;
}

#include "CItem/GetUsablePvPRank.inc"
{
    return m_intEC;
}

#include "CItem/GetIncreaseStatusType.inc"
{
    return m_increaseStatusType;
}

#include "CItem/GetIncreaseStatusIntData.inc"
    // ORIG W 0x08694658：idx<0 或 idx>=size → false；否则 out=at(idx) → true
{
    if (idx < 0 || idx >= (int)m_vec90.size())
        return false;
    out = m_vec90.at(idx);
    return true;
}

// ORIG W 0x08545fda（0x0e）：mov 0xf4(%eax),%eax — 返回 +0xf4 所需等级
#include "CItem/GetNeedLevel.inc"
{
    return m_intF4;
}

// ORIG W 0x08545fcc：返回升级技能类型字段 +0xf0。
#include "CItem/GetUpSkillType.inc"
{
    return m_intF0;
}

// ORIG W 0x822c86e（0x0d）：mov 0x8(%ebp),%eax; add $0x9c,%eax; ret
// ORIG W 0x822c86e（0x0d）：mov 0x8(%ebp),%eax; add $0x9c,%eax; ret
#include "CItem/GetDyeInfo.inc"
{
    return const_cast<DyeInfo*>(&m_dyeInfo9c);
}

// ORIG W 0x822c97a（0x57）：m_set128.find(category) != m_set128.end()
#include "CItem/hasCategory.inc"
{
    std::set<STItemScript::ENUM_ITEM_CATEGORY>::const_iterator it =
        m_set128.find(category);
    return it != m_set128.end();
}

// ORIG W 0x85f7be0（0x0f）：mov 0x8(%ebp),%eax; movzbl 0x144(%eax),%eax; ret
// [2026-09-10 E01 落 · Main 批准] 返回类型 uchar→bool（与 CItem.h 声明同批）：
// ORIG 消费端 0x085f77f4 `call; test %al,%al; je` 无 setne 归一（P248 档①）。
// 返回类型不入 mangled；体仍为 movzbl 直返（bool 返回不插 setne，P292 勘误）。
#include "CItem/IsBanRedeemItem.inc"
{
    return m_char144;
}
