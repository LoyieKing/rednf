// df_game_r Game/ CUserPremium 实现（独立 TU）。
// 声明以 CUser.h 为准；ORIG 字段已全部具名化（m_entries / m_additionalBase /
// m_additionalInfo 等，见 CUser.h 的 WongWork::CUserPremium），本 TU 不再使用
// 裸偏移访问（参照 docs/class_func_reports/WongWork__CUserPremium）。
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
    // ORIG 0x891ae68：清零顺序照抄（循环 applyDayMask=-1 在前，随后
    // kind/checkTerm/overSkillLevel/cerashopDiscountRate/useTargetServer/
    // expireTime/unlimitFatigue/fatigueRate/coin/expRate/advantageItems/
    // goldBonus/overEquipCount/overEquipPairs/returnItemCount/returnItemPairs/
    // hpMax..field118/bonusExp/questItemDropRate/dropRates/targetItemKey/premiumItems）。
    for (int i = 0; i <= 6; ++i)
        m_applyDayMask[i] = -1;
    m_kind = 0;
    m_checkTerm = 0;
    m_overSkillLevel = 0;
    m_cerashopDiscountRate = 0;
    m_useTargetServer = 0;
    m_expireTime = 0;
    m_unlimitFatigue = 0;
    m_fatigueRate = 0;
    m_coin = 0;
    m_expRate = 0;
    memset(m_advantageItems, 0, sizeof(m_advantageItems));
    m_goldBonus = 0;
    m_overEquipCount = 0;
    memset(m_overEquipPairs, 0, sizeof(m_overEquipPairs));
    m_returnItemCount = 0;
    memset(m_returnItemPairs, 0, sizeof(m_returnItemPairs));
    memset(&m_hpMax, 0, 0x54);
    m_bonusExp = 0;
    m_questItemDropRate = 0;
    m_independentDropRates.clear();
    m_targetItemKey = 0;
    m_premiumItems.clear();
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
                    else if (value == "[coat avatar]")     equipCase = 0x3;
                    else if (value == "[pants avatar]")    equipCase = 0x4;
                    else if (value == "[shoes avatar]")    equipCase = 0x5;
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

// ---------------------------------------------------------------------------
// g_SPremiumInfo 权威元素类型见 SPremiumInfo.h（stride 0x120、0x6a 元素）；
// 本 TU 经 g_SPremiumInfo[type] 具名字段访问。
// importPremiumList 残差豁免（O=1155/N=1130，-25 散布：解析器逐字段
// string/栈槽折叠宽度差，等结构无缺语义；Main 裁决标注不追 2026-09-05）。
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// CUserPremium 字段（ORIG 偏移 → CUser.h 具名成员）
//   0x000 m_entries[0x6a]  0x848 m_normalCount  0x84c m_restrictedCount
//   0x850 outFatigueRate  0x854 outPremiumCount  0x858 outExpRate
//   0x85c overEquipFlag   0x85e overSkillLevel   0x860 field860   0x862 field862
//   0x864 m_overEquipableLevel[0x58]  0x8bc m_field8bc   0x8bd m_additionalBase   0x90f m_additionalInfo
//   0x964 m_returnItemMap（ORIG 为 std::map<int,int>；真实成员，C1/D1 自动管理）
// ---------------------------------------------------------------------------
unsigned short CUserPremium::getOverSkillLevel() const
{
    return m_overSkillLevel;
}

int CUserPremium::GetOverEquipableLevel(ENUM_EQUIPMENTTYPE type) const
{
    // ORIG：0x4(%eax,%edx,4) 且 edx = type+0x218 → this+0x864+4*type（additionalInfo[type]）
    return m_overEquipableLevel[(int)type];
}

int CUserPremium::GetAdvantageExpRate() const
{
    return m_outExpRate;
}

short CUserPremium::GetAdvantageFatigueRate() const
{
    return m_outFatigueRate;
}

int CUserPremium::GetAdvPremiumCount() const
{
    return m_normalCount;
}

// ORIG W 0x0863bd5c：*(int*)(this+0x848) + *(int*)(this+0x84c)
int CUserPremium::GetTotalPremiumCount() const
{
    return m_normalCount
         + m_restrictedCount;
}

void CUserPremium::InitPremium()
{
    // ORIG：memset(this, 0, 0x848) —— 前缀 + entries
    memset(m_entries, 0, sizeof(m_entries));
    m_normalCount = 0;                // normalCount
    m_restrictedCount = 0;                // restrictedCount
    m_outFatigueRate = 0;              // outFatigueRate
    m_outPremiumCount = 0;                // outPremiumCount
    m_outExpRate = 0;                // outExpRate
    m_overEquipFlag = false;                        // overEquipFlag
    m_overSkillLevel = 0;              // overSkillLevel
    m_ceraDiscountRate = 0;                  // +0x860（field860）
    m_field862 = 0;                          // +0x862（field862）
    // ORIG 0x86ad9e8 序：memset(+0x864,0x58) → +0x8bc → clear(0x8bd) →
    // clear(0x90f) → map.clear；_Additioal_info 走 **clear() 调用**（W 符号），
    // 非 memset（后者使 4.4 展开/直调 memset，与 ORIG call 形不符）
    memset(m_overEquipableLevel, 0, sizeof(m_overEquipableLevel));  // +0x864
    m_field8bc = 0;                                                 // +0x8bc
    m_additionalBase.clear();                                       // +0x8bd
    m_additionalInfo.clear();                                       // +0x90f
    m_returnItemMap.clear();                                        // +0x964
}

void CUserPremium::AddPremium(int type, int start, int end, int flag)
{
    // ORIG 0x86adad2：每个字段读写独立重算 this+type*0x14 链（T-C9-1，
    // 无 entry/def 指针缓存）；m_applyDayMask[0] 直读 +0xac。
    if (m_entries[type].m_type != 0)
        RemovePremium(type);
    m_entries[type].m_type = type;
    m_entries[type].m_start = start;
    m_entries[type].m_end = end;
    m_entries[type].m_flag = flag;
    if (g_SPremiumInfo[type].m_applyDayMask[0] != -1)
        m_field862 = 1;
    startPremium(type, true);
}

void CUserPremium::RemovePremium(int type)
{
    // ORIG 0x86adba6：计数递减走 std::max<int>(zero, dec) 调用形（两临时
    // 槽分支各独立，非三目）；链全重算。
    // [b27 封顶] 唯一残差 = 两分支 max 临时槽对：ORIG 4.1 按分支分配独立槽
    // 对（-0x18/-0x14 与 -0x10/-0xc，N82 不按 lifetime 复用）；4.4 复用同槽
    // （-0xc/-0x10）⇒ 全帧槽平移，源码不可定向（分支顺序/声明序不改变
    // 复用策略）。其余助记符序列全等。
    if (m_entries[type].m_state != 2)
    {
        m_entries[type].m_type = 0;
        if (g_SPremiumInfo[type].m_kind == 1)
        {
            int dec = m_normalCount + -1;
            int zero = 0;
            m_normalCount = std::max(zero, dec);
        }
        else
        {
            int dec = m_restrictedCount + -1;
            int zero = 0;
            m_restrictedCount = std::max(zero, dec);
        }
    }
}

bool CUserPremium::startPremium(int type, bool force)   // 0x086adc6a；调用点 xor $0x1 ⇒ bool（Main 代落 CUser.h 同步）
{
    // ORIG 0x86adc6a：无 entry/def 缓存；state/start/end 各独立
    // this+type*0x14 链，checkTerm/kind 直读 g_SPremiumInfo 表（T-C9-1）
    if (!force && m_entries[type].m_state != 2)
        return 0;
    int curTime = GlobalData::s_systemTime_.getCurSec();
    if (g_SPremiumInfo[type].m_checkTerm != 0)
    {
        if (curTime < m_entries[type].m_start)
        {
            m_entries[type].m_state = 2;
            return 0;
        }
        if (m_entries[type].m_end <= curTime)
            return 0;
    }
    if (g_SPremiumInfo[type].m_kind == 1)
        ++(m_normalCount);
    else
        ++(m_restrictedCount);
    m_entries[type].m_state = 1;
    return 1;
}

void CUserPremium::ReCalcAdvantage()
{
    // ORIG 0x086ae10e（O=487）：形态按 T-C9-1 全去缓存——g_SPremiumInfo[type]
    // 链（type*0x120=(t*9)<<5）与 m_entries[slot]（slot*0x14）在每个字段读取点
    // 独立重算（ORIG +0x142/0x1bc/0x1ee/0x237/0x283...每处 11-14 条），不使用
    // def/groupDef/entries 指针局部。清零段：0x850(movw)/0x854/0x858/0x85c/
    // 0x8bc/memset(0x864,0x58)/clear×2/map.clear/0x85e/0x860。
    m_outFatigueRate = 0;
    m_outPremiumCount = 0;
    m_outExpRate = 0;
    m_overEquipFlag = false;
    m_field8bc = 0;
    memset(m_overEquipableLevel, 0, sizeof(m_overEquipableLevel));
    m_additionalBase.clear();
    m_additionalInfo.clear();
    m_returnItemMap.clear();
    m_overSkillLevel = 0;
    m_ceraDiscountRate = 0;

    int now = GlobalData::s_systemTime_.getCurSec();
    time_t nowT = (time_t)now;
    tm localTm;
    localtime_r(&nowT, &localTm);

    for (int slot = 1; slot <= 0x69; ++slot)
    {
        if (m_entries[slot].m_type == 0)
            continue;
        if (g_SPremiumInfo[m_entries[slot].m_type].m_kind != 1)
            continue;
        if (m_entries[slot].m_state == 2)
            continue;
        if (!_CheckApply(m_entries[slot].m_type, localTm))
            continue;

        if (g_SPremiumInfo[m_entries[slot].m_type].m_needDailyCheck != 0)
            m_overEquipFlag = true;

        // +0x85e/+0x860：unsigned short max，ORIG 调用站点 max<unsigned short>
        m_overSkillLevel = static_cast<short>(
            std::max(static_cast<unsigned short>(m_overSkillLevel),
                     g_SPremiumInfo[m_entries[slot].m_type].m_overSkillLevel));
        m_ceraDiscountRate = std::max(
            m_ceraDiscountRate,
            g_SPremiumInfo[m_entries[slot].m_type].m_field5a);

        m_outFatigueRate = static_cast<short>(
            static_cast<unsigned short>(m_outFatigueRate) +
            static_cast<unsigned short>(
                g_SPremiumInfo[m_entries[slot].m_type].m_fatigueRate));
        m_outExpRate =
            m_outExpRate + g_SPremiumInfo[m_entries[slot].m_type].m_expRate;
        m_outPremiumCount = m_outPremiumCount +
            g_SPremiumInfo[m_entries[slot].m_type].m_premiumCount;
        if (g_SPremiumInfo[m_entries[slot].m_type].m_overEquipFlag != 0)
            m_field8bc = 1;

        // ORIG 组循以 slot（非 type）索引 g_SPremiumInfo（+0x120*(slot) 链
        // 每次重算，+0x378/+0x42a/+0x4a3/+0x5d1...）；pair 元素 (t*0x24+i+0xc)
        // *8 链独立。
        for (int i = 0;
             i < g_SPremiumInfo[slot].m_field64; ++i)
        {
            int j = 0;
            while (j < (int)g_vecPremiumOverEquipableInfo
                          .operator[](
                              g_SPremiumInfo[slot]
                                  .m_returnPairs68[i]
                                  .first)
                          .m_count)
            {
                int category =
                    g_vecPremiumOverEquipableInfo
                        .operator[](
                            g_SPremiumInfo[slot].m_returnPairs68[i].first)
                        .m_slots[j];
                m_overEquipableLevel[category] = std::max(
                    m_overEquipableLevel[category],
                    g_SPremiumInfo[slot].m_returnPairs68[i].second);
                j++;
            }
        }

        for (int i = 0;
             i < g_SPremiumInfo[slot].m_field90; ++i)
        {
            int j = 0;
            while (j < (int)g_vecPremiumReturnItemInfo
                          .operator[](
                              g_SPremiumInfo[slot]
                                  .m_returnPairs94[i]
                                  .first)
                          .m_count)
            {
                int itemIndex =
                    g_vecPremiumReturnItemInfo
                        .operator[](
                            g_SPremiumInfo[slot].m_returnPairs94[i].first)
                        .m_items[j];
                std::map<int, int>::iterator it =
                    m_returnItemMap.find(itemIndex);
                if (it == m_returnItemMap.end())
                {
                    std::pair<int, int> np(itemIndex,
                        g_SPremiumInfo[slot].m_returnPairs94[i].second);
                    m_returnItemMap.insert(np);
                }
                else
                {
                    it->second = it->second +
                        g_SPremiumInfo[slot].m_returnPairs94[i].second;
                }
                j++;
            }
        }

        m_additionalBase +=
            g_SPremiumInfo[m_entries[slot].m_type].m_additionalInfo;
    }
}

void CUserPremium::setPremiumState(ENUM_PREMIUM_TYPE type, ENUM_PREMIUM_STATE state)
{
    // [b27 封顶] 残差 = 寻址/缓存形：ORIG this→ecx + lea0x10→edx 两步 disp，
    // 4.4 恒 this→ebx + add 直算（N18a/N7 族寄存器分配差，加 1 条 push %ebx）。
    m_entries[(int)type].m_state = (int)state;
}

bool CUserPremium::_CheckApply(int type, tm t)
{
    // ORIG 0x86ae834：结果落 bool 槽 local_9（命中不早退，继续迭代）；
    // mask 每判定独立全链（无 def 缓存）；+0x18 即 tm_wday。
    bool ok = false;
    if (g_SPremiumInfo[type].m_applyDayMask[0] == -1)
    {
        ok = true;
    }
    else
    {
        for (int i = 0;
             i < 7 &&
             g_SPremiumInfo[type].m_applyDayMask[i] != -1;
             ++i)
        {
            if (g_SPremiumInfo[type].m_applyDayMask[i] == t.tm_wday)
                ok = true;
        }
    }
    return ok;
}

int CUserPremium::SetPremiumItemData(unsigned long key, const STPremiumItemData& data)
{
    if (key == 0)
        return 1;
    // ORIG 0x86af70a：end() 先求值、find 后；未命中走 make_pair(lvalue,lvalue)
    // 引用对 + converting pair + insert（P123：非 operator[] 形）
    std::map<unsigned long, STPremiumItemData>::iterator e =
        g_map_premium_item_data.end();
    if (g_map_premium_item_data.find(key) == e)
    {
        g_map_premium_item_data.insert(std::make_pair(key, data));
        return 1;
    }
    return 0;
}

bool CUserPremium::IsRestrictedPremium(int type)  // static（ORIG 0x086af7c4 无 this）
{
    return (type >= 60000 && type <= 69999);
}

void CUserPremium::RecalcAdditionalInfo(CUser const* user)
{
    if (user->getCurCharacR() == 0 || GetAdvPremiumCount() == 0)
        return;

    // 残差豁免（O=316/N=325，+9：乘除段个别操作数 4.4 双段物化/单段折叠
    // 位差，Main 裁决标注不追）。
    // ORIG（0x086ae8c6）：读侧一律 this+0x8bd（= m_additionalBase），写侧一律
    // this+0x90f（= m_additionalInfo）——asm 实证 `mov 0x8bd(%eax),%eax` …
    // `mov %edx,0x90f(%eax)`，Ghidra C 同形。此前把结果写回 +0x8bd 属偏移错配。

    // 0x00/0x04/0x42 的 ORIG 除法使用无符号乘积（mul+shr）；其余 int/short
    // 分支按汇编的有符号乘除（imul+sar）处理。pack(1) 成员直访保留
    // +0x3a/+0x42 等非自然对齐字段的 ABI 宽度。
    // ORIG：getCurCharacAddInfoR 只调用一次并缓存于 ebp-0x10，各语句按
    // `mov -0x10(%ebp),%eax` + disp 重读（故此处用单局部指针）。
    const _Additioal_info* addInfo =
        static_cast<const _Additioal_info*>(user->getCurCharacAddInfoR());
    // probe p16/m1：4.4 乘法 L2R 求值——ORIG 先 *(addInfo 槽) 后 m_8bd ⇒ 左操作数
    // 必为 addInfo->field（原写 m_additionalBase 在左=先读 m_8bd，反序）。
    m_additionalInfo.m_field00 = static_cast<int>(
        (static_cast<unsigned int>(addInfo->m_field00) *
         static_cast<unsigned int>(m_additionalBase.m_field00)) / 100U);
    m_additionalInfo.m_field04 = static_cast<int>(
        (static_cast<unsigned int>(addInfo->m_field04) *
         static_cast<unsigned int>(m_additionalBase.m_field04)) / 100U);
    m_additionalInfo.m_field08 = static_cast<unsigned short>(
        (static_cast<int>(addInfo->m_field08) *
         static_cast<int>(m_additionalBase.m_field08)) / 100);
    m_additionalInfo.m_field0a = static_cast<unsigned short>(
        (static_cast<int>(addInfo->m_field0a) *
         static_cast<int>(m_additionalBase.m_field0a)) / 100);
    m_additionalInfo.m_field0c = static_cast<unsigned short>(
        (static_cast<int>(addInfo->m_field0c) *
         static_cast<int>(m_additionalBase.m_field0c)) / 100);
    m_additionalInfo.m_field0e = static_cast<unsigned short>(
        (static_cast<int>(addInfo->m_field0e) *
         static_cast<int>(m_additionalBase.m_field0e)) / 100);

    for (int i = 0; i < 4; ++i)
    {
        m_additionalInfo.m_field10[i] = static_cast<unsigned short>(
            (static_cast<short>(addInfo->m_field10[i]) *
             static_cast<short>(m_additionalBase.m_field10[i])) / 100);
    }

    m_additionalInfo.m_field3a = (addInfo->m_field3a * m_additionalBase.m_field3a) / 100;
    m_additionalInfo.m_field3e = static_cast<unsigned short>(
        (static_cast<short>(addInfo->m_field3e) *
         static_cast<short>(m_additionalBase.m_field3e)) / 100);
    m_additionalInfo.m_field40 = static_cast<unsigned short>(
        (static_cast<short>(addInfo->m_field40) *
         static_cast<short>(m_additionalBase.m_field40)) / 100);
    m_additionalInfo.m_field42 = static_cast<int>(
        (static_cast<unsigned int>(addInfo->m_field42) *
         static_cast<unsigned int>(m_additionalBase.m_field42)) / 100U);
    m_additionalInfo.m_field46 = static_cast<unsigned short>(
        (static_cast<int>(addInfo->m_field46) *
         static_cast<int>(m_additionalBase.m_field46)) / 100);
    m_additionalInfo.m_field48 = static_cast<unsigned short>(
        (static_cast<int>(addInfo->m_field48) *
         static_cast<int>(m_additionalBase.m_field48)) / 100);
    m_additionalInfo.m_field4a = static_cast<unsigned short>(
        (static_cast<short>(addInfo->m_field4a) *
         static_cast<short>(m_additionalBase.m_field4a)) / 100);
    m_additionalInfo.m_field4c = static_cast<unsigned short>(
        (static_cast<short>(addInfo->m_field4c) *
         static_cast<short>(m_additionalBase.m_field4c)) / 100);
}
// ORIG W 0x8694a88 GetAdditionalInfo() const：sret ← 隐式拷贝 ctor 的
// 内联块移（对齐检查 + rep movsl + word/byte 尾，0x52 字节，无 clear 调用
// ⇒ ORIG 源即 return 成员表达式，非 局部+memcpy——后者多 out ctor 与 call）。
_Additioal_info CUserPremium::GetAdditionalInfo() const
{
    return m_additionalInfo;
}


SUserPremiumInfo CUserPremium::CheckPremiumTimeout() const
{
    // ORIG 0x86add9c：result 即 sret（NRVO 直写，无栈局部）；entries[i] 每个
    // 判定点独立全链重算（T-C9-1）；checkTerm 直读 g_SPremiumInfo 表
    // （ORIG +0x63 无 helper call，原 static GetPremiumDefinition 属发明符号）。
    SUserPremiumInfo result;
    int curTime = GlobalData::s_systemTime_.getCurSec();
    // [b28c-L7] P23 实参求值序：ORIG 0x086add9c 序为 `movl $&s_systemTime_,(%esp);
    // call getCurSec; movl $0x0,(%ebx)`——getCurSec 先于 result.m_type 清零落栈
    // ⇒ 清零语句必须写在取时之后（result 声明仍在先，占 sret/NRVO 通道）。
    result.m_type = 0;
    for (int i = 1; i <= 0x69; ++i)
    {
        if (m_entries[i].m_type == 0)
            continue;
        if (g_SPremiumInfo[m_entries[i].m_type].m_checkTerm == 0)
            continue;
        if (m_entries[i].m_state == 3)
            continue;
        if (m_entries[i].m_end <= curTime)
        {
            result = m_entries[i];
            break;
        }
    }
    return result;
}

void CUserPremium::GetPremiumInfoList(std::vector<SUserPremiumInfo>& list, int type) const
{
    list.clear();
    const SUserPremiumInfo* entries = m_entries;
    for (int i = 1; i < 0x6a; ++i)
    {
        const SUserPremiumInfo& entry = entries[i];
        if (entry.m_type != 0 && (type == 0x270f || entry.m_type == type))
            list.push_back(entry);
    }
}

SUserPremiumInfo* CUserPremium::GetPremiumInfo(int type) const
{
    // P0：ORIG 无局部槽（sub 0，`add 0x8(%ebp),%eax`）——原码内联右值直返，
    // 禁 entries 具名局部（4.4 恒落 -0x4 槽差帧）。
    // ORIG：仅 type<0x6a 且 type!=0x270f 时按下标偏移；其余（含
    // 9999 和正向越界值）均返回 entries[0]，负值仍按原始负偏移。
    // [b28c-L7] P222 互试（第二形）：ORIG 0x086adf52 `cmpl $0x69; jg FAIL;
    // cmpl $0x270f; jne SUCC; FAIL: mov 0x8(%ebp),%eax; jmp end; SUCC:<calc>`
    // ⇒ 否定短路 || 早返（then=FAIL 落前、SUCC fall-through 落后）；三目形
    // 实测产 `jg X; je X; jmp Y` 镜像布局（+1 jmp）。
    if (type >= 0x6a || type == 0x270f)
        return const_cast<SUserPremiumInfo*>(m_entries);
    return const_cast<SUserPremiumInfo*>(&m_entries[type]);
}

int CUserPremium::GetReturnItemRate(int type) const
{
    // ORIG 0x86ae7a8：先 empty() 预检；find/end 结果落 const_iterator 临时
    // 槽后 operator!= 比较（迭代器不直呼内联比较形）
    // [b28c-L7] P59 反向：ORIG 0x86ae7b9 `call empty; test %al,%al; je +0x21`
    // （空→原地 fall 到 `mov $0,%eax; jmp end`、非空→je 跳 body）⇒ 原码是
    // 正极性 guard `if (empty()) return 0;`；`if(!empty()){}` 形 4.4 恒产
    // `xor $1` 反演 + body 前置。map::empty 为 bool callee（P168a①裸形）。
    if (m_returnItemMap.empty())
        return 0;
    std::map<int, int>::const_iterator it = m_returnItemMap.find(type);
    std::map<int, int>::const_iterator e = m_returnItemMap.end();
    // [b28c-L7] 残差 2 处 = N93（控制流/装载形已全等）：find 结果槽 ORIG
    // -0x10 vs 4.4 -0xc、end 结果槽 ORIG -0xc vs 4.4 -0x10——同块内两个直接
    // 初始化迭代器，4.1 按声明序占低→高槽、4.4 相反（N93 原例即本仓
    // DB_LoadInventory::_GetTagCharacInven 族）。排除形实测：交换声明序 ⇒
    // end() call 前置于 find()（ORIG 调用序反，新增 2 处失配）；提头声明
    // + 块内赋值 ⇒ 触发 _Rb_tree_const_iterator 默认 C1 + sret 临时拷贝
    // （N86，+3 条）。源形不可翻，语义与条数等价 ⇒ 封 N93。
    // [b28c-L7] P222：ORIG 0x86ae81a `call ne; test %al; je <mov$0 尾岛>` +
    // SUCC(ptEv+mov 0x4) fall-through 在前 ⇒ 正极性 `if(it!=e) return …`，
    // 否定早返形把 SUCC 推到岛后（+1 jmp）。
    if (it != e)
        return it->second;
    return 0;
}

bool CUserPremium::CheckPremium(int type) const
{
    // ORIG（W，汇编实证）：直接 this+type*0x14 读 m_type，仅判非 0；
    // 不查 state（state==3 已过期条目仍返回 true），无边界检查。
    // ORIG 0x80e5fe6：test;je;mov$1;jmp;mov$0 分支物化；原 !=0 折 setne（P22/P101）。
    if (m_entries[type].m_type != 0)
        return true;
    return false;
}

// ORIG W 0x086b086c：返回 +0x862 字节（任一 premium 定义需每日检查时为 1）。
bool CUserPremium::IsNeedCheckEveryday()
{
    return m_field862;
}

// ---------------------------------------------------------------------------
// ORIG W 0x085bf940 _ZNK8WongWork12CUserPremium14GetAddExpBonusEi：
// type∈[0,0x69] 时返回 g_SPremiumInfo[type].m_field44（表基 0x09483a84，
// 步长 0x120），否则 0。逐指令转写。
// ---------------------------------------------------------------------------
int CUserPremium::GetAddExpBonus(int type) const
{
    if (type < 0 || type > 0x69)
        return 0;
    return g_SPremiumInfo[type].m_field44;
}

std::vector<std::pair<int, int> > CUserPremium::GetAdvantageItem(int type)
{
    std::vector<std::pair<int, int> > items;
    // ORIG 0x86adf7c：getCurSec+localtime_r 结果不被使用（死调用，保序保留）；
    // entries[type] 每个条件位独立全链（T-C9-1）；表访问以
    // g_SPremiumInfo[type*0x120…] 内联链直读（无 def/entry/helper）；
    // 循环 pair 为默认 ctor + 两字段写 + lvalue push_back（非 make_pair）。
    time_t nowT = (time_t)GlobalData::s_systemTime_.getCurSec();
    tm localTm;
    localtime_r(&nowT, &localTm);
    (void)localTm;
    if (m_entries[type].m_type != 0 && m_entries[type].m_type < 0x6a &&
        g_SPremiumInfo[m_entries[type].m_type].m_kind == 1)
    {
        for (int i = 0;
             i < 5 &&
             g_SPremiumInfo[m_entries[type].m_type]
                     .m_advantageItems[i]
                     .first != 0;
             ++i)
        {
            std::pair<int, int> p;
            p.first = g_SPremiumInfo[m_entries[type].m_type]
                          .m_advantageItems[i]
                          .first;
            p.second = g_SPremiumInfo[m_entries[type].m_type]
                           .m_advantageItems[i]
                           .second;
            items.push_back(p);
        }
    }
    return items;
}

// ---------------------------------------------------------------------------
// 残差豁免（O=758/N=767 级，散布 +9~15：字段拷贝展开策略——4.1 以
// `g_SPremiumInfo+0x106/+0x1c/+0x256…` 别名标记的 block 段与 4.4 逐成员
// operator=/vector 形差异，Main 裁决不追，2026-09-05）。
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
        // ORIG 无任何引用缓存：每个字段读写都重算 table+i*0x130+off 与
        // g_SPremiumInfo+i*0x120+off（Ghidra C 每条赋值两侧都带 local_38*0x130 /
        // local_38*0x120 的完整下标式），且四抗性走 j=0..3 循环而非四条赋值。
        g_SPremiumInfo[i].m_kind = table[i].m_kind;                 // +0x00
        g_SPremiumInfo[i].m_checkTerm = table[i].m_checkTerm;       // +0x04
        g_SPremiumInfo[i].m_timeLimit = table[i].m_expireTime;      // +0x08
        for (int j = 0; j < 5; ++j)                     // +0x0c 特权物品对
        {
            g_SPremiumInfo[i].m_advantageItems[j].first = table[i].m_advantageItems[j].first;
            g_SPremiumInfo[i].m_advantageItems[j].second = table[i].m_advantageItems[j].second;
        }
        g_SPremiumInfo[i].m_fatigueRate = table[i].m_fatigueRate;         // +0x34
        g_SPremiumInfo[i].m_premiumCount = table[i].m_coin;               // +0x38
        g_SPremiumInfo[i].m_expRate = table[i].m_expRate;                 // +0x3c
        g_SPremiumInfo[i].m_needDailyCheck = table[i].m_unlimitFatigue;   // +0x40
        g_SPremiumInfo[i].m_field44 = table[i].m_bonusExp;                // +0x44
        g_SPremiumInfo[i].m_field48 = table[i].m_questItemDropRate;       // +0x48
        g_SPremiumInfo[i].m_independentDropRates = table[i].m_independentDropRates;  // +0x4c
        g_SPremiumInfo[i].m_overSkillLevel = table[i].m_overSkillLevel;   // +0x58 ← t+0x42
        g_SPremiumInfo[i].m_field5a = table[i].m_cerashopDiscountRate;    // +0x5a ← t+0x44
        g_SPremiumInfo[i].m_useTargetServer = table[i].m_useTargetServer; // +0x5c
        g_SPremiumInfo[i].m_overEquipFlag = table[i].m_goldBonus;         // +0x60
        g_SPremiumInfo[i].m_field64 = table[i].m_overEquipCount;          // +0x64
        for (int j = 0; j < 5; ++j)                     // +0x68 返还物品对
        {
            g_SPremiumInfo[i].m_returnPairs68[j].first = table[i].m_overEquipPairs[j].first;
            g_SPremiumInfo[i].m_returnPairs68[j].second = table[i].m_overEquipPairs[j].second;
        }
        g_SPremiumInfo[i].m_field90 = table[i].m_returnItemCount;         // +0x90
        for (int j = 0; j < 3; ++j)                     // +0x94 返还对
        {
            g_SPremiumInfo[i].m_returnPairs94[j].first = table[i].m_returnItemPairs[j].first;
            g_SPremiumInfo[i].m_returnPairs94[j].second = table[i].m_returnItemPairs[j].second;
        }
        for (int j = 0; j < 7; ++j)                     // +0xac yday 掩码
            g_SPremiumInfo[i].m_applyDayMask[j] = table[i].m_applyDayMask[j];

        g_SPremiumInfo[i].m_additionalInfo.m_field00 = table[i].m_hpMax;
        g_SPremiumInfo[i].m_additionalInfo.m_field04 = table[i].m_mpMax;
        g_SPremiumInfo[i].m_additionalInfo.m_field08 = table[i].m_physicalAttack;
        g_SPremiumInfo[i].m_additionalInfo.m_field0a = table[i].m_physicalDefense;
        g_SPremiumInfo[i].m_additionalInfo.m_field0c = table[i].m_magicalAttack;
        g_SPremiumInfo[i].m_additionalInfo.m_field0e = table[i].m_magicalDefense;
        for (int j = 0; j < 4; ++j)                     // +0xd8 四抗性（ORIG 为循环）
            g_SPremiumInfo[i].m_additionalInfo.m_field10[j] =
                (&table[i].m_fireResistance)[j];
        for (int j = 0; j < 17; ++j)
            g_SPremiumInfo[i].m_additionalInfo.m_field18[j] = table[i].m_rawE0[j];
        g_SPremiumInfo[i].m_additionalInfo.m_field3a = table[i].m_inventoryLimit;
        g_SPremiumInfo[i].m_additionalInfo.m_field3e = table[i].m_hpRegenSpeed;
        g_SPremiumInfo[i].m_additionalInfo.m_field40 = table[i].m_mpRegenSpeed;
        g_SPremiumInfo[i].m_additionalInfo.m_field42 = (unsigned int)table[i].m_moveSpeed;
        g_SPremiumInfo[i].m_additionalInfo.m_field46 = table[i].m_attackSpeed;
        g_SPremiumInfo[i].m_additionalInfo.m_field48 = table[i].m_castSpeed;
        g_SPremiumInfo[i].m_additionalInfo.m_field4a = table[i].m_hitRecovery;
        g_SPremiumInfo[i].m_additionalInfo.m_field4c = table[i].m_jumpPower;
        g_SPremiumInfo[i].m_additionalInfo.m_field4e = table[i].m_field118;

        g_SPremiumInfo[i].m_name = table[i].m_name;                    // +0x11c

        STPremiumItemData data;
        data.m_value0 = i;
        unsigned long key = (unsigned long)table[i].m_targetItemKey;
        if (table[i].m_premiumItems.size() == 0)
        {
            // 空表：flag 字节取已拷贝 g 表 m_checkTerm 低字节，key 用记录键。
            data.m_c4 = (unsigned char)g_SPremiumInfo[i].m_checkTerm;
            if (SetPremiumItemData(key, data) != 1)
                return 0x1c4;
        }
        else
        {
            std::vector<premium_script_parsing::STPremiumItemData>::const_iterator it;
            for (it = table[i].m_premiumItems.begin();
                 it != table[i].m_premiumItems.end(); ++it)
            {
                data.m_c4 = it->m_termFlag;
                data.m_c5 = (unsigned char)(it->m_restrictFlag != 0);
                if (SetPremiumItemData((unsigned long)it->m_key, data) != 1)
                    return 0x1bc;
            }
        }
    }
    return 0;
}

} // namespace WongWork

// ---- _Additioal_info 字段算术（ORIG df_game_r 逐指令转写；全局作用域类）----
// operator+=（ORIG W _ZN15_Additioal_infopLERKS_ @0x08693cea）：仅转发 add，
//   无引用返回（ORIG 函数尾未装载 this 到 eax）。
//   P210 定义前置：置于 add 定义之前，4.4 对未见面定义的 callee 用保守
//   outgoing 帧（复现 ORIG 4.1 帧 0x18 vs 继承 add 小帧的 0x8）。
void _Additioal_info::operator+=(const _Additioal_info& other)
{
    add(other);
}

// add（ORIG W _ZN15_Additioal_info3addERKS_ @0x08146300）：int@+0x00/+0x04 32 位加；
//   u16@+0x08..+0x0e 显式加；循环 i=0..3 加 u16[i] @ (i+8)*2（即 m_field10[4]）；
//   循环 j=0..0x10 加 u16[j] @ 0x18+2j（即 m_field18[17]）；
//   收尾依次 +0x3a(int)、+0x3e、+0x40、+0x42(int)、+0x46、+0x48、+0x4a、+0x4c、+0x4e(int)。
//   u16 均为零扩展后 mod 2^16 运算（movzwl/add/mov），unsigned short 成员天然良构。
// [b21 N-86b] 语义已闭合：ORIG(4.1) 对 16 位和用 `lea (%edx,%eax,1)`+`mov %eax,%edx`
//   寄存器舞（0x3e/0x40/0x4a/0x4c 四 union 字段），4.4 恒发 `add %eax,%edx` 直形——
//   同算术收形源码不可择（N-86b probe 实证恒内存直形）；循环体寄存器分配同理。
//   逐字段语义逐一相等，封 N。
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
// [b21 N-86b] 语义已闭合：与 add 同族——ORIG(4.1) 16 位差经
//   `mov %eax,%edx`+`lea/sub` 寄存器中途（含 mov %ebx,%eax 回转），4.4 恒发直接
//   `sub %ax,%cx`/`add %eax,%edx` 形；0x4c/0x4e 加法特例已忠实保留。逐字段语义
//   逐一相等，封 N。
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

// ORIG W 0x0830ed48 _ZN8WongWork12CUserPremium25GetAddIndependentDropRateEij
// 二参（unsigned，1..4）选 m_independentDropRates 列；一参（int，0..0x69）选
// g_SPremiumInfo 行；越界返回 0。
unsigned int WongWork::CUserPremium::GetAddIndependentDropRate(int level, unsigned int type)
{
    // ORIG 0x830ed48：平铺早退形（je/js/jle bad 直跳共享 mov$0 出口）；
    // type-1 落 uchar 槽 -0x9（mov %al,-0x9;movzbl -0x9 回读，P0 具名局部）
    // ORIG je 0x5a bad 块（4.4 对 ==0 恒 jne 反形，goto 保留 je 极性）
    // idx 前置声明（C++98 goto 跨初始化禁令；赋值仍 mov %al,-0x9 单步）
    unsigned char idx;
    if (type == 0)
        goto L0;
    if (type > 4)
        return 0;
    if (level < 0)
        return 0;
    if (level > 0x69)
        return 0;
    // [b27 封顶] 残差 = js/jns 极性（ORIG level<0 js 直落 mov$0 岛；4.4 对
    // if(<0) 恒 jns 前跳反形，goto/共享块/两 return 序均试不翻，N93附 同族
    // 跨版本 lowering）+ vector[] 基址 add 立即数 reloc。语义/槽/守卫序全等。
    idx = static_cast<unsigned char>(type - 1);
    return g_SPremiumInfo[level].m_independentDropRates[idx];
L0:
    return 0;
}