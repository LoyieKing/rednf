// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CItemUpgrade_Separate/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "CItemUpgrade_Separate/CItemUpgrade_Separate.inc"
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

#include "CItemUpgrade_Separate/truncateToInt.inc"
{
    // ORIG：fnstcw/fldcw 0xc 置截断后 fistp（含 64 位变体取低 dword）。
    return static_cast<int>(value);
}

}  // namespace

// ============================ POD 生命周期 ============================

#include "CItemUpgrade_Separate/GenuineDamageUpgradeInfo.inc"
{
    m_a = 0;
    m_b = 0;
    m_levelRate = 0.0f;     // +0x08（ctor 清零；import 写 level/10000f）
    m_goldRate = 0.0f;      // +0x0c；+0x10 保持未初始化（ORIG 同）
}

#include "CItemUpgrade_Separate/OutLineEFFECTInfoTemp.inc"
{
    m_a = 0;
    m_b = 0;
    m_c = 0;
    m_c = 0;                // ORIG 对 +0x8 连续写两次（死存储忠实保留）
}

namespace WongWork
{

#include "CItemUpgrade_Separate/STUpgradeInfo.inc"
{
    m_materialCnt = 0;
    m_gold = 0;
}

// ============================ IItemUpgrade ============================

#include "CItemUpgrade_Separate/IItemUpgrade.inc"

#include "CItemUpgrade_Separate/dtor_IItemUpgrade.inc"

// ORIG T 0x080fc850：put_header(1,cmd) → 虚调槽 0 → 0=finalize+Send / 否则错误包。
// [b28d-solo-CItemUpgrade_Separate] AE：`while (result == 0) { …; return; }` + 尾随
// else 体复现 ORIG 的 `cmpl $0; sete %al; test %al,%al; je <else>` 物化 + then 尾
// 共享尾 jmp（探针 /tmp/b28d_51/p7.cpp g 逐条全等；if/else 形 4.4 恒折 jne 直跳）。
// 虚调目标 %edx 亦已对齐（原 N7 残余随 S125 CDataManager.h static 化后消失）。
#include "CItemUpgrade_Separate/item_upgrade_system.inc"
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

#include "CItemUpgrade_Separate/GenuineDamageUpgradeTable.inc"

#include "CItemUpgrade_Separate/dtor_GenuineDamageUpgradeTable.inc"

// ORIG T 0x08912a54。浮点读取：statWeightsByRarity[rarity]（this+(rarity+8)*4）
// 与 weaponWeights[a][b]（this+((6a+b)+12)*4+8）；分母 *(int*)this；useAlt 选择
// info.m_b / info.m_a；尾 +0.5f（DAT_08e122e8）后截断。
// [b28d-solo-CItemUpgrade_Separate] 重写：ORIG 为 result 具名 float（P232 扩展二段
// 零初始化）、altBias 运行时 flds/fmulp/fstps（pointFive/two 必须非 const 静态量，
// 否则 4.4 折 fld1）、if/else 两臂各自完整计算（非三目共享 multiplier）、
// 乘法书写序 m_b*(count/divisor+altBias)*weapon*stat 使 4.4 浮点栈序与 ORIG 逐条一致。
#include "CItemUpgrade_Separate/getGenuineUpgradeDamage.inc"
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
// [2026-09-09 SweepCItemUpgrade] 复核（未改）：gap=1（orig 217 / our 216）即 ② 那 1 条
// 高 32 位读（0x08912cf8，fistpll 结果丢弃的高半字）；槽基常数差 8 为 N95。判词维持 N。
#include "CItemUpgrade_Separate/getGenuineUpgradeMaterialCnt.inc"
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
#include "CItemUpgrade_Separate/getEffectInfo.inc"
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
#include "CItemUpgrade_Separate/dtor_CItemUpgrade_Separate.inc"
{
    if (m_rand != 0)
    {
        delete m_rand;
    }
    m_rand = 0;
}

// ORIG T 0x0811eca8：import 尾透传（int 返回零开销透传 eax；0=成功）。
#include "CItemUpgrade_Separate/LoadUpgradeScript.inc"
{
    return importGenuineUpgradeTable(path, &m_table);
}

// ORIG T 0x0811e2ce：恒返回 1（报告全文仅此）。
#include "CItemUpgrade_Separate/_GetPenaltyType.inc"
                                           const GenuineDamageUpgradeTable& /*table*/,
                                           const GenuineDamageUpgradeInfo* /*info*/)
{
    return 1;
}

// ORIG T 0x0811e2e2。
// [2026-09-09 SweepCItemUpgrade] 判词 N（ORIG 0x0811e2e2，setcc-fold + N7 族）：
// 本轮复核（未改）：gap=5（orig 121 / our 116）全部为跨版本形态差——
// ①ORIG 条件物化 `jg <主体>; mov $0,%eax; jmp; mov $1,%eax; test %al,%al; je`
//   （0x0811e303..0x0811e321，6 条）vs 4.4 `setle %al; test; je`（3 条）⇒ setcc-fold；
// ②get_rarity→%ebx / 虚调 GetItemType→%esi 与 4.4 的 %esi/%ebx 相反（N7，P230 判
//   4.4.6 后端敏感族）。p167 11=11 CLEAR（0 missing/0 extra），帧 push esi/ebx +
// sub $0x50 与 ORIG 全等；语义/调用集齐，无源码可翻。
#include "CItemUpgrade_Separate/_GetUpgradeInfo.inc"
                                            const GenuineDamageUpgradeTable& table,
                                            STUpgradeInfo* out, int level)
{
    // [2026-09-10 SweepCEquipItem2] 真 bug 修（条件极性反写）：ORIG 0x0811e301
    // `cmp %eax,%edx; jg 0811e31a`，且 Ghidra C 独立同证
    // `if (*(int *)(param_3 + 4) < (int)(uVar3 & 0xff)) { uVar4 = 0; } else { …主体… }`
    // ⇒ **sepLv > m_maxLevel 才 return false**；sepLv <= m_maxLevel 路径继续主体，
    // 并在主体前重发一次 GetUpgradeSeparate（0x0811e30e，结果丢弃，P48 无 CSE）。
    // 旧译文写成 `if (!(sepLv > m_maxLevel)) { 死调用; return false; }`，极性完全相反
    // （正常强化恒失败、超上限反而放行）。
    if (static_cast<int>(item->m_upgradeSep.GetUpgradeSeparate()) > table.m_maxLevel)
    {
        return false;
    }
    (void)item->m_upgradeSep.GetUpgradeSeparate();   // ORIG 0x0811e30e 死调用（结果丢弃）

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
// [2026-09-10 SweepCEquipItem2] 本轮修法（P243 求值序）：末检改「sepLv+1 > m_maxLevel」
// 书写序——ORIG 0x0811e29c `lea 0x1(%eax),%edx; mov 0x1c(%ebp),%eax; mov 0x4(%eax),%eax;
// cmp %eax,%edx; setg` 先算左操作数；旧写法 `m_maxLevel < sepLv+1` 使 4.4 提前把
// m_maxLevel 装 %ebx（多 1 对 mov）并反出 setl。改后 clean 132=132、助记符序列全等，
// 本件 DIFF→NEAR（TU 四数 near 1→2 / diff 7→6）；p167 10=10 CLEAR 不变。
#include "CItemUpgrade_Separate/_CheckCondition.inc"
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
    // ORIG 0x0811e29c：`lea 0x1(%eax),%edx; mov 0x1c(%ebp),%eax; mov 0x4(%eax),%eax;
    // cmp %eax,%edx; setg` ⇒ 先算 sepLv+1（左操作数）再读 m_maxLevel（P243 求值序）。
    if (static_cast<int>(item.m_upgradeSep.GetUpgradeSeparate()) + 1 > table.m_maxLevel)
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
// [2026-09-09 SweepCItemUpgrade] 复核（未改）：gap=3（orig 69 / our 66）即该 setcc-fold
// 段（ORIG 0x0811e508 `cmp; jae <false岛>; mov $1,%eax; jmp; mov $0,%eax; test; je`
// 6 条 vs 4.4 `cmp; setb %al; test; je` 3 条）。判词维持 N。
#include "CItemUpgrade_Separate/_DoProcUpgrade.inc"
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
        // [round5-C05 2026-09-10 C05④ 复验未命中，已回退] ORIG 0x0811e508
        // `cmp; jae <岛>; mov $1,%eax; jmp; mov $0,%eax; test %al,%al; je`（寄存器化
        // bool 赋值）vs 4.4 `cmp; setb %al; test; je`。按 C05④ 四步法步骤 2 试
        // **P225 原形**（body 以 `return false;` 终止 ⇒ 满足「无回边」前置）：
        // `while (a < b) { return false; }` 实测读数**逐值不变**（我们 66 / ORIG 69，
        // 仍 setb）⇒ 步骤 4 封顶 N-88/P197④（勿再动控制流、勿动 callee）。
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
#include "CItemUpgrade_Separate/_NoticeUpgrade.inc"
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
// [2026-09-09 SweepCItemUpgrade] 判词 N（ORIG 0x0811e66e，N7 + setcc-fold 族）：
// 本轮修法——① sepOld/sepNow 由 unsigned char 改 unsigned int（T1 直证：ORIG
// 0x0811e905 / 0x0811ea93 `movzbl %al,%eax; mov %eax,-0x24/-0x1c` 32 位存；uchar 恒
// 8 位存 + 回读 movzbl，实测多 3 条）；② 删 matSlot/slot 具名局部（P0：ORIG
// 0x0811e93d/0x0811e99e/0x0811eaae/0x0811ead5 每处重读 msg->m_materialSlot /
// m_targetSlot）；③ delete_item receiver 改链式右值（ORIG 0x0811e947 无槽往返）。
// gap 7→3（orig 393 / our 386→390），p167 43=43 CLEAR（0 missing/0 extra）。
// 残余：`cond != 0` 的 setne 物化 1 处（ORIG 0x0811e8e1 cmpl/setne/test/je，4.4 直跳）
// + 虚调目标 %eax vs %edx、rarity/itemType 的 %ebx/%esi 角色互换（N7，P230 判 4.4.6
// 后端敏感族）；帧 push edi/esi/ebx 与 ORIG 全等（sub 立即数差 0x10 = N115）。
// 按 §6.2 判语义完成、byte 层封顶。
// 虚槽核对（Main 2026-09-09 判据）：本 TU 共 5 处 `call *`，槽号逐处与 ORIG 一致——
// ① item_upgrade_system 槽 0（ORIG 0x080fc889 `mov (%eax),%edx`）；② _GetUpgradeInfo
// 0x892 `add $0xc`（ORIG 0x0811e354，CItem::GetItemType）；③④ _CheckCondition
// 0xa65/0xab9 `add $0xc`（ORIG 0x0811e1fc/0x0811e250）；⑤ 本函数 0x1238 `add $0x4c`
// = CItem::IsRandomOption（ORIG 0x0811ebdd）。无槽取错。
// [2026-09-10 SweepCEquipItem2] 本轮修法（P198① 预清两句式）：cond / success 由
// 初始化式改「声明=0/false + 独立赋值」——ORIG 0x0811e894 `movl $0x0,-0x28`、
// 0x0811e90b `movb $0x0,-0x1d` 均为 call 前预清、call 后回填，合写会丢两行。
// gap 3→1（fresh .o orig 393 / our 392），p167 43=43 CLEAR，四数零回退。
// 残余（N 封顶）：setne 物化 1 处、Inven_Item 槽基 -0xf6↔-0xb6（帧 +0x10 = N115）、
// historyLog 取址 `lea 0x79700(%edx),%ecx` vs `add $0x79700,%edx`（N107）、
// success 的 %esi↔%edi（N7）。
#include "CItemUpgrade_Separate/ProcUpgrade.inc"
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

    // ORIG 0x0811e894 `movl $0x0,-0x28(%ebp)` 先于 call、0x0811e8de 再回填 ⇒
    // 原码是「声明=0 + 独立赋值」两句式（P198①，合写会丢预清行）。
    int cond = 0;
    cond = _CheckCondition(user, item, citem, matItem, *table,
                           user->get_charac_level());
    // [round5-C05 2026-09-10] P225：ORIG 0x0811e8de `cmpl $0x0,-0x28; setne %al;
    // test %al,%al; je` 是 4.1 的「bool 物化后条件分支」；4.4 对 `if (cond != 0)`
    // 恒折直跳（`je`，少 2 条）⇒ 改 while+break 形逐条复现。
    while (cond != 0)
    {
        return cond;
    }

    // T1 直证：ORIG 0x0811e905 `movzbl %al,%eax; mov %eax,-0x24` ⇒ 32 位无符号局部
    // （uchar 局部会生 `mov %al,-N` + 回读 movzbl，实测多 3 条）。
    const unsigned int sepOld =
        static_cast<unsigned char>(item.m_upgradeSep.GetUpgradeSeparate());

    // ORIG 0x0811e90b `movb $0x0,-0x1d(%ebp)` 预清 + 0x0811e932 回填（P198①）。
    bool success = false;
    success = _DoProcUpgrade(user, &item, &info);

    // P0：ORIG 0x0811e93d/0x0811e99e 每处重读 `msg->m_materialSlot`（movzwl 0x23），
    // 无 matSlot 具名局部；receiver 亦为链式右值（0x0811e947 无槽往返）。
    if (user->getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, msg->m_materialSlot,
                                                info.m_materialCnt,
                                                static_cast<eItemDelReason>(0xc),
                                                true) == 0)
    {
        return 0x11;
    }

    packet->put_byte(1);
    packet->put_short(msg->m_materialSlot);

    // ORIG 0x0811e94a：第三次 getCurCharacInvenW（receiver 右值直落）。
    Inven_Item refreshed =
        user->getCurCharacInvenW()->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_materialSlot);
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

    // 同 sepOld：ORIG 0x0811ea93 `movzbl %al,%eax; mov %eax,-0x1c`。
    const unsigned int sepNow =
        static_cast<unsigned char>(item.m_upgradeSep.GetUpgradeSeparate());
    packet->put_byte(sepNow);

    // P0：ORIG 0x0811eaae/0x0811ead5 每处重读 `msg->m_targetSlot`（movzwl 0x1b）。
    packet->put_short(msg->m_targetSlot);

    if (item.m_addInfo != 0)   // Inven_Item +0x02
    {
        invenW->update_item(INVEN_TYPE_EQUIP, msg->m_targetSlot, item);
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
#include "CItemUpgrade_Separate/IncUpgradeSeparate.inc"
{
    // [b28d-solo-CItemUpgrade_Separate] AE：位域前置自增 `++m_bits.m_lo` 复现 ORIG
    // 0x0811ed32 的「截断掩码就地施于 %eax + 再掩码写回」形（探针 /tmp/b28d_51/p6.cpp
    // 形 b 逐条全等；`= m_lo + 1` 与 `+= 1` 恒多一条 `mov %eax,%edx` 中转）。
    ++m_bits.m_lo;
}

// ============================ CUser 公会公告 ============================

// ORIG T 0x0867a242：成功且新等级>5 用 game_server_msg_248；失败且旧等级>5 用
// game_server_msg_249；前缀 game_server_msg_247 取 10 字节；长度<120 才发公会板。
// [2026-09-09 SweepCItemUpgrade] 判词 N（ORIG 0x0867a242，4.1 EH bool 归一化族）：
// 本轮修法——① 删 prefixStr 具名局部（P0：ORIG 0x0867a299→0x0867a2b7 findString 结果
// 经 %eax 直传 strncpy 第 2 参，无槽往返）；② boardReady 改裸 `register bool`（P231：
// ORIG 全程 %ebx 承载无栈槽，4.4 由 movb×3+cmpb 槽往返改寄存器保活）。p167 71=71
// CLEAR（0 missing/0 extra）。残余（gap=29，orig 512 / our 483）：ORIG 在 4 个 string
// 析构点各发 6 条 bool 归一化块（`test %ebx,%ebx; jne; mov $0,%ebx; jmp; mov $1,%ebx`，
// 0x0867a556 / 0x0867a588 / 0x0867a872 / 0x0867a8a4，共 24 条）——4.1 对 bool 局部在
// EH cleanup 点的规范化，4.4 不生成，跨版本不可复现；余下为槽基常数差
// （board -0xb8 ↔ -0xc4、帧 0x2dc ↔ 0x2fc = N115 outgoing 预留）。按 §6.2 判语义完成。
// [2026-09-10 SweepCEquipItem2] 本轮修法（P0/P52 + P198① + P222）：
// ① 删 guildkey/characNo/proxy 三个具名局部改单条链式表达式（ORIG 0x0867a8ec/
//    0x0867a8f9 把两 getter 结果留 %esi/%ebx 跨 GetServerProxy 保活、无槽往返）；
// ② boardReady 去初始化式改「未初始化声明 + if/else 双赋值」（ORIG 0x0867a4fa/
//    0x0867a52f/0x0867a816/0x0867a84b 两臂各 mov $0/$1,%ebx、函数头无预置）；
// ③ sprintf 分支改先写 upgrade != 0（ORIG 0x0867a455 `je` 落 "%s %d" 臂）。
// 改后帧 0x2ec→0x2dc（与 ORIG 全等）、board/prefix/msgBuf/tail 槽位与 ORIG 对齐
// （msgBuf 差 1 字节）、tail 清零慢路逐条相同；p167 71=71 CLEAR（仅 C1/C2 同址别名伪差）。
// 残余（gap=32，orig 512 / our 481，N 封顶）：3 个 bool EH 归一化块 15 条 + 尾 test 2 条
// （4.1 独有）、`seta`↔`setbe`（setcc-fold）、success 的 `xor $1` 物化 3 条、msgBuf 槽
// 差 1 字节（2 条 lea）、boardReady 末尾 `test %ebx`↔`test %bl`。
#include "CItemUpgrade_Separate/makeGuildItemSeparateUpgradMessage.inc"
                                               int newLevel, bool success, int upgrade)
{
    // ORIG 0x0867a257/0x0867a26d：board[120] rep stos 先、prefix 三条 movl/movl/movw
    // 后（声明序 board 在前；显式 memset 会多发 4 个 call，p167 EXTRA 实证）。
    char board[120] = {0};
    char prefix[10] = {0};

    // P0：ORIG 0x0867a299→0x0867a2b7 findString 结果经 %eax 直传 strncpy 第 2 参，
    // 无槽往返 ⇒ 原码无具名局部，实参位内联。
    std::strncpy(prefix,
                 g_scriptStringManager_.findString(4, "game_server_msg_247",
                                                   static_cast<bool*>(0)),
                 sizeof(prefix));

    // [2026-09-10 SweepCEquipItem2] P0/P231：ORIG 0x0867a4fa/0x0867a52f（else 臂
    // 0x0867a816/0x0867a84b）两臂各自 `mov $0/$1,%ebx`，函数头**无**预置 0 ⇒ 原码是
    // 未初始化声明 + if/else 双赋值（写 `= false` 会被 4.4 多生成 `mov $0,%esi`）。
    register bool boardReady;
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
        // ORIG 0x0867a455 `cmpl $0; je <"%s %d">`：fall-through 是 "+…" 臂
        // ⇒ 原码先写 upgrade != 0（P222 块序）。
        if (upgrade != 0)
        {
            std::sprintf(tail, "+%d %s %d", upgrade, name, newLevel);
        }
        else
        {
            std::sprintf(tail, "%s %d", name, newLevel);
        }
        line += tail;
        line += body;
        userName += line;

        if (sizeof(board) > userName.length())
        {
            std::memcpy(board, userName.c_str(), userName.length());
            boardReady = true;
        }
        else
        {
            boardReady = false;
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
        if (upgrade != 0)
        {
            std::sprintf(tail, "+%d %s %d", upgrade, name, oldLevel + 1);
        }
        else
        {
            std::sprintf(tail, "%s %d", name, oldLevel + 1);
        }
        line += tail;
        line += body;
        userName += line;

        if (sizeof(board) > userName.length())
        {
            std::memcpy(board, userName.c_str(), userName.length());
            boardReady = true;
        }
        else
        {
            boardReady = false;
        }
    }

    if (boardReady)
    {
        // [2026-09-10 SweepCEquipItem2] P0/P52：ORIG 0x0867a8ec/0x0867a8f9 把
        // get_charac_guildkey / getCurCharacNo 结果分别留 %esi/%ebx 跨 GetServerProxy
        // 保活、全程无栈槽往返 ⇒ 原码是单条链式表达式（实参内联），不是三个具名局部。
        GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup())
            ->SendGuildBoardWrite(
                board, 0u, static_cast<unsigned int>(getCurCharacNo()), -1,
                static_cast<char>(-1),
                static_cast<unsigned int>(get_charac_guildkey()));
    }
}
