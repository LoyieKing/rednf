// df_game_r Game/ CUserPremium 实现（独立 TU）。
// 声明以 CUser.h 为准；布局沿用 CUser.h 的 m_pad[0x97c] 模型，本 TU 通过
// 明确偏移访问 ORIG 字段（参照 docs/class_func_reports/WongWork__CUserPremium）。
#include "CUser.h"
#include "GlobalData.h"
#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanInt/ScanStr（ORIG 真实符号）

#include <string>
#include <vector>

#include <map>
#include <string.h>
#include <time.h>


// ORIG 全局 g_map_premium_item_data（0x948b198 B，C 符号名）：map<unsigned long,
// STPremiumItemData>。由 SetPremiumItemData（0x86af70a）写入，
// respond_special_item_premium（CCeraShop_Special.cpp）查询。全局命名空间定义。
std::map<unsigned long, STPremiumItemData> g_map_premium_item_data;

// ===========================================================================
// PremiumList_New.etc 解析链（ORIG：importPremiumList 0x08905d95、
// stPremiumTable_t ctor/dtor 0x86b0898/0x86b0910、clean 0x891ae68、
// premium_script_parsing::STPremiumItemData ctor 0x891ae4c）。
// 类型布局逐偏移取自 ORIG 反汇编；全局容器 g_vecPremiumOverEquipableInfo /
// g_vecPremiumReturnItemInfo（B 0x0948b180/0x0948b18c，ReCalcAdvantage 按
// index 消费）。LoadPremiumInfo(0x86aeca4) 为其唯一工程内调用方。
// ===========================================================================
namespace premium_script_parsing
{

struct STPremiumItemData
{
    unsigned char m_termFlag;      // +0x00 [term] 值截断（[item] 块内）
    unsigned char m_restrictFlag;  // +0x01 [restrict] 值截断
    unsigned short m_pad02;        // +0x02 ctor 不清零（ORIG 仅写 b0/b1/key）
    unsigned int m_key;            // +0x04 [item] 首参（目标物品 idx）

    STPremiumItemData()            // ORIG W 0x891ae4c
    {
        m_termFlag = 0;
        m_restrictFlag = 0;
        m_key = 0;
    }
};  // sizeof 8

}  // namespace premium_script_parsing

struct SPremiumOverEquipableInfo
{
    int m_count;        // +0x00 组内标签数（ScanInt 失败前已写入）
    int m_slots[26];    // +0x04 标签分类值（[hat avatar]=0 .. [magic stone]=0x15）
};  // memset 0x6c；仅前 22 槽被写入
typedef char spremiumoverequipableinfo_size_0x6c
    [(sizeof(SPremiumOverEquipableInfo) == 0x6c) ? 1 : -1];

struct SPremiumReturnItemInfo
{
    int m_count;        // +0x00 组内 int 数
    int m_items[10];    // +0x04..+0x2b
};  // memset 0x2c
typedef char spremiumreturniteminfo_size_0x2c
    [(sizeof(SPremiumReturnItemInfo) == 0x2c) ? 1 : -1];

struct stPremiumTable_t
{
    struct Pair
    {
        int first;
        int second;
    };

    stPremiumTable_t();   // ORIG W 0x86b0898（仅三个含资源成员子构造）
    ~stPremiumTable_t();  // ORIG W 0x86b0910
    void clean();         // ORIG W 0x891ae68

    int m_kind;                        // +0x000 [attr]: [bonus]=1 / [term]=2
    int m_checkTerm;                   // +0x004 [term]
    int m_expireTime;                  // +0x008 [expire time]
    Pair m_advantageItems[5];          // +0x00c [items] (item,count)*n（无上界检查）
    unsigned short m_fatigueRate;      // +0x034 [fatigue]
    unsigned short m_pad036;
    int m_coin;                        // +0x038 [coin]
    int m_expRate;                     // +0x03c [exp]
    unsigned char m_unlimitFatigue;    // +0x040 [unlimit fatigue]（v==1）
    unsigned short m_overSkillLevel;   // +0x042 [over skill]
    unsigned short m_cerashopDiscountRate;  // +0x044 [cerashop discount rate]
    unsigned short m_pad046;
    int m_bonusExp;                    // +0x048 [bonus exp]（失败→整体 return 0）
    int m_questItemDropRate;           // +0x04c [quest item drop rate]（同上怪癖）
    std::vector<unsigned int> m_independentDropRates;  // +0x050
    int m_useTargetServer;             // +0x05c [apply target server]：self=1/all=0
    unsigned char m_goldBonus;         // +0x060 [gold bonus]：[true]=1/[false]=0
    unsigned char m_pad061[3];
    int m_overEquipCount;              // +0x064 [over equipinfo]，>5→0x1591
    Pair m_overEquipPairs[5];          // +0x068
    int m_returnItemCount;             // +0x090 [return iteminfo]，>3→0x15a6
    Pair m_returnItemPairs[3];         // +0x094
    int m_applyDayMask[7];             // +0x0ac [apply date]（clean 置 -1）
    int m_hpMax;                       // +0x0c8 [HP MAX]
    int m_mpMax;                       // +0x0cc [MP MAX]
    unsigned short m_physicalAttack;   // +0x0d0
    unsigned short m_physicalDefense;  // +0x0d2
    unsigned short m_magicalAttack;    // +0x0d4
    unsigned short m_magicalDefense;   // +0x0d6
    unsigned short m_fireResistance;   // +0x0d8
    unsigned short m_waterResistance;  // +0x0da
    unsigned short m_darkResistance;   // +0x0dc
    unsigned short m_lightResistance;  // +0x0de
    unsigned short m_rawE0[17];        // +0x0e0..+0x100 解析器不写，加载器照抄栈值
    unsigned short m_pad102;
    int m_inventoryLimit;              // +0x104 [inventory limit]
    unsigned short m_hpRegenSpeed;     // +0x108
    unsigned short m_mpRegenSpeed;     // +0x10a
    unsigned short m_moveSpeed;        // +0x10c
    unsigned short m_attackSpeed;      // +0x10e
    unsigned short m_castSpeed;        // +0x110
    unsigned short m_hitRecovery;      // +0x112
    unsigned short m_jumpPower;        // +0x114
    unsigned short m_pad116;
    int m_field118;                    // +0x118 解析器不写，加载器照抄栈值
    std::string m_name;                // +0x11c [name]
    unsigned int m_targetItemKey;      // +0x120 [target item]（失败→return 0 怪癖）
    std::vector<premium_script_parsing::STPremiumItemData> m_premiumItems;  // +0x124
};  // 总计 0x130
typedef char stpremiumtable_size_0x130[(sizeof(stPremiumTable_t) == 0x130) ? 1 : -1];

stPremiumTable_t::stPremiumTable_t() {}

stPremiumTable_t::~stPremiumTable_t() {}

void stPremiumTable_t::clean()
{
    // ORIG 0x891ae68：yday 掩码全部 -1；kind/checkTerm/expireTime/
    // overSkill/discount/useTargetServer 清零（顺序照抄）。
    for (int i = 0; i <= 6; ++i)
        m_applyDayMask[i] = -1;
    m_kind = 0;
    m_checkTerm = 0;
    m_expireTime = 0;
    m_overSkillLevel = 0;
    m_cerashopDiscountRate = 0;
    m_useTargetServer = 0;
}

// ORIG 全局容器（变量名不 mangle，符号与 B 0x0948b180/0x0948b18c 一致）
std::vector<SPremiumOverEquipableInfo> g_vecPremiumOverEquipableInfo;
std::vector<SPremiumReturnItemInfo> g_vecPremiumReturnItemInfo;

int importPremiumList(const char* path,
                      stPremiumTable_t* table,
                      std::vector<SPremiumOverEquipableInfo>& overVec,
                      std::vector<SPremiumReturnItemInfo>& returnVec)
{
    if (!loadRDARScriptFile("", path))
        return 0x14a6;

    std::string token;
    std::string value;
    bool ok = false;
    bool itemOk = false;
    stPremiumTable_t* cur = table;
    // ORIG -0x60 槽：跨组保留的标签分类值（未知标签时写入残留值）。
    int equipCase = 0;

    for (int i = 0; i <= 0x69; ++i)
        table[i].clean();

    while (true)
    {
        if (!ScanType(token, true))
            return 0;

        if (token == "[overequip list]")
        {
            SPremiumOverEquipableInfo elem;
            memset(&elem, 0, sizeof(elem));
            overVec.push_back(elem);
            while (true)
            {
                elem.m_count = ScanInt(&ok);
                if (!ok)
                    break;
                for (int j = 0; elem.m_count > j; ++j)
                {
                    if (!ScanStr(&value))
                        return 0x14c5;
                    if (value == "[hat avatar]")          equipCase = 0x0;
                    else if (value == "[hair avatar]")     equipCase = 0x1;
                    else if (value == "[face avatar]")     equipCase = 0x2;
                    else if (value == "[neck avatar]")     equipCase = 0x3;
                    else if (value == "[coat avatar]")     equipCase = 0x4;
                    else if (value == "[pants avatar]")    equipCase = 0x5;
                    else if (value == "[breast avatar]")   equipCase = 0x6;
                    else if (value == "[waist avatar]")    equipCase = 0x7;
                    else if (value == "[skin avatar]")     equipCase = 0x8;
                    else if (value == "[aurora avatar]")   equipCase = 0x9;
                    else if (value == "[weapon]")          equipCase = 0xa;
                    else if (value == "[title name]")      equipCase = 0xb;
                    else if (value == "[coat]")            equipCase = 0xc;
                    else if (value == "[shoulder]")        equipCase = 0xd;
                    else if (value == "[pants]")           equipCase = 0xe;
                    else if (value == "[shoes]")           equipCase = 0xf;
                    else if (value == "[waist]")           equipCase = 0x10;
                    else if (value == "[amulet]")          equipCase = 0x11;
                    else if (value == "[wrist]")           equipCase = 0x12;
                    else if (value == "[ring]")            equipCase = 0x13;
                    else if (value == "[support]")         equipCase = 0x14;
                    else if (value == "[magic stone]")     equipCase = 0x15;
                    elem.m_slots[j] = equipCase;
                }
                overVec.push_back(elem);
            }
            continue;
        }

        if (token == "[returnitem list]")
        {
            SPremiumReturnItemInfo elem;
            memset(&elem, 0, sizeof(elem));
            returnVec.push_back(elem);
            while (true)
            {
                elem.m_count = ScanInt(&ok);
                if (!ok)
                    break;
                for (int k = 0; elem.m_count > k; ++k)
                {
                    elem.m_items[k] = ScanInt(&ok);
                    if (!ok)
                        return 0x1512;
                }
                returnVec.push_back(elem);
            }
            continue;
        }

        if (token == "[type]")
        {
            int idx = ScanInt((bool*)0);
            if (idx > 0x69)
                return 0x151b;
            cur = table + idx;
            continue;
        }

        if (token == "[name]")
        {
            if (!ScanStr(&value))
                return 0x1520;
            cur->m_name = value;
            continue;
        }

        if (token == "[attr]")
        {
            if (!ScanStr(&value))
                return 0x1525;
            if (value == "[term]")
            {
                cur->m_kind = 2;
                continue;
            }
            if (value == "[bonus]")
            {
                cur->m_kind = 1;
                continue;
            }
            return 0x152b;
        }

        if (token == "[term]")
        {
            cur->m_checkTerm = ScanInt((bool*)0);
            continue;
        }

        if (token == "[expire time]")
        {
            cur->m_expireTime = ScanInt((bool*)0);
            continue;
        }

        if (token == "[items]")
        {
            for (int n = 0;; ++n)
            {
                int itemIdx = ScanInt(&ok);
                if (!ok)
                    break;
                int cnt = ScanInt(&ok);
                if (!ok)
                    break;
                cur->m_advantageItems[n].first = itemIdx;
                cur->m_advantageItems[n].second = cnt;
            }
            continue;
        }

        if (token == "[fatigue]")
        {
            cur->m_fatigueRate = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[unlimit fatigue]")
        {
            cur->m_unlimitFatigue = (ScanInt((bool*)0) == 1);
            continue;
        }

        if (token == "[bonus exp]")
        {
            cur->m_bonusExp = ScanInt(&ok);
            if (!ok)
                return 0;  // ORIG 怪癖（0x8906f2d）：此处扫描失败按成功返回 0
            continue;
        }

        if (token == "[quest item drop rate]")
        {
            cur->m_questItemDropRate = ScanInt(&ok);
            if (!ok)
                return 0;  // 同上怪癖
            continue;
        }

        if (token == "[independent drop rate]")
        {
            while (true)
            {
                unsigned int rate = (unsigned int)ScanInt(&ok);
                if (!ok)
                    break;
                cur->m_independentDropRates.push_back(rate);
            }
            continue;
        }

        if (token == "[exp]")
        {
            cur->m_expRate = ScanInt((bool*)0);
            continue;
        }

        if (token == "[coin]")
        {
            cur->m_coin = ScanInt((bool*)0);
            continue;
        }

        if (token == "[gold bonus]")
        {
            ScanStr(&value);  // ORIG 不检查返回值（0x890669c 无判空）
            if (value == "[true]")
            {
                cur->m_goldBonus = 1;
                continue;
            }
            if (value == "[false]")
            {
                cur->m_goldBonus = 0;
                continue;
            }
            return 0x156d;
        }

        if (token == "[over skill]")
        {
            cur->m_overSkillLevel = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[cerashop discount rate]")
        {
            cur->m_cerashopDiscountRate = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[apply target server]")
        {
            if (!ScanStr(&value))
                return 0x1583;
            if (value == "self")
            {
                cur->m_useTargetServer = 1;
                continue;
            }
            if (value == "all")
            {
                cur->m_useTargetServer = 0;
                continue;
            }
            return 0x1588;
        }

        if (token == "[over equipinfo]")
        {
            cur->m_overEquipCount = ScanInt((bool*)0);
            if (cur->m_overEquipCount > 5)
                return 0x1591;
            for (int m = 0; cur->m_overEquipCount > m; ++m)
            {
                int itemId = ScanInt(&ok);
                if (!ok)
                    break;
                int cnt = ScanInt(&ok);
                if (!ok)
                    break;
                cur->m_overEquipPairs[m].first = itemId;
                cur->m_overEquipPairs[m].second = cnt;
            }
            continue;
        }

        if (token == "[return iteminfo]")
        {
            cur->m_returnItemCount = ScanInt((bool*)0);
            if (cur->m_returnItemCount > 3)
                return 0x15a6;
            int slot = 0;
            for (int n = 0; cur->m_returnItemCount > n; ++n)
            {
                if (slot > 2)
                    return 0x15aa;
                int itemId = ScanInt(&ok);
                if (!ok)
                    break;
                int cnt = ScanInt(&ok);
                if (!ok)
                    break;
                cur->m_returnItemPairs[slot].first = itemId;
                cur->m_returnItemPairs[slot].second = cnt;
                ++slot;
            }
            continue;
        }

        if (token == "[apply date]")
        {
            for (int d = 0;; ++d)
            {
                int v = ScanInt(&ok);
                if (!ok)
                    break;
                cur->m_applyDayMask[d] = v;
            }
            continue;
        }

        if (token == "[HP MAX]")
        {
            cur->m_hpMax = ScanInt((bool*)0);
            continue;
        }

        if (token == "[MP MAX]")
        {
            cur->m_mpMax = ScanInt((bool*)0);
            continue;
        }

        if (token == "[physical attack]")
        {
            cur->m_physicalAttack = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[physical defense]")
        {
            cur->m_physicalDefense = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[magical attack]")
        {
            cur->m_magicalAttack = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[magical defense]")
        {
            cur->m_magicalDefense = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[fire resistance]")
        {
            cur->m_fireResistance = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[water resistance]")
        {
            cur->m_waterResistance = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[dark resistance]")
        {
            cur->m_darkResistance = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[light resistance]")
        {
            cur->m_lightResistance = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[inventory limit]")
        {
            cur->m_inventoryLimit = ScanInt((bool*)0);
            continue;
        }

        if (token == "[HP regen speed]")
        {
            cur->m_hpRegenSpeed = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[MP regen speed]")
        {
            cur->m_mpRegenSpeed = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[move speed]")
        {
            cur->m_moveSpeed = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[attack speed]")
        {
            cur->m_attackSpeed = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[cast speed]")
        {
            cur->m_castSpeed = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[hit recovery]")
        {
            cur->m_hitRecovery = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[jump power]")
        {
            cur->m_jumpPower = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[target item]")
        {
            cur->m_targetItemKey = (unsigned int)ScanInt(&itemOk);
            if (!itemOk)
                return 0;  // ORIG 怪癖（0x8906f2c）：失败按成功返回 0
            continue;
        }

        if (token == "[item]")
        {
            premium_script_parsing::STPremiumItemData item;
            item.m_key = (unsigned int)ScanInt(&itemOk);
            if (!itemOk)
                return 0x15fb;
            while (true)
            {
                if (!ScanType(token, true))
                    return 0x1600;
                if (token == "[term]")
                {
                    int v = ScanInt(&itemOk);
                    if (!itemOk)
                        continue;
                    item.m_termFlag = (unsigned char)v;
                    continue;
                }
                if (token == "[restrict]")
                {
                    int v = ScanInt(&itemOk);
                    if (!itemOk)
                        continue;
                    item.m_restrictFlag = (unsigned char)v;
                    continue;
                }
                if (token == "[/item]")
                {
                    cur->m_premiumItems.push_back(item);
                    break;
                }
                // 其余 token：继续扫描（ORIG 8906f02 → 重入 ScanType）
            }
            continue;
        }

        // 未知顶层 token：跳过继续（ORIG 比较链全部落空 → 回循环头）
    }
}

namespace WongWork
{

// ---- 文件级容器近似（ORIG 中 returnItemMap 为 CUserPremium+0x964 的 std::map，
//      premiumItemDataMap 即全局 g_map_premium_item_data（定义见文件头）；
//      LoadPremiumInfo 未实现，容器按空处理，仅保证编译与安全访问）----
static std::map<int, int> s_returnItemMap;
// ---------------------------------------------------------------------------
// g_SPremiumInfo 权威元素类型见 SPremiumInfo.h（stride 0x120、0x6a 元素）；
// 本 TU 经 GlobalData::g_SPremiumInfo[type] 具名字段访问。
// ---------------------------------------------------------------------------
static const SPremiumInfo* GetPremiumDefinition(int type)
{
    return &GlobalData::g_SPremiumInfo[type];
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
    memset(&m_additionalInfo, 0, sizeof(_Additioal_info));  // additionalOut（+0x90f）
    m_pad[0x8bc] = 0;                        // field8bc
    memset(&m_pad[0x8bd], 0, 0x52);          // additionalBase（0x58→0x52 对齐 _Additioal_info）
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
    const SPremiumInfo* def = GetPremiumDefinition(type);
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
        const SPremiumInfo* def = GetPremiumDefinition(type);
        if (def->m_kind == 1)
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
    const SPremiumInfo* def = GetPremiumDefinition(type);
    if (def->m_checkTerm != 0)
    {
        if (curTime < entry[type].m_start)
        {
            entry[type].m_state = 2;
            return 0;
        }
        if (entry[type].m_end <= curTime)
            return 0;
    }
    if (def->m_kind == 1)
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
    memset(&m_pad[0x8bd], 0, 0x52);
    memset(&m_additionalInfo, 0, sizeof(_Additioal_info));   // +0x90f 具名成员
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
        const SPremiumInfo* def = GetPremiumDefinition(entryType);
        if (def->m_kind != 1)
            continue;
        if (!_CheckApply(entryType, localTm))
            continue;
        if (def->m_needDailyCheck != 0)
            m_pad[0x85c] = 1;
        *(int*)&m_pad[0x858] += def->m_expRate;
        *(int*)&m_pad[0x854] += def->m_premiumCount;
        *(short*)&m_pad[0x850] =
            (short)(*(short*)&m_pad[0x850] + def->m_fatigueRate);
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
    const SPremiumInfo* def = GetPremiumDefinition(type);
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
    if (g_map_premium_item_data.find(key) != g_map_premium_item_data.end())
        return 0;
    g_map_premium_item_data[key] = data;
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
    unsigned char* out =
        reinterpret_cast<unsigned char*>(&m_additionalInfo);

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
// ORIG W 0x8694a88 GetAdditionalInfo() const：sret；memcpy(this+0x90f, 0x52)。
_Additioal_info CUserPremium::GetAdditionalInfo() const
{
    _Additioal_info out;
    memcpy(&out, &m_additionalInfo, sizeof(_Additioal_info));
    return out;
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
        if (GetPremiumDefinition(entry.m_type)->m_checkTerm == 0)
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

// ORIG W 0x086b086c：返回 +0x862 字节（任一 premium 定义需每日检查时为 1）。
bool CUserPremium::IsNeedCheckEveryday()
{
    return m_pad[0x862] != 0;
}

std::vector<std::pair<int, int> > CUserPremium::GetAdvantageItem(int type)
{
    std::vector<std::pair<int, int> > items;
    SUserPremiumInfo* entry = Entries(this);
    if (entry[type].m_type == 0 || entry[type].m_type >= 0x6a)
        return items;
    const SPremiumInfo* def = GetPremiumDefinition(entry[type].m_type);
    if (def->m_kind != 1)
        return items;
    for (int i = 0; i < 5; ++i)
    {
        if (def->m_advantageItems[i].first != 0)
        {
            items.push_back(std::make_pair(def->m_advantageItems[i].first,
                                           def->m_advantageItems[i].second));
        }
    }
    return items;
}

// ---------------------------------------------------------------------------
// ORIG T 0x086aeca4 WongWork::CUserPremium::LoadPremiumInfo(char const*)。
// importPremiumList 失败→0x15c；逐记录拷贝进 g_SPremiumInfo[i]（0x130→0x120
// 字段重排，+0xc8 起写穿 _Additioal_info 子对象区）；SetPremiumItemData
// 失败：非空 vector 分支→0x1bc、空分支（key=m_targetItemKey，flag 取
// m_checkTerm 低字节）→0x1c4。全部成功返回 0（CDataManager::init 以
// "LoadPremiumInfo Fail(Line: %d)" 打印该行码）。
// ---------------------------------------------------------------------------
int CUserPremium::LoadPremiumInfo(const char* path)
{
    stPremiumTable_t table[0x6a];

    if (importPremiumList(path,
                          table,
                          g_vecPremiumOverEquipableInfo,
                          g_vecPremiumReturnItemInfo) != 0)
        return 0x15c;

    for (int i = 0; i <= 0x69; ++i)
    {
        const stPremiumTable_t& rec = table[i];
        SPremiumInfo& info = GlobalData::g_SPremiumInfo[i];

        info.m_kind = rec.m_kind;                       // +0x00
        info.m_checkTerm = rec.m_checkTerm;             // +0x04
        info.m_timeLimit = rec.m_expireTime;            // +0x08
        for (int j = 0; j < 5; ++j)                     // +0x0c 特权物品对
        {
            info.m_advantageItems[j].first = rec.m_advantageItems[j].first;
            info.m_advantageItems[j].second = rec.m_advantageItems[j].second;
        }
        info.m_fatigueRate = rec.m_fatigueRate;         // +0x34
        info.m_premiumCount = rec.m_coin;               // +0x38
        info.m_expRate = rec.m_expRate;                 // +0x3c
        info.m_needDailyCheck = rec.m_unlimitFatigue;   // +0x40
        info.m_field44 = rec.m_bonusExp;                // +0x44
        info.m_field48 = rec.m_questItemDropRate;       // +0x48
        info.m_independentDropRates = rec.m_independentDropRates;  // +0x4c vector=
        info.m_overSkillLevel = rec.m_overSkillLevel;   // +0x58 ← t+0x42
        info.m_field5a = rec.m_cerashopDiscountRate;    // +0x5a ← t+0x44
        info.m_useTargetServer = rec.m_useTargetServer; // +0x5c
        info.m_overEquipFlag = rec.m_goldBonus;         // +0x60
        info.m_field64 = rec.m_overEquipCount;          // +0x64
        for (int j = 0; j < 5; ++j)                     // +0x68 返还物品对
        {
            info.m_returnPairs68[j].first = rec.m_overEquipPairs[j].first;
            info.m_returnPairs68[j].second = rec.m_overEquipPairs[j].second;
        }
        info.m_field90 = rec.m_returnItemCount;         // +0x90
        for (int j = 0; j < 3; ++j)                     // +0x94 返还对
        {
            info.m_returnPairs94[j].first = rec.m_returnItemPairs[j].first;
            info.m_returnPairs94[j].second = rec.m_returnItemPairs[j].second;
        }
        for (int j = 0; j < 7; ++j)                     // +0xac yday 掩码
            info.m_applyDayMask[j] = rec.m_applyDayMask[j];

        _Additioal_info& additional = info.m_additionalInfo;
        additional.m_field00 = rec.m_hpMax;
        additional.m_field04 = rec.m_mpMax;
        additional.m_field08 = rec.m_physicalAttack;
        additional.m_field0a = rec.m_physicalDefense;
        additional.m_field0c = rec.m_magicalAttack;
        additional.m_field0e = rec.m_magicalDefense;
        additional.m_field10[0] = rec.m_fireResistance;
        additional.m_field10[1] = rec.m_waterResistance;
        additional.m_field10[2] = rec.m_darkResistance;
        additional.m_field10[3] = rec.m_lightResistance;
        for (int j = 0; j < 17; ++j)
            additional.m_field18[j] = rec.m_rawE0[j];
        additional.m_field3a = rec.m_inventoryLimit;
        additional.m_field3e = rec.m_hpRegenSpeed;
        additional.m_field40 = rec.m_mpRegenSpeed;
        additional.m_field42 = (unsigned int)rec.m_moveSpeed;
        additional.m_field46 = rec.m_attackSpeed;
        additional.m_field48 = rec.m_castSpeed;
        additional.m_field4a = rec.m_hitRecovery;
        additional.m_field4c = rec.m_jumpPower;
        additional.m_field4e = rec.m_field118;

        info.m_name = rec.m_name;                                      // +0x11c

        STPremiumItemData data;
        data.m_value0 = i;
        unsigned long key = (unsigned long)rec.m_targetItemKey;
        if (rec.m_premiumItems.size() == 0)
        {
            // 空表：flag 字节取已拷贝 g 表 m_checkTerm 低字节，key 用记录键。
            reinterpret_cast<unsigned char*>(&data.m_value4)[0] =
                (unsigned char)info.m_checkTerm;
            if (SetPremiumItemData(key, data) != 1)
                return 0x1c4;
        }
        else
        {
            std::vector<premium_script_parsing::STPremiumItemData>::const_iterator it;
            for (it = rec.m_premiumItems.begin();
                 it != rec.m_premiumItems.end(); ++it)
            {
                unsigned char* flags =
                    reinterpret_cast<unsigned char*>(&data.m_value4);
                flags[0] = it->m_termFlag;
                flags[1] = (unsigned char)(it->m_restrictFlag != 0);
                if (SetPremiumItemData((unsigned long)it->m_key, data) != 1)
                    return 0x1bc;
            }
        }
    }
    return 0;
}

} // namespace WongWork

// ---- _Additioal_info 字段算术（ORIG df_game_r 逐指令转写；全局作用域类）----
// add（ORIG W _ZN15_Additioal_info3addERKS_ @0x08146300）：int@+0x00/+0x04 32 位加；
//   u16@+0x08..+0x0e 显式加；循环 i=0..3 加 u16[i] @ (i+8)*2（即 m_field10[4]）；
//   循环 j=0..0x10 加 u16[j] @ 0x18+2j（即 m_field18[17]）；
//   收尾依次 +0x3a(int)、+0x3e、+0x40、+0x42(int)、+0x46、+0x48、+0x4a、+0x4c、+0x4e(int)。
//   u16 均为零扩展后 mod 2^16 运算（movzwl/add/mov），unsigned short 成员天然良构。
void _Additioal_info::add(const _Additioal_info& other)
{
    m_field00 += other.m_field00;
    m_field04 += other.m_field04;
    m_field08 += other.m_field08;
    m_field0a += other.m_field0a;
    m_field0c += other.m_field0c;
    m_field0e += other.m_field0e;
    for (int i = 0; i <= 3; ++i)
        m_field10[i] += other.m_field10[i];
    for (int j = 0; j <= 0x10; ++j)
        m_field18[j] += other.m_field18[j];
    m_field3a += other.m_field3a;
    m_field3e += other.m_field3e;
    m_field40 += other.m_field40;
    m_field42 += other.m_field42;
    m_field46 += other.m_field46;
    m_field48 += other.m_field48;
    m_field4a += other.m_field4a;
    m_field4c += other.m_field4c;
    m_field4e += other.m_field4e;
}

// subtract（ORIG W _ZN15_Additioal_info8subtractERKS_ @0x08693d04）：与 add 同构、
//   逐字段相减——唯 ORIG 对 m_field4c 与 m_field4e 两字段仍执行加法
//   （lea (%edx,%eax,1) 与 add %eax,%edx 指令实证），忠实保留该行为。
void _Additioal_info::subtract(const _Additioal_info& other)
{
    m_field00 -= other.m_field00;
    m_field04 -= other.m_field04;
    m_field08 -= other.m_field08;
    m_field0a -= other.m_field0a;
    m_field0c -= other.m_field0c;
    m_field0e -= other.m_field0e;
    for (int i = 0; i <= 3; ++i)
        m_field10[i] -= other.m_field10[i];
    for (int j = 0; j <= 0x10; ++j)
        m_field18[j] -= other.m_field18[j];
    m_field3a -= other.m_field3a;
    m_field3e -= other.m_field3e;
    m_field40 -= other.m_field40;
    m_field42 -= other.m_field42;
    m_field46 -= other.m_field46;
    m_field48 -= other.m_field48;
    m_field4a -= other.m_field4a;
    m_field4c += other.m_field4c;   // ORIG：加法（非减）
    m_field4e += other.m_field4e;   // ORIG：加法（非减）
}

// operator+=（ORIG W _ZN15_Additioal_infopLERKS_ @0x08693cea）：仅转发 add，
//   无引用返回（ORIG 函数尾未装载 this 到 eax）。
void _Additioal_info::operator+=(const _Additioal_info& other)
{
    add(other);
}