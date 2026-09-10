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
CItem* CItem::GetNeedMaterial() const
{
    CItem* p = NeedMaterialDataManager::getInstance()->getNeedMaterial(this);
    if (*(int*)p != -1)
    {
        return p;
    }
    return (CItem*)&m_pair24;
}

CItem::CItem() {}

CItem::~CItem() {}

// ORIG 0x08326739 T：GetItemType() ∈ {0xe, 0xf, 0x10}（包类商品）。
bool _isPackageItem(const CItem* item)
{
    // ORIG 双物化（内层 || 链 mov$1/mov$0 岛 + 外层 test/sete 归一）⇒
    // if(cond)return true;return false; 形（P56）。
    if (item->GetItemType() == 0xf || item->GetItemType() == 0xe ||
        item->GetItemType() == 0x10)
        return true;
    return false;
}

bool CItem::checkItemUpgradePossible() const
{
    return 0;
}

bool CItem::check_item_routing() const
{
    if (isEpicRoutingItem())
    {
        return true;
    }
    return m_rarity > 0;
}

bool CItem::check_job_type(char job) const
{
    if (job > 10)
    {
        return 0;
    }
    int idx = job;
    return m_jobUsable[idx];
}

bool CItem::isAmplifyPollutionItem() const
{
    return true;
}

// ===================== 虚函数（ORIG vtable 0x8c96da8 槽 12..23） =====================

bool CItem::IsImpossibleUpgrade() const
{
    return m_impossibleUpgrade;
}

bool CItem::IsImpossibleAmplify() const
{
    return m_impossibleAmplify;
}

bool CItem::IsImpossibleDisjoint() const
{
    return m_impossibleDisjoint;
}

bool CItem::IsImpossibleGift() const
{
    return m_impossibleGift;
}

bool CItem::IsImpossibleCharacCargo() const
{
    return m_impossibleCharacCargo;
}

bool CItem::IsPossibleKiriProtection() const
{
    return m_possibleKiriProtection;
}

int CItem::PeriodAfterUnsealing() const
{
    return m_periodAfterUnsealing;
}

bool CItem::IsRandomOption() const
{
    return 0;
}

int CItem::GetRandomOptionGrade() const
{
    return 0;
}

bool CItem::IsRandomOptionUnseal() const
{
    return 0;
}

// ===================== 非虚访问器（ORIG weak） =====================

const char* CItem::GetItemName() const
{
    return m_str2c.c_str();
}

int CItem::GetAttachType() const
{
    return m_attachType;
}

bool CItem::is_stackable() const
{
    return m_stackable;
}

int CItem::get_index() const
{
    return m_index;
}

int CItem::get_grade() const
{
    return m_grade;
}
int CItem::GetSellPrice() const
{
    return m_field30;
}

int CItem::get_price() const
{
    return m_field0c;
}

int CItem::getItemGroupName() const
{
    return m_itemGroupName;
}

int CItem::getUsableLevel() const
{
    return m_usableLevel;
}

int CItem::get_rarity() const
{
    return m_rarity;
}

int CItem::getUsablePeriod() const
{
    return m_usablePeriod;
}

int CItem::getExpirationDate() const
{
    return m_expirationDate;
}

unsigned long CItem::GetQuestItemDropBouns()
{
    return m_questBonus;
}

const char* CItem::GetItemIconName() const
{
    return m_strb0.c_str();
}

bool CItem::IsEnableWorld(ENUM_WORLD_TYPE type) const
{
    if (m_setd4.find((int)type) != m_setd4.end())
    {
        return true;
    }
    return false;
}

// ORIG 0x822c8ea：subset 检查。遍历 other->m_setd4，若任一世界类型本对象
// 未启用则 false；全部启用（含空集）则 true。
bool CItem::IsEnableWorld(const CItem* other) const
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

bool CItem::isEpicRoutingItem() const
{
    return m_bool79;
}

// ORIG 0x8150f18（weak）：movzbl 0xbd(%eax) — 返回 +0xbd 路由标志
bool CItem::IsRoutingItem() const
{
    return m_isRoutingItem;
}

bool CItem::check_low_level(int level) const
{
    return m_usableLevel <= level;
}

bool CItem::IsHiddenOption() const
{
    return m_boolF8;
}

// ORIG 0x828b5b4：（movzbl 0x60(%eax); sete %al）即 this+0x60 字节 == 1
bool CItem::isPackagable() const
{
    return m_char60 == 0x1;
}

bool CItem::isExpertJobUsable(ENUM_EXPERT_JOB_TYPE type, int level) const
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

float CItem::GetExpertJobCompoundMaterialVariation() const
{
    return m_floatFC;
}

float CItem::GetExpertJobCompoundRateVariation() const
{
    return m_float100;
}

void CItem::GetExpertJobCompoundResultVariation(
    STItemScript::SEXPERTJOB::stExpertCompoundResultVariation& out) const
{
    out = m_compound110;
}

float CItem::GetExpertJobSelfDisjointBigWinRate() const
{
    return m_float118;
}

float CItem::GetExpertJobSelfDisjointResultVariation() const
{
    return m_float11c;
}

void CItem::GetExpertJobAdditionalExp(
    STItemScript::SEXPERTJOB::stExpertJobAdditionalExp& out) const
{
    out = m_additionalExp120;
}

int CItem::GetFinishPointPrice() const
{
    return m_field64;
}

int CItem::get_need_skill() const  // ORIG 0850d27a
{
    return m_int58;
}

int CItem::getFootControlRateLimit() const  // ORIG 0850d286
{
    return m_int5c;
}

int CItem::GetUsablePvPRank() const  // ORIG W 0x086946c4
{
    return m_intEC;
}

int CItem::GetIncreaseStatusType() const  // ORIG W 0x086946b6
{
    return m_increaseStatusType;
}

bool CItem::GetIncreaseStatusIntData(int idx, int& out) const
    // ORIG W 0x08694658：idx<0 或 idx>=size → false；否则 out=at(idx) → true
{
    if (idx < 0 || idx >= (int)m_vec90.size())
        return false;
    out = m_vec90.at(idx);
    return true;
}

// ORIG W 0x08545fda（0x0e）：mov 0xf4(%eax),%eax — 返回 +0xf4 所需等级
int CItem::GetNeedLevel() const
{
    return m_intF4;
}

// ORIG W 0x08545fcc：返回升级技能类型字段 +0xf0。
int CItem::GetUpSkillType() const
{
    return m_intF0;
}

// ORIG W 0x822c86e（0x0d）：mov 0x8(%ebp),%eax; add $0x9c,%eax; ret
// ORIG W 0x822c86e（0x0d）：mov 0x8(%ebp),%eax; add $0x9c,%eax; ret
DyeInfo* CItem::GetDyeInfo() const
{
    return const_cast<DyeInfo*>(&m_dyeInfo9c);
}

// ORIG W 0x822c97a（0x57）：m_set128.find(category) != m_set128.end()
bool CItem::hasCategory(STItemScript::ENUM_ITEM_CATEGORY category) const
{
    std::set<STItemScript::ENUM_ITEM_CATEGORY>::const_iterator it =
        m_set128.find(category);
    return it != m_set128.end();
}

// ORIG W 0x85f7be0（0x0f）：mov 0x8(%ebp),%eax; movzbl 0x144(%eax),%eax; ret
// [2026-09-10 E01 落 · Main 批准] 返回类型 uchar→bool（与 CItem.h 声明同批）：
// ORIG 消费端 0x085f77f4 `call; test %al,%al; je` 无 setne 归一（P248 档①）。
// 返回类型不入 mangled；体仍为 movzbl 直返（bool 返回不插 setne，P292 勘误）。
bool CItem::IsBanRedeemItem() const
{
    return m_char144;
}
