// ============================================================================
// CItemUpgrade_Separate.cpp — WongWork 分离强化系统全链 + GenuineDamageUpgradeTable
//                             （Ctor2_ContB）
//
// 全部实现按 docs/class_func_reports/{WongWork__CItemUpgrade_Separate,
// WongWork__IItemUpgrade,GenuineDamageUpgradeTable,CUser__makeGuildItemSeparateUpgradMessage}/
// 报告 + ORIG objdump 逐段转写。符号对照：
//   WongWork::IItemUpgrade        C1 T 0x080fc834 / D1 T 0x080fc842 /
//                                 item_upgrade_system T 0x080fc850
//   WongWork::STUpgradeInfo       C1 W 0x0811edfe
//   GenuineDamageUpgradeInfo      C1 W 0x0891baf0
//   OutLineEFFECTInfoTemp         C1 W 0x0891bb1e
//   GenuineDamageUpgradeTable     C1 W 0x0811ee5e / D1 W 0x0811eeda /
//                                 getGenuineUpgradeDamage T 0x08912a54 /
//                                 getGenuineUpgradeMaterialCnt T(_ZNK) 0x08912b88 /
//                                 getEffectInfo T 0x08912d10
//   UpgradeSeparateInfo           IncUpgradeSeparate W 0x0811ed32（声明补入 Inven_Item.h）
//   WongWork::CItemUpgrade_Separate C1 T 0x0811dfc0 / D1 T 0x0811e0be /
//                                 ProcUpgrade T 0x0811e66e / LoadUpgradeScript T 0x0811eca8 /
//                                 _CheckCondition T 0x0811e138 / _GetPenaltyType T 0x0811e2ce /
//                                 _GetUpgradeInfo T 0x0811e2e2 / _DoProcUpgrade T 0x0811e468 /
//                                 _NoticeUpgrade T 0x0811e53a / getUpgradeInfoTable W 0x0811ee16
//   CUser::makeGuildItemSeparateUpgradMessage T 0x0867a242（CUser.h 补声明）
// importGenuineUpgradeTable(T 0x08912336) 仅声明——定义归脚本导入域批次。
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

// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 最前，使 `_GLOBAL__I_` 名等于
// ORIG 的 `_ZN8WongWork21CItemUpgrade_SeparateC2Ev`（P327）。全限定定义可写在
// 命名空间块之外 ⇒ 发射序提前。语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
// ORIG T 0x0811dfc0：基类 vptr → 本类 vptr → 表构造 → +0x190=100000 →
// new CMTRand(time) 存 +0x18c → 100 次 randInt(m_randRange) 热身。
WongWork::CItemUpgrade_Separate::CItemUpgrade_Separate()
{
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

union FloatBits
{
    unsigned int u;
    float f;
};

// ORIG .rodata 常量（位模式实证）：
FloatBits kMatEpsilon = { 0x33d6bf95UL };   // DAT_08e122f4 ≈ 1e-7f（材料放大前加值）
const float kGoldBase = 9840.0f;            // DAT_08b575b0（金币换算基数）
const float kRoundHalf = 0.5f;              // DAT_08e122e8（截断前置）

int truncateToInt(float value)
{
    // ORIG：fnstcw/fldcw 0xc 置截断后 fistp（含 64 位变体取低 dword）。
    return static_cast<int>(value);
}

}  // namespace

// ============================ POD 生命周期 ============================

GenuineDamageUpgradeInfo::GenuineDamageUpgradeInfo()          // ORIG W 0x0891baf0
{
    m_a = 0;
    m_b = 0;
    m_levelRate = 0.0f;     // +0x08（ctor 清零；import 写 level/10000f）
    m_goldRate = 0.0f;      // +0x0c；+0x10 保持未初始化（ORIG 同）
}

OutLineEFFECTInfoTemp::OutLineEFFECTInfoTemp()                // ORIG W 0x0891bb1e
{
    m_a = 0;
    m_b = 0;
    m_c = 0;
    m_c = 0;                // ORIG 对 +0x8 连续写两次（死存储忠实保留）
}

namespace WongWork
{

STUpgradeInfo::STUpgradeInfo()                                // ORIG W 0x0811edfe
{
    m_materialCnt = 0;
    m_gold = 0;
}

// ============================ IItemUpgrade ============================

IItemUpgrade::IItemUpgrade() {}                               // ORIG T 0x080fc834

IItemUpgrade::~IItemUpgrade() {}                              // ORIG T 0x080fc842（非虚）

// ORIG T 0x080fc850：put_header(1,cmd) → 虚调槽 0 → 0=finalize+Send / 否则错误包。
// [b28d-solo-CItemUpgrade_Separate] AE：`while (result == 0) { …; return; }` + 尾随
// else 体复现 ORIG 的 `cmpl $0; sete %al; test %al,%al; je <else>` 物化 + then 尾
// 共享尾 jmp（探针 /tmp/b28d_51/p7.cpp g 逐条全等；if/else 形 4.4 恒折 jne 直跳）。
// 虚调目标 %edx 亦已对齐（原 N7 残余随 S125 CDataManager.h static 化后消失）。
void IItemUpgrade::item_upgrade_system(CUser* user, MSG_ITEM_UPGRADE* msg, int cmd)
{
    int result = 0;
    PacketGuard pg;
    pg.put_header(1, cmd);
    result = this->ProcUpgrade(user, msg, &pg);
    while (result == 0)
    {
        pg.finalize(true);
        user->Send(pg);
        return;
    }
    user->SendCmdErrorPacket2Byte(static_cast<ENUM_CMDPACKET>(cmd),
                                  static_cast<unsigned short>(result & 0xffff));
}

// ============================ GenuineDamageUpgradeTable ============================

}  // namespace WongWork

GenuineDamageUpgradeTable::GenuineDamageUpgradeTable() {}     // ORIG W 0x0811ee5e：仅三 map 构造

GenuineDamageUpgradeTable::~GenuineDamageUpgradeTable() {}    // ORIG W 0x0811eeda：三 map 析构

// ORIG T 0x08912a54。浮点读取：statWeightsByRarity[rarity]（this+(rarity+8)*4）
// 与 weaponWeights[a][b]（this+((6a+b)+12)*4+8）；分母 *(int*)this；useAlt 选择
// info.m_b / info.m_a；尾 +0.5f（DAT_08e122e8）后截断。
// [b28d-solo-CItemUpgrade_Separate] 重写：ORIG 为 result 具名 float（P232 扩展二段
// 零初始化）、altBias 运行时 flds/fmulp/fstps（pointFive/two 必须非 const 静态量，
// 否则 4.4 折 fld1）、if/else 两臂各自完整计算（非三目共享 multiplier）、
// 乘法书写序 m_b*(count/divisor+altBias)*weapon*stat 使 4.4 浮点栈序与 ORIG 逐条一致。
int GenuineDamageUpgradeTable::getGenuineUpgradeDamage(ENUM_RARITY rarity, int a, int b,
                                                       int key, int count, bool useAlt)
{
    static float pointFive = 0.5f;   // ORIG .data 093f4fb0（非 const ⇒ 4.4 不折 fld1）
    static float two = 2.0f;         // ORIG .data 093f4fb4
    float result = 0.0f;
    const float altBias = two * pointFive;

    if (key == 0)
    {
        return 0;
    }

    // P48 no-CSE：ORIG 两臂各 fresh m_infos[key]（operator[] 共 2 次），且各臂
    // 完整展开乘链（非三目共享）。
    if (useAlt)
    {
        result = static_cast<float>(m_infos[key].m_b) *
                 (static_cast<float>(count) / static_cast<float>(m_divisor) + altBias) *
                 m_weaponWeights[a][b] *
                 m_statWeightsByRarity[static_cast<int>(rarity)];
    }
    else
    {
        result = static_cast<float>(m_infos[key].m_a) *
                 (static_cast<float>(count) / static_cast<float>(m_divisor) + altBias) *
                 m_weaponWeights[a][b] *
                 m_statWeightsByRarity[static_cast<int>(rarity)];
    }

    result += kRoundHalf;
    return static_cast<int>(result);
}

// ORIG T(_ZNK…) 0x08912b88。
// [b28d-solo-CItemUpgrade_Separate] 重写：eps 死局部（-0x14，P232 扩展 float 二段
// 常量初始化）、count 具名预置 0（-0x10）、factor 1.0f 两句式（1.0f 落槽不被折）、
// 乘加拆两句（ORIG 不融合 fmuls/fadds）、第二次 end 检查照抄 ORIG 的 it==m_infos.end()
// 重复形（ORIG 0x08912c90 lea 0x158 实证；mit 无 end 检查，缺键时 ORIG/我方同为
// 越界解引用，行为一致）。p167 8=8 CLEAR。判词 N（N95/N82 + N92/N18a 族）：残余 =
// ①4.4 浅槽基恒比 ORIG 深 8（eps/count/factor -0x1c/-0x18/-0x14 vs -0x14/-0x10/-0xc；
// 帧 0x58 vs 0x68，加 char[0x10]/double pad 只会整体平移 0x10，N95 不可上移）；
// ②ORIG `mov -0x50,%eax; mov -0x4c,%edx` 的 64 位宽读 vs 4.4 只读低 dword（N92）。
int GenuineDamageUpgradeTable::getGenuineUpgradeMaterialCnt(unsigned char growth, int sepLv,
                                                            ENUM_EQUIPMENTTYPE type,
                                                            ENUM_RARITY rarity) const
{
    const float eps = 1e-7f;      // ORIG -0x14 dead init（0x33d6bf95 立即数）
    (void)eps;

    if (type != static_cast<ENUM_EQUIPMENTTYPE>(10))
    {
        return -1;
    }
    if (growth > 99)
    {
        return -1;
    }

    int count = 0;
    float factor = 1.0f;
    factor = m_itemWeightsByRarity[static_cast<int>(rarity)] * factor;

    std::map<int, GenuineDamageUpgradeInfo>::const_iterator it =
        m_infos.find(sepLv + 1);
    if (it == m_infos.end())
    {
        return -1;
    }
    // ORIG 0x08912b88：pt() 后 flds 0x10(%eax) = pair+0x10 = value+0xc = m_goldRate
    // （import 第 4 值；m_materialRate +0x10 从不被 import 写入）
    factor = it->second.m_goldRate * factor;
    factor = kMatEpsilon.f + factor;   // ORIG 0x08912c59：flds factor 先于 flds kMatEpsilon

    std::map<int, GenuineDamageUpgradeMaterialInfo>::const_iterator mit =
        m_materials.find(static_cast<int>(growth));
    if (it == m_infos.end())   // ORIG 照抄：第二次检查对象仍是 it/m_infos（0x08912c90）
    {
        return -1;
    }

    const long long wide = static_cast<long long>(
        static_cast<float>(mit->second.m_materialCount) * factor);   // ORIG fistpll
    count = static_cast<int>(wide);                                  // 取低 dword
    if (count == 0)
    {
        return 1;
    }
    return count;
}

// ORIG T 0x08912d10（sret 返回；静态局部 temp 经 __cxa_guard 初始化）。
// [b28c-32] verdict=DIFF：N86/N82（sret 按值返回类对象 + 函数局部 static guard 临时槽跨版本差）。
// [b28d-solo-CItemUpgrade_Separate] 修布局：`if (m_maxLevel < key) return temp;` 早返形
// 复现 ORIG 0x08912d53 的 `jge <find块>` 正极性（旧 `>=` 包裹形 4.4 产 `jl <temp块>` 反向
// 布局）。残余 N86/N82：ORIG 两处独立 temp 拷贝块 vs 4.4 共享尾块（sret/临时槽跨版本差）。
OutLineEFFECTInfoTemp GenuineDamageUpgradeTable::getEffectInfo(int key)
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

// ORIG T 0x0811e0be：delete m_rand 置空 → 表析构 → 基类析构（成员逆序自动）。
CItemUpgrade_Separate::~CItemUpgrade_Separate()
{
    if (m_rand != 0)
    {
        delete m_rand;
    }
    m_rand = 0;
}

// ORIG T 0x0811eca8：import 尾透传（int 返回零开销透传 eax；0=成功）。
bool CItemUpgrade_Separate::LoadUpgradeScript(const char* path)
{
    return importGenuineUpgradeTable(path, &m_table);
}

// ORIG T 0x0811e2ce：恒返回 1（报告全文仅此）。
int CItemUpgrade_Separate::_GetPenaltyType(unsigned char /*a*/, ENUM_RARITY /*rarity*/,
                                           const GenuineDamageUpgradeTable& /*table*/,
                                           const GenuineDamageUpgradeInfo* /*info*/)
{
    return 1;
}

// ORIG T 0x0811e2e2。
// [b28d-solo-CItemUpgrade_Separate] 判词 N（ORIG 0x0811e2e2，setcc-fold + N7 族）：
// 修法——条件内联 GetUpgradeSeparate（含 0x0811e30e 死调用，p167 11=11 CLEAR）、
// rarity/itemType 内联实参、gold 走 long long+fistpll。残余：①ORIG `!(a()>b)` 的
// `jg;mov $1/%0;test` 物化 vs 4.4 `setle;test` 直跳（setcc-fold，跨版本后端差）；
// ②虚调目标 ORIG %edx vs 4.4 %eax + rarity/itemType 的 %ebx/%esi 角色互换（N7，
// P230 判 4.4.6 后端敏感族）。帧 push esi/ebx+sub $0x50 与 ORIG 全等；语义/调用集齐。
bool CItemUpgrade_Separate::_GetUpgradeInfo(const Inven_Item* item, const CItem* citem,
                                            const GenuineDamageUpgradeTable& table,
                                            STUpgradeInfo* out, int level)
{
    // [b28d-solo-CItemUpgrade_Separate] ORIG 0x0811e2f3/0x0811e30e：条件内联
    // GetUpgradeSeparate（4.1 `!(a() > b)` 求值两次，第二次结果丢弃）+ movzbl/int 比较
    // （jg 有符号），复现两次调用 + jg 形。
    if (!(static_cast<int>(item->m_upgradeSep.GetUpgradeSeparate()) > table.m_maxLevel))
    {
        (void)item->m_upgradeSep.GetUpgradeSeparate();   // ORIG 0x0811e30e 死调用（结果丢弃）
        return false;
    }

    const unsigned char growth =
        static_cast<unsigned char>(
            reinterpret_cast<const CEquipItem*>(citem)->getGrowthGrade(level));
    // ORIG 0x0811e34d/0x0811e35f：rarity→%ebx、itemType→%esi 内联实参（无具名槽）。
    const int materialCnt = table.getGenuineUpgradeMaterialCnt(
        growth,
        static_cast<int>(static_cast<unsigned char>(
            item->m_upgradeSep.GetUpgradeSeparate())),
        static_cast<ENUM_EQUIPMENTTYPE>(citem->GetItemType()),
        static_cast<ENUM_RARITY>(citem->get_rarity()));
    if (materialCnt < 0)
    {
        return false;
    }

    std::map<int, GenuineDamageUpgradeInfo>::const_iterator it =
        table.m_infos.find(
            static_cast<int>(static_cast<unsigned char>(
                item->m_upgradeSep.GetUpgradeSeparate())) + 1);
    if (it == table.m_infos.end())
    {
        return false;
    }

    out->m_materialCnt = materialCnt;
    // ORIG 0x0811e138：pt() 后 flds 0xc(%eax) = pair+0xc = value+0x8 = m_levelRate；
    // fistpll 64 位截断后取低 dword（ORIG 0x0811e445 + mov -0x38/-0x34）。
    const long long goldWide =
        static_cast<long long>(kGoldBase * it->second.m_levelRate);
    out->m_gold = static_cast<int>(goldWide);
    return true;
}

// ORIG T 0x0811e138（const）。
// [b28d-solo-CItemUpgrade_Separate] 判词 N（ORIG 0x0811e138，N7 族）：修法——haveCount
// 直用 matItem.m_addInfo2（%edi 跨调用保活）、rarity/itemType/sepLv 全部内联实参、
// materialItemIdx 具名槽、m_fieldb 双零扩展 32 位比较、两次独立虚调 GetItemType
// （P282 真语义缺口已补）。p167 10=10 CLEAR；gap=1（133 vs 132）。残余仅 N7：
// 虚调目标 ORIG %edx vs 4.4 %eax、rarity/itemType 的 %ebx/%esi 角色互换及实参槽
// 0xc/0x10 互换（P230 判 4.4.6 后端敏感族）。
int CItemUpgrade_Separate::_CheckCondition(const CUser* /*user*/, const Inven_Item& item,
                                           const CItem* citem, const Inven_Item& matItem,
                                           const GenuineDamageUpgradeTable& table,
                                           int level) const
{
    const unsigned char growth = static_cast<unsigned char>(
        reinterpret_cast<const CEquipItem*>(citem)->getGrowthGrade(level));

    std::map<int, GenuineDamageUpgradeMaterialInfo>::const_iterator mit =
        table.m_materials.find(static_cast<int>(growth));
    if (mit == table.m_materials.end())
    {
        return 1;
    }
    const int materialItemIdx = mit->second.m_materialItemIdx;   // ORIG -0x1c 具名槽
    if (matItem.m_addInfo != materialItemIdx)
    {
        return 0x16;
    }

    // [b28d-solo-CItemUpgrade_Separate] ORIG 0x0811e1e2：haveCount 直用 matItem.m_addInfo2
    // （%edi 跨调用保活），rarity/itemType/sepLv 全部内联实参（%ebx/%esi/%edx，右→左求值）；
    // P282/P48：0x0811e1fa 与 0x0811e24e 两次独立虚调 citem->GetItemType()（vptr+0xc）。
    if (matItem.m_addInfo2 <
        static_cast<unsigned int>(const_cast<GenuineDamageUpgradeTable&>(table)
            .getGenuineUpgradeMaterialCnt(
                growth,
                static_cast<int>(static_cast<unsigned char>(
                    item.m_upgradeSep.GetUpgradeSeparate())),
                static_cast<ENUM_EQUIPMENTTYPE>(citem->GetItemType()),
                static_cast<ENUM_RARITY>(citem->get_rarity()))))
    {
        return 0x16;
    }
    if (citem->GetItemType() != 10)
    {
        return 0x13;
    }
    // ORIG 0x0811e271：movzwl 0xb(%eax),%eax + movzwl %ax,%ebx（双零扩展 ⇒ 32 位比较）
    if (static_cast<int>(item.m_fieldb) !=
        static_cast<int>(
            reinterpret_cast<const CEquipItem*>(citem)->get_endurance()))
    {
        return 7;
    }
    // [b28d-51] P48：ORIG 0x0811e29c 末检再次独立调 GetUpgradeSeparate
    // （不复用 sepLv 局部）——p167 差集曾缺 1 个该调用。
    if (table.m_maxLevel <
        static_cast<int>(item.m_upgradeSep.GetUpgradeSeparate()) + 1)
    {
        return 0x5f;
    }
    return 0;
}

// ORIG T 0x0811e468。
// [b28d-solo-CItemUpgrade_Separate] 判词 N（ORIG 0x0811e468，setcc-fold/N2 族；p167
// 6=6 CLEAR）：形 k
// （探针 /tmp/b28d_51/p9.cpp k）复现 gold@-0xc、limit@%ebx 跨 randInt 保活、range@-0x10、
// 「limit 先求值→range 内联赋值→randInt」求值序、push %ebx+sub $0x24 帧。残余 5 条：
// ORIG 的 failed 为 4.1 寄存器化分支赋值（`jae;mov $1,%eax;jmp;mov $0,%eax;test %al,%al;je`），
// 4.4 对 `<` 恒 setcc（setb+test+je）且具名 bool 恒槽化（register/无初始化/两分支赋值
// p/q/m 三形实测：setb+分支写槽或 setb %bl），跨版本后端差不可造。语义/调用集齐。
bool CItemUpgrade_Separate::_DoProcUpgrade(CUser* user, Inven_Item* item,
                                           const STUpgradeInfo* info)
{
    if (user->IsGameMasterMode())
    {
        if (user->getUpgradeSeparateTrueFlag() == 1)
        {
            item->m_upgradeSep.IncUpgradeSeparate();
            return true;
        }
        if (user->getUpgradeSeparateFlaseFlag() == 1)
        {
            return false;
        }
    }

    const int gold = info->m_gold;
    if (gold != 0)
    {
        unsigned long range;
        if (static_cast<unsigned int>(gold * 10) <
            static_cast<unsigned int>(m_rand->randInt(range = m_randRange)))
        {
            return false;
        }
    }
    item->m_upgradeSep.IncUpgradeSeparate();
    return true;
}

// ORIG T 0x0811e53a：header 0x56 全服广播分离强化结果。
void CItemUpgrade_Separate::_NoticeUpgrade(const CUser* user, const Inven_Item& item,
                                           bool success)
{
    PacketGuard pg;
    pg.put_header(0, 0x56);
    pg.put_byte(10);
    // ORIG movzbl -0x1c,%eax 直装参：bool 隐式转 int 即零扩展（三目反生岛）。
    pg.put_byte(success);
    pg.put_short(user->get_unique_id());
    pg.put_int(item.m_addInfo);
    pg.put_byte(item.GetUpgrade());
    pg.put_byte(item.m_upgradeSep.GetUpgradeSeparate());
    // Inven_Item+0x25 = m_random(+0x1f).m_random（RandomOption，+0x06）。
    item.m_random.m_random.put_packet_random_option(pg);
    pg.finalize(true);
    G_GameWorld()->send_all(pg);
}

// ORIG T 0x0811e66e：vtable 槽 0 主流程。
// [b28d-solo-CItemUpgrade_Separate] 判词 N（ORIG 0x0811e66e，N7 + setcc-fold 族）：
// 修法——matItem 默认构造+赋值（补 Inven_Item::C1Ev）、第三次 getCurCharacInvenW、
// get_charac_level 两处内联、success 三目展开为 if/else（补第 5 个 put_byte call）、
// IsRandomOption 正极性两臂各自内联 makeGuild/GetUpgrade/GetUpgradeSeparate、
// msg 槽位改 movzwl 零扩展、find_item 链式 receiver。p167 43=43 CLEAR（0 missing/
// 0 extra）。残余：虚调目标 %eax vs %edx（N7）+ `cond!=0` setne 物化（setcc-fold）；
// 帧 push edi/esi/ebx 与 ORIG 全等。按 §6.2 判语义完成、byte 层封顶。
int CItemUpgrade_Separate::ProcUpgrade(CUser* user, MSG_ITEM_UPGRADE* msg,
                                       PacketGuard* packet)
{
    CInventory* invenW = user->getCurCharacInvenW();

    Inven_Item item = invenW->GetInvenSlot(INVEN_TYPE_EQUIP,
                                           msg->m_targetSlot);  // MSG_ITEM_UPGRADE +0x1b（movzwl 零扩展）
    if (item.isEmpty())
    {
        return 4;
    }
    if (item.m_addInfo !=        // Inven_Item +0x02
        msg->m_targetItemIdx)    // MSG_ITEM_UPGRADE +0x1d
    {
        return 4;
    }

    // ORIG 0x0811e6f5/0x0811e7e9：matItem 先 Inven_Item::C1Ev 默认构造（-0xf6），
    // 再由 GetInvenSlot 的 sret 临时（-0x138）逐字段隐式赋值拷贝。
    Inven_Item matItem;
    matItem = invenW->GetInvenSlot(INVEN_TYPE_EQUIP,
                                   msg->m_materialSlot);  // +0x23（movzwl 零扩展）
    if (matItem.isEmpty())
    {
        return 0x16;
    }

    // ORIG 0x0811e80d：item.m_addInfo 先入 %ebx 跨 G_CDataManager call 保活（P52 链式）。
    CItem* citem = G_CDataManager()->find_item(item.m_addInfo);
    if (citem == 0)
    {
        return 4;
    }

    const GenuineDamageUpgradeTable* table = getUpgradeInfoTable();
    STUpgradeInfo info;

    // ORIG 0x0811e850/0x0811e8a1：get_charac_level 两处内联独立调用（P48 无 CSE）。
    if (!_GetUpgradeInfo(&item, citem, *table, &info, user->get_charac_level()))
    {
        return 0xd;
    }

    const int cond = _CheckCondition(user, item, citem, matItem, *table,
                                     user->get_charac_level());
    if (cond != 0)
    {
        return cond;
    }

    const unsigned char sepOld =
        static_cast<unsigned char>(item.m_upgradeSep.GetUpgradeSeparate());

    const bool success = _DoProcUpgrade(user, &item, &info);

    const unsigned short matSlot = msg->m_materialSlot;   // MSG_ITEM_UPGRADE +0x23
    CInventory* invenW2 = user->getCurCharacInvenW();
    if (invenW2->delete_item(INVEN_TYPE_EQUIP, matSlot, info.m_materialCnt,
                             static_cast<eItemDelReason>(0xc), true) == 0)
    {
        return 0x11;
    }

    packet->put_byte(1);
    packet->put_short(matSlot);

    // ORIG 0x0811e94a：第三次 getCurCharacInvenW（receiver 右值直落）。
    Inven_Item refreshed =
        user->getCurCharacInvenW()->GetInvenSlot(INVEN_TYPE_EQUIP, matSlot);
    packet->put_int(refreshed.get_add_info());
    packet->put_byte(static_cast<char>(sepOld));

    if (static_cast<unsigned char>(item.m_upgradeSep.GetUpgradeSeparate()) > 5)
    {
        _NoticeUpgrade(user, item, success);
    }

    // ORIG 0x0811ea54：三目两臂各发独立 put_byte call（4.4 折单 call 丢 1 个调用点）。
    if (success)
    {
        packet->put_byte(0);
    }
    else
    {
        packet->put_byte(1);
    }

    const unsigned char sepNow =
        static_cast<unsigned char>(item.m_upgradeSep.GetUpgradeSeparate());
    packet->put_byte(sepNow);

    const unsigned short slot = msg->m_targetSlot;   // MSG_ITEM_UPGRADE +0x1b
    packet->put_short(slot);

    if (item.m_addInfo != 0)   // Inven_Item +0x02
    {
        invenW->update_item(INVEN_TYPE_EQUIP, slot, item);
    }

    // ORIG 0x0811eb91：sepNow 实参内联 GetUpgradeSeparate（不用 -0x1c 变量）。
    user->m_historyLog.SeparateUpgradeItem(
        citem->get_index(), sepOld,
        static_cast<int>(item.m_upgradeSep.GetUpgradeSeparate()), success);

    // ORIG 0x0811ebd8/0x0811ec41：IsRandomOption 正极性两臂，各自内联完整实参
    // （makeGuild/GetUpgrade/GetUpgradeSeparate 各 2 次调用）。
    if (citem->IsRandomOption())
    {
        user->makeGuildItemSeparateUpgradMessage(
            msg->m_28, sepOld,
            static_cast<int>(item.m_upgradeSep.GetUpgradeSeparate()), success,
            item.GetUpgrade());
    }
    else
    {
        user->makeGuildItemSeparateUpgradMessage(
            citem->GetItemName(), sepOld,
            static_cast<int>(item.m_upgradeSep.GetUpgradeSeparate()), success,
            item.GetUpgrade());
    }
    return 0;
}

}  // namespace WongWork

// ============================ UpgradeSeparateInfo ============================

// ORIG W 0x0811ed32：低 5 位自增（模 32），保留高位（bit5=交易限制等）。
void UpgradeSeparateInfo::IncUpgradeSeparate()
{
    // [b28d-solo-CItemUpgrade_Separate] AE：位域前置自增 `++m_bits.m_lo` 复现 ORIG
    // 0x0811ed32 的「截断掩码就地施于 %eax + 再掩码写回」形（探针 /tmp/b28d_51/p6.cpp
    // 形 b 逐条全等；`= m_lo + 1` 与 `+= 1` 恒多一条 `mov %eax,%edx` 中转）。
    ++m_bits.m_lo;
}

// ============================ CUser 公会公告 ============================

// ORIG T 0x0867a242：成功且新等级>5 用 game_server_msg_248；失败且旧等级>5 用
// game_server_msg_249；前缀 game_server_msg_247 取 10 字节；长度<120 才发公会板。
// [b28d-solo-CItemUpgrade_Separate] 判词 N（ORIG 0x0867a242，N78/N82/N89 族）：
// 修法——4 处 `char X[N]` 改 `= {0}` 初始化、board 声明序前置、消除 4 个 EXTRA
// memset call（p167 71=71 CLEAR，0 missing/0 extra）。残余：①ORIG board[120] 为
// 4.1 内联 rep stos / tail[255] 对齐剥离展开，4.4 恒 call memset（N78）；②std::string
// 构造/析构临时槽阶梯与模板实例跨版本差（N82/N89）；gap=10（516 vs 506）。按 §6.2
// 判语义完成、byte 层封顶。
void CUser::makeGuildItemSeparateUpgradMessage(const char* name, int oldLevel,
                                               int newLevel, bool success, int upgrade)
{
    // ORIG 0x0867a257/0x0867a26d：board[120] rep stos 先、prefix 三条 movl/movl/movw
    // 后（声明序 board 在前；显式 memset 会多发 4 个 call，p167 EXTRA 实证）。
    char board[120] = {0};
    char prefix[10] = {0};

    const char* prefixStr =
        g_scriptStringManager_.findString(4, "game_server_msg_247",
                                          static_cast<bool*>(0));
    std::strncpy(prefix, prefixStr, sizeof(prefix));

    bool boardReady = false;
    if (success && newLevel > 5)
    {
        char msgBuf[255];
        std::strncpy(msgBuf,
                     g_scriptStringManager_.findString(4, "game_server_msg_248",
                                                       static_cast<bool*>(0)),
                     255);

        std::string userName(getCurCharacName());
        std::string line(prefix);
        std::string body(msgBuf);

        char tail[255] = {0};
        if (upgrade == 0)
        {
            std::sprintf(tail, "%s %d", name, newLevel);
        }
        else
        {
            std::sprintf(tail, "+%d %s %d", upgrade, name, newLevel);
        }
        line += tail;
        line += body;
        userName += line;

        if (userName.length() < sizeof(board))
        {
            std::memcpy(board, userName.c_str(), userName.length());
            boardReady = true;
        }
    }
    else
    {
        if (success)
        {
            return;
        }
        if (oldLevel <= 5)
        {
            return;
        }

        char msgBuf[255];
        std::strncpy(msgBuf,
                     g_scriptStringManager_.findString(4, "game_server_msg_249",
                                                       static_cast<bool*>(0)),
                     255);

        std::string userName(getCurCharacName());
        std::string line(prefix);
        std::string body(msgBuf);

        char tail[255] = {0};
        if (upgrade == 0)
        {
            std::sprintf(tail, "%s %d", name, oldLevel + 1);
        }
        else
        {
            std::sprintf(tail, "+%d %s %d", upgrade, name, oldLevel + 1);
        }
        line += tail;
        line += body;
        userName += line;

        if (userName.length() < sizeof(board))
        {
            std::memcpy(board, userName.c_str(), userName.length());
            boardReady = true;
        }
    }

    if (boardReady)
    {
        const int guildkey = get_charac_guildkey();
        const unsigned int characNo =
            static_cast<unsigned int>(getCurCharacNo());
        CGuildServerProxy* proxy =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
        proxy->SendGuildBoardWrite(board, 0u, characNo, -1,
                                   static_cast<char>(-1),
                                   static_cast<unsigned int>(guildkey));
    }
}
