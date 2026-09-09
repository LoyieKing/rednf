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

// [b28d-solo-CUserPremium] P254：ReCalcAdvantage 0x086ae5xx 读 m_slots[j]
// 为两段 `mov 0x4(%eax,%edx,4),%eax; mov %eax,%ebx`（4.1 对 enum 字段保留
// eax→目标中转）；int 字段 4.4 恒折单段 ⇒ m_slots 声明为 enum（4 字节，
// 值域 0..0x15，布局/sizeof 不变）。
enum ENUM_OVER_EQUIP_SLOT
{
    OVER_EQUIP_SLOT_HAT_AVATAR = 0x00,
    OVER_EQUIP_SLOT_MAGIC_STONE = 0x15,
    OVER_EQUIP_SLOT_MAX = 0x16
};

struct SPremiumOverEquipableInfo
{
    int m_count;        // +0x00 组内标签数（ScanInt 失败前已写入）
    ENUM_OVER_EQUIP_SLOT m_slots[26];  // +0x04 标签分类值（[hat avatar]=0 .. [magic stone]=0x15）
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

// [b28d-solo-CUserPremium] 修法（P167/N-XVAL）：ORIG 0x08905d95 调
// `vector<unsigned int>::push_back(unsigned int&&)`（EOj）；左值实参使 4.4 选
// const& 重载（RKj），改 static_cast<unsigned int&&> 后差集清零。
// 终判：P167 CLEAR（145/145）+ gap 17/1142（1.5%）⇒ 语义完成；byte 层为
// 帧-槽族（帧 0x100 vs 0xf0、局部槽 -0x74/-0x78 vs -0x60/-0x64 全帧错位、
// ORIG-only movl×13 全为槽偏移对齐失败非缺写）⇒ N82/N95/P212 封顶。
int importPremiumList(const char* path,
                      stPremiumTable_t* table,
                      std::vector<SPremiumOverEquipableInfo>& overVec,
                      std::vector<SPremiumReturnItemInfo>& returnVec)
{
    if (!loadRDARScriptFile("", path))
        return 0x14a6;

    std::string token;
    std::string value;
    // [b28d-solo-CUserPremium] P171/P232：ORIG +0x45 `movl $0x0,-0x6c(%ebp)`
    // 全函数唯一一次写、无读（死局部占帧）⇒ 照抄。
    int deadLocal = 0;
    bool ok = false;
    // ORIG -0x60 槽：跨组保留的标签分类值（未知标签时写入残留值）；ORIG 头段
    // 无 `movl $0,-0x60` ⇒ 原码声明处不带初始化。
    int equipCase;

    for (int i = 0; i <= 0x69; ++i)
        table[i].clean();

    // [b28d-solo-CUserPremium] P0：ORIG +0x7c 才写 cur（clean 循环之后）⇒
    // 原码在 clean 之后声明。
    stPremiumTable_t* cur = table;

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
                    elem.m_slots[j] = (ENUM_OVER_EQUIP_SLOT)equipCase;
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
            // [b28d-solo-CUserPremium] P0/P211：ORIG +0x842 rate 局部为 **int**
            // （for-init `movl $0x0,-0x40` 在头、for-incr `movl $0x0,-0x40` 在尾
            // +0x883）；push_back 处 `mov -0x40,%eax; mov %eax,-0x70` 转换临时
            // ⇒ 原码 `push_back(rate)` 由 int→unsigned int 产生右值临时，选中
            // `push_back(unsigned int&&)`（EOj）。旧 `unsigned int rate` +
            // static_cast<unsigned int&&> 无 -0x70 临时且尾无 re-init。
            for (int rate = 0; ; rate = 0)
            {
                rate = ScanInt(&ok);
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
            // [b28d-solo-CUserPremium] P0：ORIG 0x08906f1a 该分支自带
            // `movl $0x0,-0x14` + `movb $0x0,-0x7a` 两槽（int v 与分支局部
            // bool），与 [item] 分支的 -0x7b bool 各占独立槽——原码是两个
            // 分支各自的局部，非函数头单例 itemOk。
            int v = 0;
            bool itemOk = false;
            v = ScanInt(&itemOk);
            if (!itemOk)
                return 0;  // ORIG 怪癖（0x8906f2c）：失败按成功返回 0
            cur->m_targetItemKey = (unsigned int)v;
            continue;
        }

        if (token == "[item]")
        {
            // [b28d-solo-CUserPremium] P0：ORIG 0x08906f0a `movb $0x0,-0x7b`
            // 先于 item 的 C1（0x08906f0f），且循环内 [term]/[restrict] 复用
            // 同一槽 ⇒ 分支局部 bool 声明在 item 之前。
            bool itemOk = false;
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
// importPremiumList 终判（b28d-solo-CUserPremium）：P167 CLEAR（145/145）
// + 上列 gap ⇒ 语义完成；残余全为帧-槽族 N82/N95/P212（见函数头判词）。
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

unsigned short CUserPremium::GetAdvantageFatigueRate() const
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

// [b28d-solo-CUserPremium] P309 + 早返尾形（b28d 复测）：ORIG 0x086adba6 两处
// std::max 临时槽对为四个独立槽（if 臂 dec/zero=-0x18/-0x14、else 臂
// dec/zero=-0x10/-0xc）⇒ 四标量提函数头且声明序 zero2(-0xc)/dec2(-0x10)/
// zero1(-0x14)/dec1(-0x18)，4.4「头部声明先声明浅」精确复现四槽。
// 尾形：ORIG 两臂各自 `jmp <leave>`（0x86adc2f/0x86adc64）+ je 落 nop 岛
// （0x86adc66）⇒ 源形必须是「if 臂 return + else 臂落体外 return」两处裸早返
// （探针 /tmp/b28d_solo/p3.o f4：`if(kind==1){…;return;} …;return;` 产
// `jne 臂2; 臂1; jmp exit; 臂2; jmp exit; nop; exit` 与 ORIG 逐条同形）；
// if/else 形 4.4 令末臂直落（缺第二 jmp）⇒ 已由本形取代。
void CUserPremium::RemovePremium(int type)
{
    // ORIG 0x86adba6：计数递减走 std::max<int>(zero, dec) 调用形（两临时
    // 槽分支各独立，非三目）；链全重算。
    // P309：声明序 = ORIG 槽浅→深（勿改序，否则槽位回退）。
    int zero2;
    int dec2;
    int zero1;
    int dec1;
    if (m_entries[type].m_state != 2)
    {
        m_entries[type].m_type = 0;
        if (g_SPremiumInfo[type].m_kind == 1)
        {
            dec1 = m_normalCount + -1;
            zero1 = 0;
            m_normalCount = std::max(zero1, dec1);
            return;
        }
        dec2 = m_restrictedCount + -1;
        zero2 = 0;
        m_restrictedCount = std::max(zero2, dec2);
        return;
    }
    return;
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

// [b28d-solo-CUserPremium] 修法（P167/P0/P211/P214①/P254）：
// ①插入路径改 make_pair<int&,int&> + pair<const int,int> 右值转换构造、
//   else 用 `it->second +=`（ORIG 单次 ptEv）⇒ 调用点差集清零（23/23 CLEAR）；
// ②overEquip body 的 lhs 索引 + max 第一实参各重算 vec[]（ORIG 3 调用点）；
// ③去 int now 中转（nowT 单槽）；④+0x85e/+0x850 由 Main 代落 CUser.h 两字段
//   short→unsigned short（ORIG 0x0812cdb4/0x0860939e 两处 movzwl 证据；探针
//   /tmp/p_u16b.cpp f6 vs f7/f8：目标 short ⇒ `movzwl→%eax; mov→%edx` 两段，
//   目标 unsigned short ⇒ `movzwl→%edx` 单段）；⑤m_slots enum 化复现两段
//   中转（P254）。
// 终判（b28d 复测）：IDENTICAL 482/482（ndiff 0，gap 0，槽位集全等），
// P167 CLEAR（23/23）——旧判词「余 3 处 N18a/N-86b 封顶」已由④推翻。
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

    // [b28d-solo-CUserPremium] P0：ORIG 只有 nowT 一槽（`mov %eax,-0x3c(%ebp)`
    // 后直接 lea 传址）；具名 int now 中转多 1 槽 + 1 条 mov（ndiff -3）。
    time_t nowT = (time_t)GlobalData::s_systemTime_.getCurSec();
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
        // [b28d-solo-CUserPremium] P0：ORIG 对 +0x85e 直接取址（`add $0x85e,%eax`）
        // 传引用；`static_cast<unsigned short>(字段)` 产生临时槽（movzwl+movw+lea）。
        m_overSkillLevel =
            std::max(reinterpret_cast<unsigned short&>(m_overSkillLevel),
                     g_SPremiumInfo[m_entries[slot].m_type].m_overSkillLevel);
        m_ceraDiscountRate = std::max(
            m_ceraDiscountRate,
            g_SPremiumInfo[m_entries[slot].m_type].m_field5a);

        m_outFatigueRate =
            (unsigned short)m_outFatigueRate +
            g_SPremiumInfo[m_entries[slot].m_type].m_fatigueRate;
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
                // [b28d-solo-CUserPremium] P167：ORIG 该 body 有**两个**
                // vector<SPremiumOverEquipableInfo>::operator[] 调用点
                // （lhs 索引 + max 第一实参各重算一次，无 category 具名局部；
                // ORIG 共 3 处调用 vs 旧形 2 处 ⇒ MISSING×1）。
                m_overEquipableLevel[
                    g_vecPremiumOverEquipableInfo
                        .operator[](
                            g_SPremiumInfo[slot].m_returnPairs68[i].first)
                        .m_slots[j]] =
                    std::max(
                        m_overEquipableLevel[
                            g_vecPremiumOverEquipableInfo
                                .operator[](
                                    g_SPremiumInfo[slot].m_returnPairs68[i].first)
                                .m_slots[j]],
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
                    // [b28d-solo-CUserPremium] P167：ORIG 调 make_pair<int&,int&>
                    // + pair<const int,int>::C1<int,int>(pair<int,int>&&)（右值
                    // 转换构造）；旧具名 pair 左值走拷贝版（EXTRA 两符号）。
                    m_returnItemMap.insert(std::make_pair(itemIndex,
                        g_SPremiumInfo[slot].m_returnPairs94[i].second));
                }
                else
                {
                    // [b28d-solo-CUserPremium] P167：ORIG 只发 1 次 ptEv
                    // （`it->second += x` 复合赋值形）；写成 `it->second =
                    // it->second + x` 4.4 会发 2 次 ptEv。
                    it->second +=
                        g_SPremiumInfo[slot].m_returnPairs94[i].second;
                }
                j++;
            }
        }

        m_additionalBase +=
            g_SPremiumInfo[m_entries[slot].m_type].m_additionalInfo;
    }
}

// [b28d-solo-CUserPremium] N107/N7 封顶（b28d 复测；旧注释地址 0x0830ed48
// 系误录，权威地址 nm：0x086b084c）。ORIG 14 条：前两条 `mov 0xc(%ebp),%edx;
// mov 0x8(%ebp),%ecx`（type 先载/this 后载），尾段 `lea (%ecx,%eax,1),%eax;
// lea 0x10(%eax),%edx; mov 0x10(%ebp),%eax; mov %eax,(%edx)`。
// 指针算术形（下方实现）13 条：4.4 折 `lea (%ecx,%eax,1),%edx` + disp store
// `mov %eax,0x10(%edx)`，且 this 先载。
// 本波穷举读数：①`m_entries[(int)type]` 下标形 16 条（push %ebx）；
// ②`*(&m_entries[(int)type].m_state)` 取址形 16 条（push %ebx + add $0x10）；
// ③`((int*)(m_entries+(int)type))[4]` 整型数组视图 15 条（add $0x10 + this
// 中转）——均劣于指针算术形。两段 lea（N107：4.4 对成员常量偏移恒折
// disp/add）+ 形参装载序（4.4 this-first vs 4.1 type-first）双封顶；
// P167 CLEAR、gap 1/14 ⇒ 语义等价。
void CUserPremium::setPremiumState(ENUM_PREMIUM_TYPE type, ENUM_PREMIUM_STATE state)
{
    (m_entries + (int)type)->m_state = (int)state;
}

// [b28d-solo-CUserPremium] P225 修法：ORIG 0x086ae834 的 mask[0]==-1 分支是
// `mov $1,%eax; jmp end`（return true 直返，非 ok=true 落槽）；循环为
// while+break 形（体首 mask==-1 je 出口、尾测 `cmpl $6; setle; test; jne`），
// 旧 for&& 形使 4.4 折成 `jg` 直跳（缺 setle 物化）。
bool CUserPremium::_CheckApply(int type, tm t)
{
    // ORIG 0x86ae834：结果落 bool 槽 local_9（命中不早退，继续迭代）；
    // mask 每判定独立全链（无 def 缓存）；+0x18 即 tm_wday。
    bool ok = false;
    if (g_SPremiumInfo[type].m_applyDayMask[0] == -1)
        return true;
    int i = 0;
    while (i < 7)
    {
        if (g_SPremiumInfo[type].m_applyDayMask[i] == -1)
            break;
        if (g_SPremiumInfo[type].m_applyDayMask[i] == t.tm_wday)
            ok = true;
        ++i;
    }
    return ok;
}

// [b28d-solo-CUserPremium] IDENTICAL/AE（routed 口径 check_tu_routed 复验）：
// ORIG 0x086af70a 逐条对齐（52/52，ndiff=0）。P167 唯一差 = make_pair 模板
// 实参 IRKm vs IRm（形参 top-level const 差异，不入 mangled/布局）。
bool CUserPremium::SetPremiumItemData(unsigned long key, const STPremiumItemData& data)
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

// [b28d-solo-CUserPremium] 修法（P235/P211）：ORIG 0x086ae8c6 读侧单段
// `mov (%eax),%edx; shr $0x5,%edx`；改 unsigned int 同型视图
// （m_maxHP/m_maxMP/m_moveSpeed）直读后 315=315 ndiff=0（旧 int 视图 +
// static_cast 使 4.4 多发 `mov %eax,%edx`，3 处 ×7 条）。P167 CLEAR（3/3）。
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
    // [b28d-solo-CUserPremium] P235/P211：ORIG 读侧为单段 `mov (%eax),%edx`
    // （无 eax→edx 中转）⇒ 源用 unsigned int 同型视图 m_maxHP/m_maxMP/
    // m_moveSpeed 直读；`static_cast<unsigned int>(int 视图)` 使 4.4 多发
    // `mov %eax,%edx` 两段（3 处 ×7 条 = ndiff 21）。
    m_additionalInfo.m_maxHP =
        addInfo->m_maxHP * m_additionalBase.m_maxHP / 100U;
    m_additionalInfo.m_maxMP =
        addInfo->m_maxMP * m_additionalBase.m_maxMP / 100U;
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
    m_additionalInfo.m_moveSpeed =
        addInfo->m_moveSpeed * m_additionalBase.m_moveSpeed / 100U;
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

// [b28d-solo-CUserPremium] N93 封顶（b28d 复测）：ORIG 0x086ae7a8；find 结果槽
// -0x10 / end -0xc 与 4.4 相反（同块两直接初始化迭代器，4.1 声明序低→高、
// 4.4 相反）。交换声明序 ⇒ 调用序反；提头声明 ⇒ 默认 C1+sret 拷贝（N86）；
// P293 单表达式形 ⇒ end 先于 find（ORIG 调用序反）。P167 CLEAR（5/5）、
// 42=42 gap0 ⇒ 语义等价，封 N93。
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

// [b28d-solo-CUserPremium] 修法（P225）：ORIG 0x086adf7c 循环为「尾测 setle
// 物化 + 体首 first==0 出口」形；for&& 合并条件使 4.4 折 jg/bool 物化，
// 改 while+break 后 132=132 ndiff=0。P167 CLEAR（7/7）。
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
        // [b28d-solo-CUserPremium] P225：ORIG 循环为「尾测 setle 物化 + 体首
        // first==0 出口」形；for&& 合并条件使 4.4 折 jg/bool 物化（ndiff 38）。
        int i = 0;
        while (i < 5)
        {
            if (g_SPremiumInfo[m_entries[type].m_type]
                    .m_advantageItems[i]
                    .first == 0)
                break;
            std::pair<int, int> p;
            p.first = g_SPremiumInfo[m_entries[type].m_type]
                          .m_advantageItems[i]
                          .first;
            p.second = g_SPremiumInfo[m_entries[type].m_type]
                           .m_advantageItems[i]
                           .second;
            items.push_back(p);
            ++i;
        }
    }
    return items;
}

// ---------------------------------------------------------------------------
// 终判（b28d-solo-CUserPremium，b28d 复测）：P167 CLEAR（20/20）；两处修法：
// ①删冗余 `unsigned long key` 具名局部（ORIG 该键内联为实参、无槽往返，P0）；
// ②拷贝段赋值顺序按 ORIG g 表 store 序列重排（0x00/0x04/0x08/0x0c/0x34/0x38/
// 0x3c/0x58/0x5a/0x5c/0x40/0x44/0x48/0x60/0x64，见下方注释）⇒ ndiff 725→261、
// 761 条 vs 758（gap -3）。残余：帧 0x7e3c vs 0x7e4c、数组基址 -0x7e28 vs
// -0x7e2c（N95 槽基差）＋附加信息段 ORIG `sub $0x7d50 + disp 0x10..` vs 4.4
// `sub $0x7d40 + disp 0`（N1 常量折叠基准分配）⇒ N82/N95/P232 封顶，语义等价。
// ORIG T 0x086aeca4 WongWork::CUserPremium::LoadPremiumInfo(char const*)。
// importPremiumList 失败→0x15c；逐记录拷贝进 g_SPremiumInfo[i]（0x130→0x120
// 字段重排，+0xc8 起写穿 _Additioal_info 子对象区）；SetPremiumItemData
// 失败：非空 vector 分支→0x1bc、空分支（key=m_targetItemKey，flag 取
// m_checkTerm 低字节）→0x1c4。全部成功返回 0（CDataManager::init 以
// "LoadPremiumInfo Fail(Line: %d)" 打印该行码）。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserPremium] 修法（P167/P96/P50）：ORIG 0x086aeca4 调非 const
// 迭代器族（ptEv/ppEv/ne(N,N)）且无 `iterator::C1Ev`（P167 EXTRA×1）⇒ 非
// const iterator 且 for 初始化处直构；改后差集清零（20/20 CLEAR）。
// 终判（b28d）：同上——删 key 局部后 gap -3/758；残余帧差与槽基差（N82/N95/
// P232）封顶。
int CUserPremium::LoadPremiumInfo(const char* path)
{
    stPremiumTable_t table[0x6a];
    // [b28d-solo-CUserPremium] P212：ORIG 0x086aed54 起外层循环变量 i 占 -0x34
    // （4.4 若留在 for-init 会被同块 key 顶到 -0x38）⇒ 提函数头声明、循环内仅赋值。
    int i;

    if (importPremiumList(path,
                          table,
                          g_vecPremiumOverEquipableInfo,
                          g_vecPremiumReturnItemInfo) != 0)
        return 0x15c;

    for (i = 0; i <= 0x69; ++i)
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
        // [b28d-solo-CUserPremium] 顺序照 ORIG g 表 store 序列（0x00/0x04/0x08/
        // 0x0c/0x34/0x38/0x3c/0x58/0x5a/0x5c/0x40/0x44/0x48/0x60/0x64）。
        g_SPremiumInfo[i].m_overSkillLevel = table[i].m_overSkillLevel;   // +0x58 ← t+0x42
        g_SPremiumInfo[i].m_field5a = table[i].m_cerashopDiscountRate;    // +0x5a ← t+0x44
        g_SPremiumInfo[i].m_useTargetServer = table[i].m_useTargetServer; // +0x5c
        g_SPremiumInfo[i].m_needDailyCheck = table[i].m_unlimitFatigue;   // +0x40
        g_SPremiumInfo[i].m_field44 = table[i].m_bonusExp;                // +0x44
        g_SPremiumInfo[i].m_field48 = table[i].m_questItemDropRate;       // +0x48
        g_SPremiumInfo[i].m_independentDropRates = table[i].m_independentDropRates;  // +0x4c
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
        // [b28d-solo-CUserPremium] P0 取舍（b28d 实测）：ORIG 0x86af54c
        // `mov %eax,-0x38(%ebp)` + 空分支 0x86af667 回读，确为键具名局部；
        // 但 4.4 一旦为该键分配栈槽即触发帧 0x7e3c→0x7e4c、table 基址
        // -0x7e28→-0x7e2c 全帧平移（探针 /tmp/b28d_solo：带槽 ndiff 466、
        // 内联实参 56、纯 register 52；帧/槽对齐后整函数 ~700 条回正）。
        // 取内联实参形（P264/P112 帧族：语义与 ORIG 完全一致，仅少 store/load 对）。
        // [b28d-solo-CUserPremium] P22/P197④：ORIG 0x86af571 `test %eax,%eax;
        // setne %al; test %al,%al; je <空分支 0x86af651>` ⇒ 原码是 `size() != 0`
        // 正向判（非空分支直落、空分支置后），非 `==0` 反极性（后者产 sete）。
        if (table[i].m_premiumItems.size() != 0)
        {
            // [b28d-solo-CUserPremium] P13/P50：ORIG 0x86af5a2/0x86af5ce 的
            // begin/end 各只调一次（it=-0x44、end=-0x48 缓存），循环条件复用
            // 槽；原码在 for 初始化处同时直构 it/itEnd（无 iterator::C1Ev）。
            for (std::vector<premium_script_parsing::STPremiumItemData>::iterator
                     it = table[i].m_premiumItems.begin(),
                     itEnd = table[i].m_premiumItems.end();
                 it != itEnd; ++it)
            {
                data.m_c4 = it->m_termFlag;
                data.m_c5 = (unsigned char)(it->m_restrictFlag != 0);
                // [b28d-solo-CUserPremium] P292/P321-T3：ORIG 0x086af61d
                // `call; xor $0x1,%eax; test %al,%al; je` ⇒ bool 返回 + 裸 `!`
                // （`!=1` 形在 bool 下仍产 cmp/setne）。
                if (!SetPremiumItemData((unsigned long)it->m_key, data))
                    return 0x1bc;
            }
        }
        else
        {
            // 空表：flag 字节取已拷贝 g 表 m_checkTerm 低字节，key 用记录键。
            data.m_c4 = (unsigned char)g_SPremiumInfo[i].m_checkTerm;
            if (!SetPremiumItemData((unsigned long)table[i].m_targetItemKey, data))
                return 0x1c4;
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
// [b28d-solo-CUserPremium] N-86b/N7 封顶（b28d 复测）：ORIG 0x08146300 对
// 16 位加发 `movzwl; mov %eax,%edx; movzwl; lea (%edx,%eax,1); mov %eax,%edx`
// 寄存器舞 + %edx 索引中转；4.4 恒折 `movzwl/movzwl/add/mov %dx` 直形且用
// %ecx 索引（探针 /tmp/probe_add.cpp 六形 + probe_add2.cpp 五形全败）。
// P167 CLEAR（0/0）、逐字段语义相等 ⇒ 封顶。
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
// [b28d-solo-CUserPremium] N-86b/N7 封顶（b28d 复测）：同 add——ORIG 16 位差
// 经 lea/mov 寄存器舞 + %edx 索引中转，4.4 恒直接 sub/add + %ecx；0x4c/0x4e
// 两字段加法特例已忠实保留。P167 CLEAR（0/0）⇒ 语义等价，封顶。
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
// [b28d-solo-CUserPremium] P133/P258 修法：ORIG 0x0830ed48 的两组越界检查是
// **短路或**形（`cmp 0; je bad; cmp 4; jbe ok` 与 `cmp 0; js bad; cmp 0x69;
// jle ok`，两失败块各自独立 mov$0）；旧独立早返形使 4.4 对 level<0 产 jns 反跳。
unsigned int WongWork::CUserPremium::GetAddIndependentDropRate(int level, unsigned int type)
{
    // ORIG 0x830ed48：两组短路或守卫各自独立物化 mov$0 失败块；
    // type-1 落 uchar 槽 -0x9（mov %al,-0x9;movzbl -0x9 回读，P0 具名局部）。
    if (type == 0 || type > 4)
        return 0;
    if (level < 0 || level > 0x69)
        return 0;
    unsigned char idx = static_cast<unsigned char>(type - 1);
    return g_SPremiumInfo[level].m_independentDropRates[idx];
}