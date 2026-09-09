#include "GameRand.h"
// df_game_r CEquipItem 还原（I1 批次，2026-08-16）。
// 函数形态以 docs/class_func_reports/CEquipItem.md + ORIG weak 反汇编为准
// （AE 口径：调用目标归一；字段偏移/常量逐条一致）。
// STEquipmentScript 内联完整布局为 I1 工作布局（脚本表批次细化），
// 依赖的辅助类（CodePage/CSystemTime/CMTRand 等）由 GameStubs.cpp 提供桩。

#include "CEquipItem.h"
#include "GamePODTypes.h"

#include <set>
#include <math.h>
#include <string.h>

#include "CSystemTime.h"
#include "GameTypes.h"

// ===================== 外部依赖（其它 TU 提供） =====================

int get_rand_int(int range);

#include "CEnvironment.h"  // m_serverEnvir.m_gcNo（+0x198）
#include "GlobalData.h"    // s_systemTime_（+0x941f714；make_item getCurSec 调用）
extern CEnvironment* G_CEnvironment();

class CDataManager
{
public:
    void set_AuctionRegFreeCeraItem(unsigned int idx);
};
extern CDataManager* G_CDataManager();

// ORIG 0x898c79d：线性插值
// [b28d-solo-CEquipItem] 逐条还原（ORIG 0x0898c79d；帧 0x68）：
//   ① vSpan/vLevel/vDiff 三个 int 槽按 ORIG 顺序先算（vDiff=d-c 在 x87 段之前，槽 -0x24）；
//   ② ratio 的钳位是 `ratio <= 0.0`（ORIG `fucompp;fnstsw;test $0x45,%ah;sete;test;je`），
//      不是旧写的 `ratio == 0.0`（后者 4.4 产 fucomip+jne/jp 双跳）；
//   ③ `double diff = (double)vDiff;` 具名局部复现 ORIG `fildl -0x24;fstpl -0x50`。
//   x87 比较 lowering 需 -march=i486 才出 ORIG 的 fnstsw/sahf 形（P230 补；已上报 Main 路由）。
int getLevelLinearAbility(int a, int b, int c, int d, int level)
{
    double diff;                                    // ORIG：声明在函数头（深槽 -0x50）
    int vSpan = b - a;                              // ORIG：先求除数（-0x2c）
    int vLevel = level - a;                         // ORIG：再求被除数（-0x28）
    int vDiff = d - c;                              // ORIG：-0x24（在 x87 段之前）
    double ratio = (double)vLevel / (double)vSpan;
    // ORIG `fucompp;fnstsw;test $0x45,%ah;sete;test;je` = !(x>0) 钳回 0；
    // 掩码 0x45（C0|C2|C3 全 0 = 严格大于）对应源码 `x < 0.0`；`x <= 0` 产 `test $0x5`。
    if (ratio < 0.0)
        ratio = 0.0;
    if (ratio > 1.0)
        ratio = 1.0;
    diff = (double)vDiff;                           // ORIG：-0x50
    // ORIG 无具名 result 局部：floor 结果留 st0 直接续算（`fldl const;fdivrp;fmull -0x50`）。
    double result2 = floor(ratio * 1000000.0 + 0.5L) / 1000000.0 * diff;
    int out = (int)((double)c + result2);
    if (out < 0)
        out = 0;
    return out;
}

#include "ItemGloballyUniqueIdentifierGenerator.h"  // itemGloballyUniqueIdentifier_t 权威定义
#include "../../ServerCommon/DNFFunctionLib.h"

namespace user_creature
{
class CCreatureScriptMgr
{
public:
    void RegisterCreatureIdAndItemId(int creatureId, int itemId);
};
CCreatureScriptMgr* GetInstanceCreatureScriptMgr();
}

namespace ExtreamDungeon
{
class CCompound_ExtreamDun_Item
{
public:
    void ForceResultItemRule_Setting(int idx, bool flag, int value);
};
}

// STEquipmentScript 权威布局见 STEquipmentScript.h（class STEquipmentScript : public STItemScript，
// [b28d-D43] N82/N89 大函数（669/705）（ORIG 0x0850fd84）。
// 基类 0x178 + 派生。CEquipItem::set_item(STEquipmentScript&) 直接使用权威成员名访问，
// 不再保留本文件的 flat 工作布局副本（消除与权威头的重复定义）。
// ===================== 报告函数（CEquipItem.md） =====================

// ORIG WpBonusPointData/ExpBonusPointData ctor/clear 为独立符号（0x08513a6e/0x08513a82 等），
// 由 CEquipItem::C1/STEquipmentScript::C1 调用；故在此 out-of-line 定义以发射符号。
WpBonusPointData::WpBonusPointData() { clear(); }
void WpBonusPointData::clear() { m_type = 2; m_value = 0; }
ExpBonusPointData::ExpBonusPointData() { clear(); }
void ExpBonusPointData::clear() { m_type = 2; m_value = 0; }

// ORIG 0x851547e：+0x1e0 WpBonusPointData 与 +0x1e8 ExpBonusPointData 各自
// clear()（m_type=2、m_value=0）。本文件以 4 个 int 平铺该 16 字节区
// （m_field1e0..m_field1ec），初始化列表按声明顺序（0x1f0 vector 之前）补齐。
CEquipItem::CEquipItem()
    : m_wpBonus1e0(),   // WpBonusPointData::C1（type=2, value=0）
      m_expBonus1e8()    // ExpBonusPointData::C1（type=2, value=0）
{
}

CEquipItem::~CEquipItem() {}

unsigned char CEquipItem::IsAvatarColorVariation(int idx) const
{
    if (idx < 0 || idx > 1)
    {
        return 0;
    }
    return m_avatarColor[idx];
}

bool CEquipItem::IsAvatarOptionValid(char idx) const
{
    if (idx < 0)
    {
        return 0;
    }
    if ((signed char)m_avatarAbility.size() <= idx)
    {
        return 0;
    }
    return 1;
}

// [b28d-solo-CEquipItem] ORIG 0x0851388e（34 条，N 封顶）：值全程寄存器直传（无槽往返）
// ⇒ P0 判原码为内联表达式右值；4.1 对 `&&` 两侧同一 this->m_itemType 的两次读复用为单条
// `mov 0x234(%eax),%eax`（cmp 与 shl 共用 %eax）。4.4 -O0 无该复用：内联形恒发第二次读
// （36 条），register 局部形（P231 合法）改出 `mov %eax,%ebx; cmp %ebx; ja; mov %ebx,%eax`
// 两条中转（同 36 条，掩码段寄存器分配差异）。两形实测（/tmp/p1/t.cpp f_inline/f_reg、
// t2.cpp g1..g7、t3.cpp h1..h6、4.4.6 后端同形）均差 2 条 ⇒ 跨版本 CSE 结构性封顶。
bool CEquipItem::checkItemUpgradePossible() const
{
    if (IsImpossibleUpgrade())
    {
        return 0;
    }
    if ((unsigned int)m_itemType < 0x16 &&
        ((1 << (unsigned int)m_itemType) & 0x3ff400U) != 0)
    {
        return 1;
    }
    return 0;
}

int CEquipItem::check_able_To_equip(char eType, int job, int level)
{
    if (is_stackable())
    {
        return 1;
    }
    if (!check_eType(eType))
    {
        return 0x17;
    }
    if (!check_job_type((char)job))
    {
        return 8;
    }
    if (!check_low_level(level))
    {
        return 0xe;
    }
    return 0;
}

bool CEquipItem::check_eType(char eType) const
{
    if (eType > 0x15)
    {
        return 0;
    }
    if ((char)m_itemType == eType)
    {
        return 1;
    }
    return 0;
}
ExpBonusPointData* CEquipItem::getExpBonusPoint() const
{
    return (ExpBonusPointData*)&m_expBonus1e8;
}

int CEquipItem::getGrowthGrade(int level) const
{
    if (m_growthFlag != 0)
    {
        return getLevelLinearAbility(m_growthA, m_growthB, m_growthC,
                                     m_growthD, level);
    }
    return m_grade;
}

int CEquipItem::getGrowthRepairCost(int level) const
{
    if (m_growthFlag != 0)
    {
        return getLevelLinearAbility(m_growthA, m_growthB, m_growthE,
                                     m_growthF, level);
    }
    return m_repairCost;
}

// ============================================================================
// CEquipItem::getUpgradeCostDiscount @ 0x822ca80
//   返回 +0x1cc 的 float 折扣率（强化成本折扣；ORIG flds 位读）。
// ============================================================================
float CEquipItem::getUpgradeCostDiscount() const
{
    return m_field1cc;   // ORIG flds 0x1cc 位读直返
}

WpBonusPointData* CEquipItem::getWpBonusPoint() const
{
    return (WpBonusPointData*)&m_wpBonus1e0;
}
int CEquipItem::get_aura_avatar_option_value()
{
    return m_auraOptionValue;
}

bool CEquipItem::isAmplifyPollutionItem() const
{
    return m_pollutionFlag;
}

bool CEquipItem::isExistUpgradableLevelTag()
{
    return m_upgradeLevel.size() != 0;
}

unsigned char CEquipItem::isGrowthType() const
{
    return m_growthFlag;
}

bool CEquipItem::isUpgradableLevel(ItemUpgradeType::T type, int level)
{
    std::map<ItemUpgradeType::T, std::pair<int, int> >::iterator it =
        m_upgradeLevel.find(type);
    if (it != m_upgradeLevel.end())
    {
        if (level < it->second.first || it->second.second < level)
        {
            return 0;
        }
    }
    return 1;
}

bool CEquipItem::is_aura_avatar_option(char option)
{
    if ((unsigned char)m_auraOptionType == (char)option)
    {
        return 1;
    }
    return 0;
}

bool CEquipItem::verify()
{
    if (m_usablePeriod != 0 || m_expirationDate != 0)
    {
        if ((GetItemType() < 0 || GetItemType() > 9) &&
            GetItemType() != 0xb &&
            GetItemType() != 0x16 &&
            GetItemType() != 10)
        {
            return 0;
        }
    }
    return 1;
}

// ===================== make_item（ORIG 0x851098a） =====================

// [b28d-solo-CEquipItem] ORIG 0x0851098a（182/184）：仅剩两处结构性残差，N 封顶。
//   ① 位掩码段（0x08510b7f 段）ORIG 为单次读 `mov 0x234(%eax),%eax`（cmp 与 shl 共用
//      %eax）：4.1 对 `&&` 两侧同一 this->m_itemType 的两次读复用为一次；4.4 -O0 内联形
//      恒发第二次 `mov 0x8(%ebp),%eax; mov 0x234(%eax),%eax`（+2 条），register 局部形
//      换成 `mov %eax,%ebx`+`mov %ebx,%eax` 两条中转（净差同）⇒ 跨版本同表达式字段读
//      CSE 差异，封顶（探针 /tmp/p1/t.cpp f_inline/f_reg）。
//   ② G_CEnvironment 段（0x08510ba3 段）ORIG 值先入 %eax、&item+0x15 入 %edx；4.4 恒
//      互换（助记符序列全等仅寄存器角色互换 = N7；本 TU 已路由 4.4.6 后端仍存）。
void CEquipItem::make_item(Inven_Item& item) const
{
    if (IsAvatarItem())
    {
        item.m_field1 = 8;
    }
    else
    {
        item.m_field1 = 1;
    }
    if (m_itemType == 0x16)
    {
        item.m_field1 = 5;
    }
    else if (m_itemType == 0x17 || m_itemType == 0x18 || m_itemType == 0x19)
    {
        item.m_field1 = 6;
    }
    item.set_add_info(get_rand_int(0));
    item.ResetItemAttr();
    item.m_fieldb = (unsigned short)m_randomTable.m_endurance;
    if (item.m_field1 == 8)
    {
        if (m_avatarSelectIdx > 0 && m_avatarSelectIdx <
            (int)((std::vector<stAvatarTypeSelect_t>*)getAvatarTypeSelect())->size())
        {
            item.m_fieldb = (unsigned short)m_avatarSelectIdx;
        }
    }
    if (m_attachType == 3)
    {
        item.m_field0 = 1;
    }
    if (PeriodAfterUnsealing() == 0)
    {
        int t = 0;
        if (getUsablePeriod() != 0)
        {
            t = GlobalData::s_systemTime_.getCurSec() +
                getUsablePeriod() * 0x15180;
        }
        else if (getExpirationDate() != 0)
        {
            t = getExpirationDate();
        }
        if (t != 0 && m_itemType != 10)
        {
            item.m_fieldb = (unsigned short)((t - 0x44a54a80) / 0x15180);
        }
    }
    item.m_fieldd = 0;
    // ORIG（make_item）：(`inven` + 0x15) 处 0x11 字节打包全局唯一标识，位于
    // Inven_Item 的 m_amp(+0x11).m_pad4 起始（即 &item + 0x15），跨 ampen/pad 与
    // m_random 的交叠打包区。保留地址形式（打包交叠区非独立具名成员），锚定到
    // 具名子成员 m_amp.m_pad4 使偏移可审计。
    ((itemGloballyUniqueIdentifier_t*)(char*)&item.m_amp.m_pad4[0])->reset();
    if ((unsigned int)m_itemType < 0x1a &&
        ((1 << m_itemType) & 0x3bffc00U) != 0)
    {
        g_ItemGloballyUniqueIdentifierGenerator.generate(
            (itemGloballyUniqueIdentifier_t*)(char*)&item.m_amp.m_pad4[0],
            G_CEnvironment()->m_serverEnvir.m_gcNo);
    }
    item.m_amp.reset();
}

// ===================== set_item（ORIG 0x850fd84） =====================

// [b28d-solo-CEquipItem] ORIG 0x0850fd84（669=669 AE；p167 32/32 CLEAR）修复要点：
//   ① 真语义 bug：`(T*)0x941f750` 把全局变量**地址**当对象指针 ⇒ 改取
//      `GlobalData::s_pCompoundBindCube` 的**指针值**（ORIG `mov 0x941f750,%eax`，P237）；
//   ② bool 族源字段化（P136/P300）：STItemScript/STEquipmentScript 的 11 个 char 字段
//      （m_usable[0xb]、m_fieldd0/d9/da/10f、m_6f4/5/6、m_7ac/7bc/888）改 bool 后直存；
//   ③ cMyTrace 全表达式临时链（P227）复现 ctor 前右起求 varargs + 跨 ctor 保活；
//   ④ m_field1d4 包匿名 union float 视图复现单条位拷贝（N107 取址形恒 add+mov）；
//   ⑤ Wp/ExpBonus 类型统一（stBonusPointData_t 副本删除，§4.4）+ trivial op= 整体赋值
//      复现 P246 的 dest 缓存 %ecx 块拷贝；
//   ⑥ m_880 char→unsigned char 复现两段零扩展装载；
//   ⑦ m_experienceAmount float 视图（替旧 `(int)` 转换 7 条）、m_compound110/
//      m_additionalExp120 结构整体赋值（P246）、m_auraOptionType 直存。
void CEquipItem::set_item(STEquipmentScript& script)
{
    m_index = script.m_field0;
    m_grade = script.m_field4;
    m_field0c = script.m_field14;
    m_field30 = script.m_field24;
    m_field10 = script.m_field18;
    m_field14 = script.m_field1c;
    m_rarity = script.m_field10;
    m_char60 = script.m_fieldfc;
    m_pair24.first = script.m_pair28.first;
    m_pair24.second = script.m_pair28.second;
    if (script.m_field30)
    {
        G_CDataManager()->set_AuctionRegFreeCeraItem(m_index);
    }
    m_boolF8 = script.m_field31;
    char buf[500] = {0};
    strcpy(buf, script.m_strac.c_str());
    if (!CodePage::script2Database((char*)script.m_strac.c_str(), buf))
    {
        // [b28d-solo-CEquipItem] ORIG 在 ctor 之前右起求 varargs 并跨 ctor 保活
        // （edi/esi/ebx）⇒ P227 全表达式临时链形（具名 trace 会先构造后求值）。
        cMyTrace(__PRETTY_FUNCTION__, 0x553, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), script.m_strac.c_str(),
            __PRETTY_FUNCTION__, 0x553);
    }
    m_str2c = buf;
    m_attachType = script.m_field8;
    m_field50 = script.m_fieldc;
    m_field64 = script.m_field20;
    m_field68 = script.m_88c;
    for (int i = 0; i <= 10; ++i)
    {
        m_jobUsable[i] = script.m_usable[i];
    }
    m_usableLevel = script.m_field5c;
    m_field4c = script.m_field60;
    m_itemType = script.m_field1b4;
    m_randomTable.m_endurance = script.m_1bc;
    m_stackable = 0;
    m_int58 = script.m_184;
    m_int5c = script.m_180;
    // [b28d-solo-CEquipItem] ORIG `mov 0x750(%eax),%edx` 单条位拷贝 ⇒ 走 +0x10 float 视图
    // （float=float 直搬），旧 `(int)` 转换形多 flds/fnstcw/fistpl 七条。
    m_randomTable.m_experienceAmount = script.m_artifact738.m_expRate;
    m_randomTable.m_field14 = script.m_6bc;
    m_randomTable.m_field20 = script.m_artifact738.m_minLevel;
    if (script.m_field1b4 == 0x16)
    {
        switch (script.m_1b8)
        {
        case 0:
            user_creature::GetInstanceCreatureScriptMgr()
                ->RegisterCreatureIdAndItemId(m_randomTable.m_field14, m_index);
            break;
        case 1:
            m_randomTable.m_field1c = script.m_764;
            break;
        case 2:
            m_randomTable.Set(&script.m_vec88);
            break;
        }
    }
    m_randomTable.m_field18 = script.m_1b8;
    m_usablePeriod = script.m_fieldf4;
    m_expirationDate = script.m_fieldf8;
    m_randomTable.m_field24 = script.m_7ac;
    m_avatarSelect = script.m_avatar6c0;
    m_vec180 = script.m_vec6cc;
    m_field18c = script.m_6d8;
    m_repairCost = script.m_178;
    m_itemGroupName = script.m_field100;
    m_char78 = script.m_field104;
    m_bool79 = script.m_7bc;
    m_strb0 = script.m_str6c;
    m_int80 = script.m_fieldc8;
    m_int7c = script.m_fieldcc;
    m_impossibleUpgrade = script.m_6f4;
    m_impossibleAmplify = script.m_6f5;
    m_impossibleDisjoint = script.m_6f6;
    m_impossibleGift = script.m_fieldd0;
    m_impossibleCharacCargo = script.m_fieldda;
    m_possibleKiriProtection = script.m_fieldd9;
    m_periodAfterUnsealing = script.m_6f8;
    m_drop = script.m_specialDrop;
    script.m_specialDrop.clear();
    m_char194 = script.m_field10e;
    m_vec198 = script.m_vec7d8;
    m_vec1a4 = script.m_vec7e4;
    m_enchantVec = script.m_enchant7cc;
    m_field1bc = 0;
    if (IsAvatarItem())
    {
        if (m_grade == 3 && script.m_798 == 3)
        {
            m_field1bc = 6;
        }
        setAvatarOptionSize((int)script.m_param1c0.m_avatar264.size());
    }
    else if (GetItemType() == 0x16)
    {
        if (script.m_field10 == 2)
        {
            m_field1bc = 10;
        }
    }
    else if (script.m_field10 == 2)
    {
        if (script.m_fieldc4 == 5)
        {
            m_field1bc = 2;
        }
        else if (script.m_fieldc4 == 0x1e)
        {
            m_field1bc = 1;
        }
        else
        {
            m_field1bc = 9;
        }
    }
    else if (script.m_field10 == 3)
    {
        m_field1bc = 3;
    }
    else if (script.m_field10 == 5)
    {
        m_field1bc = 4;
    }
    else if (script.m_field10 == 4)
    {
        m_field1bc = 5;
    }
    else if (script.m_field10 == 0)
    {
        m_field1bc = 7;
    }
    else if (script.m_field10 == 1)
    {
        m_field1bc = 8;
    }
    m_isRoutingItem = script.m_field10f;
    m_vecC0 = script.m_vec110;
    m_questBonus = script.m_810;
    m_field1c0 = script.m_17c;
    if (script.m_814 != 2)
    {
        // [b28d-solo-CEquipItem] ORIG `mov 0x941f750,%eax; mov %eax,(%esp)` = 取
        // GlobalData::s_pCompoundBindCube **指针值**（+0x941f750 存的是 void*）；
        // 旧硬编码 `(T*)0x941f750` 直传变量地址 = 真语义 bug（P237）。
        ((ExtreamDungeon::CCompound_ExtreamDun_Item*)GlobalData::s_pCompoundBindCube)
            ->ForceResultItemRule_Setting(m_index, script.m_814 != 0,
                                          script.m_818);
    }
    m_setd4 = script.m_setdc;
    m_intEC = script.m_field64;
    m_avatarSelectIdx = script.m_6e0;
    m_randomOption = script.m_888;
    m_randomGrade = script.m_889;
    m_floatFC = script.m_expertJob.m_field18;
    m_float100 = script.m_expertJob.m_field1c;
    m_vec104.clear();
    m_vec104 = script.m_expertJob.m_usable;
    // [b28d-solo-CEquipItem] ORIG dest 缓存 %ecx + 两读交错 = 8 字节结构整体赋值（P246）。
    m_compound110 = script.m_expertJob.m_variation;
    m_float118 = script.m_expertJob.m_field10;
    m_float11c = script.m_expertJob.m_field14;
    m_additionalExp120 = script.m_expertJob.m_additionalExp;
    m_set128 = script.m_set11c;
    // ORIG set_item @ 0x850fd84 +0x8a1..+0x8f9 reads STEquipmentScript
    // +0x604/+0x608/+0x60c, i.e. EquipmentParameterInfo
    // +0x444/+0x448/+0x44c.  The middle field is rounded by the x87
    // fistp sequence; the final field is copied as one dword by ORIG.
    m_field1cc = script.m_param1c0.m_upgradeCostDiscount;
    m_field1d0 = static_cast<int>(script.m_param1c0.m_upgradeProbIncrease);
    // [b28d-solo-CEquipItem] ORIG 单条 `mov 0x60c(%eax),%edx` 位拷贝 ⇒ float 视图直搬。
    m_assaultCostDiscount = script.m_param1c0.m_assaultCostDiscount;
    // [b28d-solo-CEquipItem] ORIG 两段装载 `movzbl 0x880(%eax),%eax; mov %eax,%edx`
    //（源字段 unsigned char 的零扩展 + 目标 char 转换中转）。
    m_auraOptionType = script.m_880;
    m_auraOptionValue = script.m_884;
    {
        // ORIG 依字节偏移读 STEquipmentScript +0x81c/+0x820/+0x824/+0x828
        //（+0x81c/+0x820=wpBonus、+0x824/+0x828=expBonus，具名成员见 STEquipmentScript.h）。
        // [b28d-solo-CEquipItem] ORIG dest 缓存 %ecx + 两读交错 = 8 字节整体赋值（P246）；
        // 类型统一后 trivial op= 直接块拷贝，不发射符号。
        m_wpBonus1e0 = script.m_wpBonus81c;
        m_expBonus1e8 = script.m_expBonus824;
    }
    m_avatarAbility = script.m_param1c0.m_avatar264;
    if (script.m_890 == 1)
    {
        m_pollutionFlag = 0;
    }
    else
    {
        m_pollutionFlag = 1;
    }
    m_upgradeLevel = script.m_upgrade6fc;
    m_avatarColor[0] = script.m_88a;
    m_avatarColor[1] = script.m_88b;
    // ORIG set_item @ 0x850fd84 +0x9fc..+0xa62 copies the
    // [level linear ability] values in this order: level bounds, grade
    // bounds, then repair bounds.  These names preserve the nested layout.
    m_growthFlag = script.m_69c;
    m_growthA = script.m_param1c0.m_growthLevelBegin;
    m_growthB = script.m_param1c0.m_growthLevelEnd;
    m_growthC = script.m_param1c0.m_growthGradeBegin;
    m_growthD = script.m_param1c0.m_growthGradeEnd;
    m_growthE = script.m_param1c0.m_growthRepairBegin;
    m_growthF = script.m_param1c0.m_growthRepairEnd;
    m_char144 = script.m_fieldd8;
    m_parentBoosterItemId = 0;
}


// ===================== 简单虚函数 / 访问器（ORIG weak） =====================

int CEquipItem::GetItemType() const
{
    return m_itemType;
}

void CEquipItem::setAvatarOptionSize(int size)
{
    m_avatarOptionSize = size;
}

int CEquipItem::getAvatarOptionSize() const
{
    // ORIG W 0x81803bc：返回 +0x1c4（m_avatarOptionSize）。
    return m_avatarOptionSize;
}

bool CEquipItem::IsAvatarItem() const
{
    if (m_itemType >= 0 && m_itemType <= 9)
    {
        return 1;
    }
    return 0;
}

bool CEquipItem::IsCreatureItem() const
{
    if (m_itemType > 0x15 && m_itemType <= 0x19)
    {
        return 1;
    }
    return 0;
}

bool CEquipItem::IsClearAvatar() const
{
    return m_randomTable.m_field24;
}

bool CEquipItem::IsEnchantalbe() const
{
    if (m_itemType > 9 && m_itemType <= 0x15)
    {
        return 1;
    }
    return 0;
}

bool CEquipItem::getUsableItemType(int type) const
{
    return 0;
}

bool CEquipItem::isEnableAvatarFunc(ENUM_AVATAR_FUNC_FILTER filter) const
{
    return (m_field18c & (int)filter) == (int)filter;
}

int CEquipItem::GetGuildPowerWarPoint() const
{
    return m_int80;
}

int CEquipItem::GetUserPowerWarPoint() const
{
    return m_int7c;
}

bool CEquipItem::IsImpossibleUpgrade() const
{
    return m_impossibleUpgrade;
}

bool CEquipItem::IsImpossibleAmplify() const
{
    return m_impossibleAmplify;
}

bool CEquipItem::IsImpossibleDisjoint() const
{
    return m_impossibleDisjoint;
}

bool CEquipItem::IsImpossibleGift() const
{
    return m_impossibleGift;
}

bool CEquipItem::IsRandomOption() const
{
    return m_randomOption;
}

int CEquipItem::GetRandomOptionGrade() const
{
    return (signed char)m_randomGrade;
}

void CEquipItem::set_parent_booster_item_id(unsigned long id)
{
    m_parentBoosterItemId = id;
}

unsigned long CEquipItem::get_parent_booster_item_id() const
{
    return m_parentBoosterItemId;
}

int CEquipItem::get_endurance() const
{
    return m_randomTable.m_endurance;
}

void* CEquipItem::getAvatarTypeSelect() const
{
    return (void*)&m_avatarSelect;
}
int CEquipItem::getAvatarPeriod(unsigned char a) const
{
    // ORIG 0x8151120：result 默认 = getUsablePeriod(); 若 a < avatarSelect.size()，
    // 则 result = avatarSelect[a].m_field0。
    // ORIG 经 getAvatarTypeSelect() 取视图且比较为有符号（cmp+setl）→ 同型复刻。
    const std::vector<stAvatarTypeSelect_t>* pSel =
        (const std::vector<stAvatarTypeSelect_t>*)getAvatarTypeSelect();
    int result = getUsablePeriod();
    if ((int)a < (int)pSel->size())
        result = (*pSel)[a].m_field0;
    return result;
}

// ORIG 0x8150f36：填充 emblem 的小孔类型。
// 1) 先将 m_vec180（vector<ushort>）逐项写入 emblem.stEmblemSocket[i].emblemSocketType_
// 2) 依 slot 选择 m_avatarSelect 项并写入其 socketType 数组（index 续接）
// [b28d-solo-CEquipItem] 逐条还原（ORIG 0x8150f36）：
//   ① 第一循环 ORIG 无 index<5 上限（P140：ORIG 无防御就照抄直访）且 end() 循环前求值一次
//      （P13 缓存 end）；
//   ② slot 范围检查与 empty() 检查是**两个独立 if**（ORIG 各出一段 bool 物化，
//      P133 独立守卫不可合并）；
//   ③ slot==-1 分支同样缓存 end()；
//   ④ 残余：ORIG `add $0x8,%ecx; movzwl 0x4(%eax,%ecx,2)` 是 4.1 对 +0x14 数组下标的
//      add+disp 拆形，4.4 恒折单 disp 0x14(%eax,%edx,2)（P30/N4 结构性伪影，封顶）。
void CEquipItem::getAvatarSocket(int slot, stAvatarEmblemInfo_t& emblem) const
{
    size_t index = 0;
    std::vector<unsigned short>::const_iterator it = m_vec180.begin();
    std::vector<unsigned short>::const_iterator e = m_vec180.end();
    for (; it != e; ++it)
    {
        emblem.stEmblemSocket[index].emblemSocketType_ = *it;
        index++;
    }

    if (slot < -1 || slot >= (int)m_avatarSelect.size())
        return;
    if (m_avatarSelect.empty())
        return;

    int selIndex = slot;
    if (slot == -1)
    {
        // 找第一个 m_field10 > 0 的选项
        std::vector<stAvatarTypeSelect_t>::const_iterator it2 =
            m_avatarSelect.begin();
        std::vector<stAvatarTypeSelect_t>::const_iterator e2 =
            m_avatarSelect.end();
        while (it2 != e2)
        {
            ++selIndex;
            if (it2->m_field10 > 0)
                break;
            ++it2;
        }
        if (it2 == e2)
            return;
    }

    const stAvatarTypeSelect_t* p = &m_avatarSelect.at(selIndex);
    if (index + (size_t)p->m_field10 > 4)
        return;

    // ORIG 增量段序：先 index++ 再 i++（`addl $0x1,-0x18` 在 `addl $0x1,-0xc` 之前）。
    for (int i = 0; i < p->m_field10; ++index, ++i)
        emblem.stEmblemSocket[index].emblemSocketType_ = p->m_socketType[i];
}

int CEquipItem::GetUsableMaxLevel() const  // ORIG 084e9700
{
    return m_field1c0;
}

// ORIG _ZNK10CEquipItem20getEnchantSystemDataEv W 0x0854b544：返回 +0x1b0（m_enchantVec）
const std::vector<STEnchantSystemData>& CEquipItem::getEnchantSystemData() const
{
    return m_enchantVec;
}

// ORIG 0x833eecc：返回 this+0x160（脚本写入的 sub type 选择值，见 set_item 报告）
int CEquipItem::get_sub_type() const
{
    return m_randomTable.m_field18;
}

// ORIG W 0x833eea6：flds [this+0x158]，即 m_randomTable.m_field10 的 float 位模式。
float CEquipItem::get_experience_amount() const
{
    // [b28c-21] ORIG `mov 0x158(%eax),%eax` 单条；float 位视图成员直读。
    return m_randomTable.m_experienceAmount;
}

// ORIG W 0x833eee8：返回 this+0x168（m_randomTable.m_field20）。
int CEquipItem::get_creature_minimum_level() const
{
    return m_randomTable.m_field20;
}

// ORIG W 0x0833eebe：返回 this+0x15c 的 creature species。
int CEquipItem::get_creature_species() const
{
    return m_randomTable.m_field14;
}

// ORIG _ZNK10CEquipItem17get_hatch_item_idEv W 0x0833eeda size 0xe（nm -S + objdump
// 实证：push ebp/mov esp,ebp/mov 0x8(%ebp),%eax/mov 0x164(%eax),%eax/pop ebp/ret），
// 即 m_randomTable.m_field1c（蛋孵化物品 id，set_item case 1 由脚本 m_764 写入）。
// 本 TU 提供该 W 符号的强定义；调用方 user_creature::CEgg::Hatch（0x833912a）
// 按 ORIG 形态外联调用它（非内联）。
int CEquipItem::get_hatch_item_id() const
{
    return m_randomTable.m_field1c;
}

// ===================== RandomItemTable::Set（G6 物品表；ORIG T 0x89bba40）=====================
// [b28d-solo-CEquipItem] ORIG 0x089bba40 逐条还原：
//   1. m_vec4.clear(); m_field0 = 0;
//   2. 遍历 source：pair 默认构造；pr.first = *it；it++；
//      若 it == end 则 break；否则 m_field0 += *it；
//      **pr.second = m_field0**（累计权重上界，GetRandomItemIndex 依此判定区间；
//      旧重建漏写该赋值 = 真语义 bug）；m_vec4.push_back(pr)；it++。
// 增量用后置 it++（ORIG 增量段为 operator++(int)/ppEi，P28 增量侧推论）。
void RandomItemTable::Set(std::vector<int>* source)
{
    m_vec4.clear();
    m_field0 = 0;
    std::vector<int>::iterator it = source->begin();
    while (it != source->end())
    {
        std::pair<int, int> pr;
        pr.first = *it;
        it++;
        if (it == source->end())
            break;
        pr.second = *it;
        m_field0 += pr.second;
        pr.second = m_field0;
        m_vec4.push_back(pr);
        it++;
    }
}

// ===================== RandomItemTable::GetRandomItemIndex（ORIG 0x089bbb6c，const）=====================
// 遍历 m_vec4（pair<int,int> 数组，+0x04）：找第一个 it->second > index 的元素，
// 返回其 it->first；未命中返回 0。SetEggItem 依赖此把随机权重映射到物品 index。
int RandomItemTable::GetRandomItemIndex(int index) const
{
    // [b28c-21 代落 C28L35 spec] ORIG 增量用后置 ++（operator++(int) 三参形）。
    for (std::vector<std::pair<int, int> >::const_iterator it = m_vec4.begin();
         it != m_vec4.end(); it++)
    {
        if (index < it->second)
            return it->first;
    }
    return 0;
}
