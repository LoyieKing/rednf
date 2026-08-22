// df_game_r Game/ CUserPremium 实现（独立 TU）。
// 声明以 CUser.h 为准；布局沿用 CUser.h 的 m_pad[0x97c] 模型，本 TU 通过
// 明确偏移访问 ORIG 字段（参照 docs/class_func_reports/WongWork__CUserPremium）。
#include "CUser.h"
#include "GlobalData.h"

#include <map>
#include <string.h>
#include <time.h>


namespace WongWork
{

// ---- 文件级容器近似（ORIG 中 returnItemMap 为 CUserPremium+0x964 的 std::map，
//      premiumItemDataMap 为全局 g_map_premium_item_data；LoadPremiumInfo 未实现，
//      容器按空处理，仅保证编译与安全访问）----
static std::map<int, int> s_returnItemMap;
static std::map<unsigned long, STPremiumItemData> s_premiumItemDataMap;
// ---------------------------------------------------------------------------
// g_SPremiumInfo 分区视图。g_SPremiumInfo[type] 定义每类 premium，stride 0x120。
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
struct PremiumItemView
{
    int m_itemIndex;   // +0x00
    int m_count;       // +0x04
};

struct PremiumDefinitionView
{
    int m_type;                 // +0x00
    int m_enabled;              // +0x04
    PremiumItemView m_items[5]; // +0x08（5 * 8 = 0x28）
    char m_reserved30[0x40];    // +0x30 .. +0x6f
    unsigned short m_advantageExpRate;      // +0x70
    int m_advantageFatigueRate;             // +0x74
    int m_advantagePremiumCount;            // +0x78
    int m_needDailyCheck;                   // +0x7c
    char m_reserved80[0x18];                // +0x80 .. +0x97
    char m_overEquipFlag;                   // +0x98
    char m_reserved99[0x0b];                // +0x99 .. +0xa3
    int m_applyDayMask[7];                  // +0xac（7 天允许掩码；[0]==-1 表示每天）
    char m_reservedc8[0x58];                // +0xc8 .. +0x11f
};
#pragma pack(pop)

static const PremiumDefinitionView* GetPremiumDefinition(int type)
{
    return &reinterpret_cast<const PremiumDefinitionView*>(GlobalData::g_SPremiumInfo)[type];
}

// ---------------------------------------------------------------------------
// CUserPremium 字段（ORIG 偏移，位于 m_pad 内）
//   0x000 entries[0x69]   0x848 normalCount   0x84c restrictedCount
//   0x850 outFatigueRate  0x854 outPremiumCount  0x858 outExpRate
//   0x85c overEquipFlag   0x85e overSkillLevel   0x860 field860   0x862 field862
//   0x864 additionalInfo[0x58]  0x8bc field8bc   0x8bd additionalBase   0x90f additionalOut
//   0x964 returnItemMap（ORIG 为 std::map<int,int>；本 TU 用文件级 map 近似）
// ---------------------------------------------------------------------------
static SUserPremiumInfo* Entries(CUserPremium* self)
{
    return reinterpret_cast<SUserPremiumInfo*>(self->m_pad);
}

unsigned short CUserPremium::getOverSkillLevel() const
{
    return *reinterpret_cast<const unsigned short*>(&m_pad[0x85e]);
}

int CUserPremium::GetOverEquipableLevel(ENUM_EQUIPMENTTYPE type) const
{
    return *reinterpret_cast<const int*>(&m_pad[0x218 + (int)type * 4 + 4]);
}

int CUserPremium::GetAdvantageExpRate() const
{
    return *reinterpret_cast<const int*>(&m_pad[0x858]);
}

short CUserPremium::GetAdvantageFatigueRate() const
{
    return *reinterpret_cast<const short*>(&m_pad[0x850]);
}

int CUserPremium::GetAdvPremiumCount() const
{
    return *reinterpret_cast<const int*>(&m_pad[0x848]);
}

void CUserPremium::InitPremium()
{
    // ORIG：memset(this, 0, 0x848) —— 前缀 + entries
    memset(m_pad, 0, 0x848);
    *(int*)&m_pad[0x848] = 0;                // normalCount
    *(int*)&m_pad[0x84c] = 0;                // restrictedCount
    *(short*)&m_pad[0x850] = 0;              // outFatigueRate
    *(int*)&m_pad[0x854] = 0;                // outPremiumCount
    *(int*)&m_pad[0x858] = 0;                // outExpRate
    m_pad[0x85c] = 0;                        // overEquipFlag
    *(short*)&m_pad[0x85e] = 0;              // overSkillLevel
    *(short*)&m_pad[0x860] = 0;              // field860
    m_pad[0x862] = 0;                        // field862
    memset(&m_pad[0x864], 0, 0x58);          // additionalInfo[0x58]
    m_pad[0x8bc] = 0;                        // field8bc
    memset(&m_pad[0x8bd], 0, 0x58);          // additionalBase
    memset(&m_pad[0x90f], 0, 0x55);          // additionalOut
    s_returnItemMap.clear();                 // returnItemMap
}

void CUserPremium::AddPremium(int type, int start, int end, int flag)
{
    SUserPremiumInfo* entry = Entries(this);
    entry += type;
    if (entry->m_type != 0)
        RemovePremium(type);
    entry->m_type = type;
    entry->m_start = start;
    entry->m_end = end;
    entry->m_flag = flag;
    const PremiumDefinitionView* def = GetPremiumDefinition(type);
    if (def->m_applyDayMask[0] != -1)
        m_pad[0x862] = 1;
    startPremium(type, true);
}

void CUserPremium::RemovePremium(int type)
{
    SUserPremiumInfo* entry = Entries(this);
    if (entry[type].m_state != 2)
    {
        entry[type].m_type = 0;
        const PremiumDefinitionView* def = GetPremiumDefinition(type);
        if (def->m_type == 1)
        {
            int c = *(int*)&m_pad[0x848] - 1;
            *(int*)&m_pad[0x848] = c > 0 ? c : 0;
        }
        else
        {
            int c = *(int*)&m_pad[0x84c] - 1;
            *(int*)&m_pad[0x84c] = c > 0 ? c : 0;
        }
    }
}

int CUserPremium::startPremium(int type, bool force)
{
    SUserPremiumInfo* entry = Entries(this);
    if (!force && entry[type].m_state != 2)
        return 0;
    int curTime = GlobalData::s_systemTime_.getCurSec();
    const PremiumDefinitionView* def = GetPremiumDefinition(type);
    if (def->m_enabled != 0)
    {
        if (curTime < entry[type].m_start)
        {
            entry[type].m_state = 2;
            return 0;
        }
        if (entry[type].m_end <= curTime)
            return 0;
    }
    if (def->m_type == 1)
        ++(*(int*)&m_pad[0x848]);
    else
        ++(*(int*)&m_pad[0x84c]);
    entry[type].m_state = 1;
    return 1;
}

void CUserPremium::ReCalcAdvantage()
{
    *(short*)&m_pad[0x850] = 0;   // outFatigueRate
    *(int*)&m_pad[0x854] = 0;     // outPremiumCount
    *(int*)&m_pad[0x858] = 0;     // outExpRate
    m_pad[0x85c] = 0;             // overEquipFlag
    m_pad[0x8bc] = 0;             // field8bc
    memset(&m_pad[0x864], 0, 0x58);
    memset(&m_pad[0x8bd], 0, 0x58);
    memset(&m_pad[0x90f], 0, 0x55);
    *(short*)&m_pad[0x85e] = 0;   // overSkillLevel
    *(short*)&m_pad[0x860] = 0;   // field860
    s_returnItemMap.clear();

    int now = GlobalData::s_systemTime_.getCurSec();
    tm localTm;
    time_t nowT = (time_t)now;
    localtime_r(&nowT, &localTm);

    SUserPremiumInfo* entries = Entries(this);
    for (int type = 1; type <= 0x69; ++type)
    {
        int entryType = entries[type].m_type;
        if (entryType == 0 || entryType >= 0x6a)
            continue;
        if (entries[type].m_state == 2)
            continue;
        const PremiumDefinitionView* def = GetPremiumDefinition(entryType);
        if (def->m_type != 1)
            continue;
        if (!_CheckApply(entryType, localTm))
            continue;
        if (def->m_needDailyCheck != 0)
            m_pad[0x85c] = 1;
        *(int*)&m_pad[0x858] += def->m_advantageExpRate;
        *(int*)&m_pad[0x854] += def->m_advantagePremiumCount;
        *(short*)&m_pad[0x850] =
            (short)(*(short*)&m_pad[0x850] + def->m_advantageFatigueRate);
        if (def->m_overEquipFlag != 0)
            m_pad[0x8bc] = 1;
    }
}

void CUserPremium::setPremiumState(ENUM_PREMIUM_TYPE type, ENUM_PREMIUM_STATE state)
{
    Entries(this)[(int)type].m_state = (int)state;
}

bool CUserPremium::_CheckApply(int type, tm t)
{
    const PremiumDefinitionView* def = GetPremiumDefinition(type);
    if (def->m_applyDayMask[0] == -1)
        return true;
    for (int i = 0; i < 7; ++i)
    {
        if (def->m_applyDayMask[i] == t.tm_yday)
            return true;
    }
    return false;
}

int CUserPremium::SetPremiumItemData(unsigned long key, const STPremiumItemData& data)
{
    if (key == 0)
        return 1;
    if (s_premiumItemDataMap.find(key) != s_premiumItemDataMap.end())
        return 0;
    s_premiumItemDataMap[key] = data;
    return 1;
}

bool CUserPremium::IsRestrictedPremium(int type)
{
    return (type >= 60000 && type <= 69999);
}

void CUserPremium::RecalcAdditionalInfo(CUser const* user)
{
    if (user->getCurCharacR() == 0 || GetAdvPremiumCount() == 0)
        return;

    const unsigned char* addInfo =
        reinterpret_cast<const unsigned char*>(user->getCurCharacAddInfoR());
    const unsigned char* base =
        reinterpret_cast<const unsigned char*>(&m_pad[0x8bd]);
    unsigned char* out = reinterpret_cast<unsigned char*>(&m_pad[0x90f]);

    int v0 = *(const int*)(addInfo + 0x00);
    int v1 = *(const int*)(addInfo + 0x04);
    unsigned short v2 = *(const unsigned short*)(addInfo + 0x08);
    unsigned short v3 = *(const unsigned short*)(addInfo + 0x0a);
    unsigned short v4 = *(const unsigned short*)(addInfo + 0x0c);
    unsigned short v5 = *(const unsigned short*)(addInfo + 0x0e);

    // base rates / multipliers（ORIG 存储于 0x8bd 区域）
    int b0 = *(const int*)(base + 0x00);
    int b1 = *(const int*)(base + 0x04);
    unsigned short br[4];
    for (int i = 0; i < 4; ++i)
    {
        br[i] = *(const unsigned short*)(base + 0x08 + i * 2);
    }
    unsigned short sc[4];
    for (int i = 0; i < 4; ++i)
    {
        sc[i] = *(const unsigned short*)(base + 0x10 + i * 2);
    }

    *(int*)(out + 0x00) = (unsigned int)(b0 * v0) / 100;
    *(int*)(out + 0x04) = (unsigned int)(b1 * v1) / 100;
    *(unsigned short*)(out + 0x08) = (unsigned short)(v2 * br[0] / 100);
    *(unsigned short*)(out + 0x0a) = (unsigned short)(v3 * br[1] / 100);
    *(unsigned short*)(out + 0x0c) = (unsigned short)(v4 * br[2] / 100);
    *(unsigned short*)(out + 0x0e) = (unsigned short)(v5 * br[3] / 100);
    for (int i = 0; i < 4; ++i)
    {
        int loopVal = *(const int*)(addInfo + 0x10 + i * 4);
        *(unsigned short*)(out + 0x10 + i * 2) =
            (unsigned short)(loopVal * sc[i] / 100);
    }

    int m0 = *(const int*)(addInfo + 0x3a);
    unsigned short m1 = *(const unsigned short*)(addInfo + 0x3e);
    unsigned short m2 = *(const unsigned short*)(addInfo + 0x40);
    int m3 = *(const int*)(addInfo + 0x42);
    unsigned short m4 = *(const unsigned short*)(addInfo + 0x46);
    unsigned short m5 = *(const unsigned short*)(addInfo + 0x48);
    unsigned short m6 = *(const unsigned short*)(addInfo + 0x4a);

    int q0 = *(const int*)(base + 0x3a);     // multiplier0
    unsigned short q1 = *(const unsigned short*)(base + 0x3e);
    unsigned short q2 = *(const unsigned short*)(base + 0x40);
    int q3 = *(const int*)(base + 0x42);
    unsigned short q4 = *(const unsigned short*)(base + 0x46);
    unsigned short q5 = *(const unsigned short*)(base + 0x48);
    unsigned short q6 = *(const unsigned short*)(base + 0x4a);

    *(int*)(out + 0x34) = (m0 * q0) / 100;
    *(unsigned short*)(out + 0x38) = (unsigned short)(m1 * q1 / 100);
    *(unsigned short*)(out + 0x3a) = (unsigned short)(m2 * q2 / 100);
    *(int*)(out + 0x3c) = (unsigned int)(q3 * m3) / 100;
    *(unsigned short*)(out + 0x40) = (unsigned short)(m4 * q4 / 100);
    *(unsigned short*)(out + 0x42) = (unsigned short)(m5 * q5 / 100);
    *(unsigned short*)(out + 0x44) = (unsigned short)(m6 * q6 / 100);
}

SUserPremiumInfo CUserPremium::CheckPremiumTimeout() const
{
    SUserPremiumInfo result;
    result.m_type = 0;
    int curTime = GlobalData::s_systemTime_.getCurSec();
    const SUserPremiumInfo* entries = Entries(const_cast<CUserPremium*>(this));
    for (int i = 1; i <= 0x69; ++i)
    {
        const SUserPremiumInfo& entry = entries[i];
        if (entry.m_type == 0)
            continue;
        if (GetPremiumDefinition(entry.m_type)->m_enabled == 0)
            continue;
        if (entry.m_state == 3)
            continue;
        if (entry.m_end <= curTime)
        {
            result = entry;
            break;
        }
    }
    return result;
}

void CUserPremium::GetPremiumInfoList(std::vector<SUserPremiumInfo>& list, int type) const
{
    list.clear();
    const SUserPremiumInfo* entries = Entries(const_cast<CUserPremium*>(this));
    for (int i = 1; i < 0x6a; ++i)
    {
        const SUserPremiumInfo& entry = entries[i];
        if (entry.m_type != 0 && (type == 0x270f || entry.m_type == type))
            list.push_back(entry);
    }
}

SUserPremiumInfo* CUserPremium::GetPremiumInfo(int type) const
{
    if (type < 0x6a && type != 0x270f)
        return Entries(const_cast<CUserPremium*>(this)) + type;
    return Entries(const_cast<CUserPremium*>(this));
}

int CUserPremium::GetReturnItemRate(int type) const
{
    std::map<int, int>::const_iterator it = s_returnItemMap.find(type);
    if (it == s_returnItemMap.end())
        return 0;
    return it->second;
}

bool CUserPremium::CheckPremium(int type) const
{
    const SUserPremiumInfo* info = GetPremiumInfo(type);
    return info->m_type != 0 && info->m_state != 3;
}

std::vector<std::pair<int, int> > CUserPremium::GetAdvantageItem(int type)
{
    std::vector<std::pair<int, int> > items;
    SUserPremiumInfo* entry = Entries(this);
    if (entry[type].m_type == 0 || entry[type].m_type >= 0x6a)
        return items;
    const PremiumDefinitionView* def = GetPremiumDefinition(entry[type].m_type);
    if (def->m_type != 1)
        return items;
    for (int i = 0; i < 5; ++i)
    {
        if (def->m_items[i].m_itemIndex != 0)
        {
            items.push_back(std::make_pair(def->m_items[i].m_itemIndex,
                                           def->m_items[i].m_count));
        }
    }
    return items;
}

} // namespace WongWork