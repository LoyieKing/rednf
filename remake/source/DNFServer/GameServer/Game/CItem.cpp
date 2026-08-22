// df_game_r CItem 还原（I1 批次，2026-08-16）。
// 函数形态以 docs/class_func_reports/CItem.md + ORIG weak 访问器反汇编为准
// （AE 口径：调用目标归一；字段偏移/常量逐条一致）。
// 依赖的 NeedMaterialDataManager 由 GameStubs.cpp 提供桩。

#include "CItem.h"

// ===================== 外部依赖（其它 TU 提供） =====================

class NeedMaterialDataManager
{
public:
    static NeedMaterialDataManager* getInstance();
    CItem* getNeedMaterial(const CItem* item) const;
};

// ===================== 报告函数（CItem.md） =====================

CItem::CItem() {}

CItem::~CItem() {}

CItem* CItem::GetNeedMaterial() const
{
    CItem* p = NeedMaterialDataManager::getInstance()->getNeedMaterial(this);
    if (*(int*)p != -1)
    {
        return p;
    }
    return (CItem*)&m_pair24;
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

unsigned char CItem::check_job_type(char job) const
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

// ===================== 缺失弱访问器（ORIG weak） =====================

bool CItem::isEpicRoutingItem() const
{
    return m_bool79;
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
