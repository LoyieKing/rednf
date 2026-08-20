// df_game_r Game/ CUserPremium 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 声明以 CUser.h 为准。
#include "CUser.h"
#include "GlobalData.h"
namespace WongWork
{
#pragma pack(push, 1)
struct PremiumItemView
{
    int m_itemIndex;
    int m_count;
};

struct PremiumDefinitionView
{
    int m_type;
    int m_enabled;
    PremiumItemView m_items[5];
    char m_reserved34[0x40];
    unsigned short m_advantageExpRate;
    int m_advantageFatigueRate;
    int m_advantagePremiumCount;
    int m_needDailyCheck;
    char m_reserved84[0x1c];
    char m_overEquipFlag;
    char m_reserveda1[0xb];
    int m_applyDayMask;
    char m_reserveda9[3];
    int m_fieldAc;
    char m_reservedb0[0x70];
};
struct UserAdditionalInfoView
{
    int m_value0;
    int m_value1;
    unsigned short m_value2;
    unsigned short m_value3;
    unsigned short m_value4;
    unsigned short m_value5;
    int m_value10;
    char m_reserved14[0x26];
    int m_value3a;
    unsigned short m_value3e;
    union
    {
        unsigned short m_loopValues[4];
        struct
        {
            int m_value42;
            unsigned short m_value46;
        };
    };
    short m_value4a;
    char m_reserved4c[0x14];
};

struct PremiumAdditionalView
{
    int m_base0;
    int m_base1;
    unsigned short m_baseRate[4];
    unsigned short m_scale[4];
    char m_reserved18[0x22];
    int m_multiplier0;
    short m_multiplier1;
    short m_multiplier2;
    int m_multiplier3;
    short m_multiplier4;
    short m_multiplier5;
    short m_multiplier6;
    short m_multiplier7;
    char m_reserved4e[4];
    unsigned int m_output0;
    unsigned int m_output1;
    unsigned short m_output2;
    unsigned short m_output3;
    unsigned short m_output4;
    unsigned short m_output5;
    unsigned short m_output6[4];
    char m_reserved6a[0x22];
    int m_output10;
    short m_output11;
    short m_output12;
    unsigned int m_output13;
    short m_output14;
    short m_output15;
    short m_output16;
    short m_output17;
};
#pragma pack(pop)

static PremiumDefinitionView* GetPremiumDefinition(int type)
{
    return &reinterpret_cast<PremiumDefinitionView*>(GlobalData::g_SPremiumInfo)[type];
}

unsigned short CUserPremium::getOverSkillLevel() const
{
    return m_overSkillLevel;
}

int CUserPremium::GetOverEquipableLevel(ENUM_EQUIPMENTTYPE type) const
{
    (void)type;
    return m_premium.m_overEquipFlag != 0 ? 10 : 0;
}

void CUserPremium::InitPremium()
{
    memset(m_entryPrefix, 0, sizeof(m_entryPrefix));
    memset(m_entries, 0, sizeof(m_entries));
    m_premium.m_normalCount = 0;
    m_premium.m_restrictedCount = 0;
    m_premium.m_advantageExpRate = 0;
    m_premium.m_advantageFatigueRate = 0;
    m_premium.m_advantagePremiumCount = 0;
    m_premium.m_needDailyCheck = 0;
    m_premium.m_field85e = 0;
    m_premium.m_field860 = 0;
    m_premium.m_field862 = 0;
    memset(m_premium.m_additionalInfo, 0, sizeof(m_premium.m_additionalInfo));
    m_premium.m_overEquipFlag = 0;
    m_premium.m_additionalBase[0] = 0;
    m_premium.m_returnItemMap.clear();
}

short CUserPremium::GetAdvantageFatigueRate() const
{
    return static_cast<short>(m_premium.m_advantageFatigueRate);
}

short CUserPremium::GetAdvantageExpRate() const
{
    return static_cast<short>(m_premium.m_advantageExpRate);
}

bool CUserPremium::CheckPremium(int type) const
{
    SUserPremiumInfo* info = GetPremiumInfo(type);
    return info->m_type != 0 && info->m_state != 3;
}

int CUserPremium::GetAdvPremiumCount() const
{
    return m_premium.m_advantagePremiumCount;
}

void CUserPremium::AddPremium(int type, int start, int end, int flag)
{
    SUserPremiumInfo* entry = GetPremiumInfo(type);
    if (entry->m_type != 0)
        RemovePremium(type);
    entry->m_type = type;
    entry->m_start = start;
    entry->m_end = end;
    entry->m_flag = flag;
    if (GetPremiumDefinition(type)->m_fieldAc != -1)
        m_premium.m_field862 = 1;
    startPremium(type, true);
}

void CUserPremium::RemovePremium(int type)
{
    SUserPremiumInfo* entry = GetPremiumInfo(type);
    if (entry->m_state != 2)
    {
        entry->m_type = 0;
        PremiumDefinitionView* premiumInfo = GetPremiumDefinition(type);
        if (premiumInfo->m_type == 1)
        {
            --m_premium.m_normalCount;
            if (m_premium.m_normalCount < 0)
                m_premium.m_normalCount = 0;
        }
        else
        {
            --m_premium.m_restrictedCount;
            if (m_premium.m_restrictedCount < 0)
                m_premium.m_restrictedCount = 0;
        }
    }
}

int CUserPremium::startPremium(int type, bool force)
{
    SUserPremiumInfo* entry = GetPremiumInfo(type);
    if (!force && entry->m_state != 2)
        return 0;
    int curTime = GlobalData::s_systemTime_.getCurSec();
    PremiumDefinitionView* premiumInfo = GetPremiumDefinition(type);
    if (premiumInfo->m_enabled != 0)
    {
        if (curTime < entry->m_start)
        {
            entry->m_state = 2;
            return 0;
        }
        if (entry->m_end <= curTime)
            return 0;
    }
    if (premiumInfo->m_type == 1)
        ++m_premium.m_normalCount;
    else
        ++m_premium.m_restrictedCount;
    entry->m_state = 1;
    return 1;
}

void CUserPremium::ReCalcAdvantage()
{
    m_premium.m_advantageExpRate = 0;
    m_premium.m_advantageFatigueRate = 0;
    m_premium.m_advantagePremiumCount = 0;
    m_premium.m_needDailyCheck = 0;
    m_premium.m_overEquipFlag = 0;
    memset(m_premium.m_additionalInfo, 0, sizeof(m_premium.m_additionalInfo));
    m_premium.m_field85e = 0;
    m_premium.m_field860 = 0;
    m_premium.m_returnItemMap.clear();

    for (int type = 1; type < 0x6a; ++type)
    {
        SUserPremiumInfo* entry = GetPremiumInfo(type);
        int entryType = entry->m_type;
        if (entryType == 0 || entryType >= 0x6a)
            continue;
        PremiumDefinitionView* premiumInfo = GetPremiumDefinition(entryType);
        if (premiumInfo->m_type != 1)
            continue;
        if (premiumInfo->m_needDailyCheck != 0)
            m_premium.m_needDailyCheck = 1;
        m_premium.m_advantageExpRate += premiumInfo->m_advantageExpRate;
        m_premium.m_advantagePremiumCount += premiumInfo->m_advantagePremiumCount;
        m_premium.m_advantageFatigueRate += premiumInfo->m_advantageFatigueRate;
        if (premiumInfo->m_overEquipFlag != 0)
            m_premium.m_overEquipFlag = 1;
    }
}



void CUserPremium::RecalcAdditionalInfo(CUser* user)
{
    if (user->getCurCharacR() == 0 || GetAdvPremiumCount() == 0)
        return;

    const UserAdditionalInfoView* addInfo =
        reinterpret_cast<const UserAdditionalInfoView*>(user->getCurCharacAddInfoR());
    PremiumAdditionalView* premium =
        reinterpret_cast<PremiumAdditionalView*>(m_premium.m_additionalBase);
    premium->m_output0 = static_cast<unsigned int>(premium->m_base0 * addInfo->m_value0) / 100;
    premium->m_output1 = static_cast<unsigned int>(premium->m_base1 * addInfo->m_value1) / 100;
    premium->m_output2 = static_cast<short>(
        (static_cast<unsigned int>(addInfo->m_value2) * premium->m_baseRate[0]) / 100);
    premium->m_output3 = static_cast<short>(
        (static_cast<unsigned int>(addInfo->m_value3) * premium->m_baseRate[1]) / 100);
    premium->m_output4 = static_cast<short>(
        (static_cast<unsigned int>(addInfo->m_value4) * premium->m_baseRate[2]) / 100);
    premium->m_output5 = static_cast<short>(
        (static_cast<unsigned int>(addInfo->m_value5) * premium->m_baseRate[3]) / 100);
    for (int i = 0; i < 4; ++i)
    {
        premium->m_output6[i] = static_cast<short>(
            (static_cast<int>(addInfo->m_loopValues[i]) * premium->m_scale[i]) / 100);
    }
    premium->m_output10 = (addInfo->m_value3a * premium->m_multiplier0) / 100;
    premium->m_output11 = static_cast<short>(addInfo->m_value3e * premium->m_multiplier1 / 100);
    premium->m_output12 = static_cast<short>(addInfo->m_value10 * premium->m_multiplier2 / 100);
    premium->m_output13 = static_cast<unsigned int>(premium->m_multiplier3 * addInfo->m_value42) / 100;
    premium->m_output14 = static_cast<short>(addInfo->m_value46 * premium->m_multiplier4 / 100);
    premium->m_output15 = static_cast<short>(addInfo->m_loopValues[3] * premium->m_multiplier5 / 100);
    premium->m_output16 = static_cast<short>(addInfo->m_value4a * premium->m_multiplier6 / 100);
    premium->m_output17 = static_cast<short>(addInfo->m_loopValues[1] * premium->m_multiplier7 / 100);
}

SUserPremiumInfo* CUserPremium::CheckPremiumTimeout(SUserPremiumInfo* out) const
{
    int curTime = GlobalData::s_systemTime_.getCurSec();
    out->m_type = 0;
    for (int i = 1; i <= 0x69; ++i)
    {
        const SUserPremiumInfo& entry = m_entries[i - 1];
        PremiumDefinitionView* definition = GetPremiumDefinition(entry.m_type);
        if (entry.m_type != 0 && definition->m_enabled != 0 &&
            entry.m_state != 3 && entry.m_end <= curTime)
        {
            *out = entry;
            return out;
        }
    }
    return out;
}

void CUserPremium::setPremiumState(int type, int state)
{
    if (type >= 1 && type <= 0x69)
        m_entries[type - 1].m_state = state;
}

bool CUserPremium::_CheckApply(int type, int yday)
{
    PremiumDefinitionView* definition = GetPremiumDefinition(type);
    if (definition->m_applyDayMask == -1)
        return true;
    return definition->m_applyDayMask == yday;
}

void CUserPremium::GetPremiumInfoList(std::vector<SUserPremiumInfo>& list, int type)
{
    list.clear();
    for (int i = 0; i < 0x69; ++i)
    {
        const SUserPremiumInfo& entry = m_entries[i];
        if (entry.m_type != 0 && (type == 0x270f || entry.m_type == type))
            list.push_back(entry);
    }
}

SUserPremiumInfo* CUserPremium::GetPremiumInfo(int type) const
{
    if (type >= 1 && type <= 0x69)
        return const_cast<SUserPremiumInfo*>(&m_entries[type - 1]);
    return const_cast<SUserPremiumInfo*>(m_entries);
}

int CUserPremium::GetReturnItemRate(int type) const
{
    std::map<int, int>::const_iterator it = m_premium.m_returnItemMap.find(type);
    if (it == m_premium.m_returnItemMap.end())
        return 0;
    return it->second;
}

bool CUserPremium::IsRestrictedPremium(int type)
{
    return (type >= 60000 && type <= 69999);
}

void CUserPremium::GetAdvantageItem(int type, std::vector<std::pair<int, int> >& items)
{
    items.clear();
    SUserPremiumInfo* entry = GetPremiumInfo(type);
    if (entry->m_type == 0 || entry->m_type >= 0x6a)
        return;
    PremiumDefinitionView* premiumInfo = GetPremiumDefinition(entry->m_type);
    if (premiumInfo->m_type != 1)
        return;
    for (int i = 0; i < 5; ++i)
    {
        if (premiumInfo->m_items[i].m_itemIndex != 0)
        {
            items.push_back(std::make_pair(premiumInfo->m_items[i].m_itemIndex,
                                            premiumInfo->m_items[i].m_count));
        }
    }
}

int CUserPremium::SetPremiumItemData(unsigned long key, const void* data)
{
    if (key == 0)
        return 1;

    static std::map<unsigned long, const void*> s_premiumItemDataMap;
    std::map<unsigned long, const void*>::iterator it = s_premiumItemDataMap.find(key);
    if (it != s_premiumItemDataMap.end())
        return 0;

    s_premiumItemDataMap[key] = data;
    return 1;
}

} // namespace WongWork
