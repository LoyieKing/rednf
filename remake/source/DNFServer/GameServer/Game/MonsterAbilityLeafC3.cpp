// ============================================================================
// df_game_r Game/ 怪物能力升级域 —— 独占 TU（MonsterAbilityLeafC3）。
//
// 恢复范围（全部为 ORIG 真实符号，nm/objdump 实证；无函数级报告，
// 依据规则先读同类报告 CBattle_Field/set_monster_ability.md、调用方
// BattleMonsterDepsC2.cpp / BattleDepsB2.cpp 后逐指令转写）：
//   - upgradeAbilityByBossFeature            T 0x88b7c24（实为空函数）
//   - upgradeAbilityByChampionFeature        T 0x88b7c29
//   - upgradeAbilityByMonsterParameterCategory T 0x88b7f36
//   - upgradeAbilityByDifficultyOfDungeon    T 0x88b813a
//   - makeChampionFeatureList                T 0x88ba0fd
//   - CRidable::SetRidableObject(RidableScript&) T 0x82fe6d0（报告已完整阅读：
//     docs/class_func_reports/CRidable/SetRidableObject.md ## 汇编+## 反编译 C）
// 及其依赖链（同为 ORIG 符号）：
//   - haveChampionFeature                    T 0x8891ad4
//   - getToleranceActiveStatus               T 0x8891b2a
//   - randomSetChampionFeature               T 0x88b9dcf
//   - scriptSetChampionFeature               T 0x88b9f12
//   - addChampionFeature                     T 0x88b9f66
//   - STMonsterParameterCategory::getValueInt/getValueFloat W 0x88ba480/0x88ba4a2
//   - DungeonRidableObject::operator=(RidableObject)        W 0x830e9d8
// ============================================================================

#include <algorithm>
#include <cstdlib>
#include <map>
#include <utility>
#include <vector>

#include "ActiveStaticInfo.h"
#include "CDungeon.h"     // RidableObject / RidableScript 完整类型
#include "CRidable.h"
#include "STMonsterScript.h"

// ---------------------------------------------------------------------------
// ORIG 枚举类型名仅用于 ABI（mangled 中 17CHAMPION_FEATURES /
// 19ENUM_ACTIVEPROPERTY）；具体枚举子命名未还原【推断】，调用点一律显式转换。
// ---------------------------------------------------------------------------
enum CHAMPION_FEATURES
{
    CHAMPION_FEATURES_MIN = 0
};
enum ENUM_ACTIVEPROPERTY
{
    ENUM_ACTIVEPROPERTY_MIN = 0
};

// ORIG .data D 0x93f4a00[0x120]（72 float）；定义在 InitScriptProducers_4.cpp
// （float[13][5]），运行期由脚本导入填充。此处按声明类型引用并做扁平索引。
extern float DIFFICULTY_OF_DUNGEON_BONUS[13][5];

namespace
{
// 冠军特性随机池：ORIG .rodata 0x8e08000 共 37 项；
// 低难度分支取 rand()%28，高难度(diff>3)分支取 [28,35)。
const int kChampionFeaturePool[37] = {
    0x0001ff00, 0x0000ff01, 0x0002ff02, 0x0002ff03,  // 0..3
    0x00030004, 0x00030005, 0x00030006, 0x00030007,  // 4..7
    0x00040008, 0x0003fe09, 0x0003fe0a, 0x0003fe0b,  // 8..11
    0x0003fe0c, 0x0002000d, 0x0003000e, 0x0003000f,  // 12..15
    0x00020010, 0x00040011, 0x00010012, 0x00020013,  // 16..19
    0x0002fd14, 0x0002fd15, 0x0002fd16, 0x0002fd17,  // 20..23
    0x0002fd18, 0x0002fd19, 0x0002fd1a, 0x0003001b,  // 24..27
    0x0004fd1c, 0x0004fd1d, 0x0004fd1e, 0x0004fd1f,  // 28..31
    0x0004fd20, 0x0004fd21, 0x0004fd22, 0x0004fd23,  // 32..35
    0x0004fd24                                       // 36
};

// 特性 tier→点数开销表：ORIG .rodata 0x8e08094[0x400]，按 (feature>>16)&0xff 索引。
const int kChampionFeatureTierCost[256] = {
    0x0000000a, 0x00000008, 0x00000005, 0x00000003, 0x00000002, 0x00000002, 0x0000000c, 0x00000029,
    0x00000021, 0x00000021, 0x00000006, 0x0001869f, 0x00000004, 0x00000003, 0x00000027, 0x000000b4,
    0x00000002, 0x0000000a, 0x00000005, 0x00000014, 0x00000006, 0x00000009, 0x00124f80, 0x000000b4,
    0x00000004, 0x00000006, 0x00000007, 0x00000031, 0x00000008, 0x00000009, 0x00000003, 0x44a54a80,
    0x000003e8, 0x00000005, 0x00000005, 0x00000050, 0x00000096, 0x000000ff, 0x00000010, 0x000000c8,
    0x000000d1, 0x00000003, 0x0000000a, 0x0000003c, 0x00000002, 0x0098967f, 0x0000001d, 0x3b9aca00,
    0x00002710, 0x00989680, 0x000003e8, 0x64050a11, 0x00030ac9, 0x7fffffff, 0x00000258, 0x641e0164,
    0x00000064, 0x0000001e, 0x000003e8, 0x000006a4, 0x00001220, 0x00001220, 0x000003e8, 0x000007d0,
    0x00000a8c, 0x00001388, 0x00001220, 0x00070a1e, 0x46a7b9d0, 0x46d5de50, 0x46e1bbd0, 0x47686d10,
    0x479fcc10, 0x47b208d0, 0x478e7050, 0x000249f0, 0x0007a120, 0x000f4240, 0x000000c8, 0x000000c8,
    0x0001869f, 0x00000004, 0x00000007, 0x00000000, 0x00000040, 0x00000000, 0x00002710, 0x00640064,
    0x00090014, 0x000a0028, 0x0014000a, 0x00640008, 0x00020001, 0x00000005, 0x030a0504, 0x00000032,
    0x000000b4, 0x00000002, 0x00000008, 0x00000004, 0x00000028, 0x00000002, 0x00000004, 0x00000002,
    0x00000007, 0x00000064, 0x0000000a, 0x0000000a, 0x00000080, 0x00000010, 0x0000ea60, 0x00000001,
    0x0000001e, 0x000004b0, 0x00001388, 0x0000270f, 0x00000003, 0x00000008, 0x00000078, 0x00000032,
    0x00000001, 0x00007805, 0x00000080, 0x00000064, 0x0000000f, 0x00000046, 0x000000c8, 0x00000078,
    0x0000003c, 0x00000078, 0x0000005a, 0x0000000f, 0x00000032, 0x00000008, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000040f, 0x00000410, 0x00000411, 0x00000412, 0x00000413,
    0x00000414, 0x00000415, 0x00000416, 0x00000009, 0x00000012, 0x00000014, 0x0000f68a, 0x00000014,
    0x00000002, 0x00000013, 0x0000003b, 0x0000002d, 0xffffffff, 0x00000002, 0x0000000b, 0x00000014,
    0x0000000a, 0x00000003, 0x0000001e, 0x0098967f, 0x00000037, 0x00001388, 0x000000dc, 0x0000003c,
    0x00003390, 0x00000005, 0x00000014, 0x00000032, 0x000000ff, 0x00000096, 0x00000046, 0x00000001,
    0x000000c8, 0x00000000, 0x00000000, 0x00000003, 0x00000002, 0x00000000, 0x00000003, 0x00000004,
    0x00000002, 0x00000000, 0x00000002, 0x00000003, 0x00000002, 0x00000001, 0x00000001, 0x00000003,
    0x00000000, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0x00000001, 0x00000003,
    0x00000004, 0x00000002, 0x00000000, 0x00000003, 0x00000001, 0x00000001, 0x00000003, 0x00000000,
    0x00000001, 0x00000001, 0x00000001, 0x00000002, 0x00000001, 0x00000001, 0x00000002, 0x00000000,
    0x00000002, 0x00000003, 0x00000002, 0x00000000, 0x00000000, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000005, 0x00000003, 0x00000032, 0x00989763, 0x000186a0, 0x00000004,
    0x00000003, 0x00000004, 0x00000000, 0x00000014, 0x00000004, 0x00000000, 0x00000001, 0x00000002,
    0x00000003, 0x08100810, 0x0000001b, 0x00000046, 0x00000006, 0x0000000c, 0x00000006, 0x00000000
};

// 难度加成的公共乘算形态：P = old * BONUS[...]（fstps 中转），再 trunc(row * P)。
int scaleByBonus(float oldValue, float bonus, float tableValue)
{
    const float scaled = oldValue * bonus;
    return static_cast<int>(tableValue * scaled);
}
}  // namespace

// ---------------------------------------------------------------------------
// haveChampionFeature — ORIG 0x8891ad4：线性查找。
// ---------------------------------------------------------------------------
bool haveChampionFeature(std::vector<int>* features, CHAMPION_FEATURES feature)
{
    for (std::vector<int>::size_type i = 0; i < features->size(); ++i)
    {
        if ((*features)[i] == static_cast<int>(feature))
            return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// getToleranceActiveStatus — ORIG 0x8891b2a：跳转表 0x8e040f4（17 项）。
// ---------------------------------------------------------------------------
int getToleranceActiveStatus(ENUM_ACTIVEPROPERTY prop)
{
    switch (static_cast<int>(prop))
    {
    case 0: return 0x00;
    case 1: return 0x01;
    case 2: return 0x02;
    case 3: return 0x03;
    case 4: return 0x04;
    case 5: return 0x05;
    case 6: return 0x06;
    case 7: return 0x07;
    case 8: return 0x08;
    case 9: return 0x09;
    case 10: return 0x0a;
    case 11: return 0x0b;
    case 12: return 0x12;   // 调用方以此值跳过该属性
    case 13: return 0x12;
    case 14: return 0x0f;
    case 15: return 0x10;
    case 16: return 0x11;
    default: return 0x12;
    }
}

// ---------------------------------------------------------------------------
// addChampionFeature — ORIG 0x88b9f66：同组(fc/ fd/fe/ff)冲突拒绝 → 重复拒绝 →
// 点数足够才 push_back 并扣点。
// ---------------------------------------------------------------------------
void addChampionFeature(CHAMPION_FEATURES feature, int& count,
                        std::vector<int>& list)
{
    const int f = static_cast<int>(feature);
    const int fGroup = (f & 0xff00) >> 8;

    for (std::vector<int>::size_type i = 0; i < list.size(); ++i)
    {
        const int group = (list[i] & 0xff00) >> 8;
        if ((group == 0xff && fGroup == 0xff) ||
            (group == 0xfe && fGroup == 0xfe) ||
            (group == 0xfd && fGroup == 0xfd) ||
            (group == 0xfc && fGroup == 0xfc))
            return;
        if (list[i] == f)
            return;
    }

    const int cost = kChampionFeatureTierCost[(f >> 16) & 0xff];
    if (count < cost)
        return;
    list.push_back(f);
    count -= cost;
}

// ---------------------------------------------------------------------------
// randomSetChampionFeature — ORIG 0x88b9dcf：srand(seed)；
// diff>3 直接取池[28+rand()%7]；否则 3700 次尝试内按过滤规则随机添加。
// 过滤：>0x30007 时仅 [0x3fe09,0x3fe0c] 与 0x40008 跳过；≤0x30007 且 ≥0x30004
// 跳过；等于 0x1ff00/0x20013/0xff01 跳过；其余添加。count<=1 提前终止。
// ---------------------------------------------------------------------------
void randomSetChampionFeature(int seed, int& count, std::vector<int>& temp,
                              int diff)
{
    std::srand(static_cast<unsigned int>(seed));

    if (diff > 3)
    {
        const int f = kChampionFeaturePool[rand() % 7 + 28];
        addChampionFeature(static_cast<CHAMPION_FEATURES>(f), count, temp);
        return;
    }

    for (int remain = 0xe74; remain > 0; --remain)
    {
        if (count <= 1)
            break;

        const int f = kChampionFeaturePool[rand() % 28];
        bool add;
        if (f > 0x30007)
        {
            if (f < 0x3fe09)
                add = true;
            else if (f <= 0x3fe0c)
                add = false;
            else if (f == 0x40008)
                add = false;
            else
                add = true;
        }
        else if (f >= 0x30004)
        {
            add = false;
        }
        else
        {
            add = !(f == 0x1ff00 || f == 0x20013 || f == 0xff01);
        }

        if (add)
            addChampionFeature(static_cast<CHAMPION_FEATURES>(f), count, temp);
    }
}

// ---------------------------------------------------------------------------
// scriptSetChampionFeature — ORIG 0x88b9f12：对脚本列表逐项 addChampionFeature。
// ---------------------------------------------------------------------------
void scriptSetChampionFeature(std::vector<int>& scriptList, int& count,
                              std::vector<int>& temp)
{
    for (std::vector<int>::size_type i = 0; i < scriptList.size(); ++i)
        addChampionFeature(static_cast<CHAMPION_FEATURES>(scriptList[i]),
                           count, temp);
}

// ---------------------------------------------------------------------------
// makeChampionFeatureList — ORIG 0x88ba0fd。
// count = trunc((level-1)*0.2f + 2.0f)（常量 .rodata 0x8e084b8=0.2f /
// 0x8e084a0=2.0f）；脚本列表为空走随机池，否则按脚本逐项登记。
void makeChampionFeatureList(std::vector<int>& features, int level, int seed,
                             std::vector<int>& temp, int diff)
{
    int count =
        static_cast<int>(static_cast<float>(level - 1) * 0.2f + 2.0f);

    if (features.empty())
        randomSetChampionFeature(seed, count, temp, diff);
    else
        scriptSetChampionFeature(features, count, temp);
}

// ---------------------------------------------------------------------------
// upgradeAbilityByBossFeature — ORIG 0x88b7c24 仅 push ebp/mov/pop/ret，
// 即空函数（链接边界保留）。
// ---------------------------------------------------------------------------
void upgradeAbilityByBossFeature(ActiveStaticInfo* /*info*/)
{
}

// ---------------------------------------------------------------------------
// upgradeAbilityByChampionFeature — ORIG 0x88b7c29。
// 角色系数表为函数内静态初值（ORIG 栈上立即数构造，与 ORIG 数值一致）：
//   elem 0..3：elemAttack[e] += (150-old)*ratioA[role]
//   elem == 4：attack/magicAttack *= ratioB[role]
//   公共：hpBase *= ratioC[role]；随后按特性调整 statBase*；末尾 attr17Base[13]+=50。
// 偏移实证：m_elemAttack@+0x38、m_attack@+0x18、m_magicAttack@+0x20、
// m_hpBase@+0x04、statBase3/4/6@+0x118/+0x120/+0x130、statBase11..14@
// +0x158/+0x160/+0x168/+0x170、attr17Base[13]=+0xac（ActiveStaticInfo.h 布局）。
// ---------------------------------------------------------------------------
void upgradeAbilityByChampionFeature(int elem, int roleType,
                                     std::vector<int>* features,
                                     ActiveStaticInfo* info)
{
    static const float kElemRatio[4] = {0.0f, 0.3f, 0.5f, 0.0f};
    static const float kAttackRatio[4] = {0.0f, 1.5f, 2.0f, 0.0f};
    static const float kHpRatio[4] = {0.0f, 1.5f, 3.0f, 0.0f};

    if (elem >= 0 && elem <= 3)
    {
        const int oldValue = info->m_elemAttack[elem];
        info->m_elemAttack[elem] = static_cast<int>(
            static_cast<float>(oldValue) +
            (150 - oldValue) * kElemRatio[roleType]);
    }
    else if (elem == 4)
    {
        info->m_attack =
            static_cast<int>(static_cast<float>(info->m_attack) *
                             kAttackRatio[roleType]);
        info->m_magicAttack =
            static_cast<int>(static_cast<float>(info->m_magicAttack) *
                             kAttackRatio[roleType]);
    }

    info->m_hpBase = static_cast<int>(static_cast<float>(info->m_hpBase) *
                                      kHpRatio[roleType]);

    if (haveChampionFeature(features, static_cast<CHAMPION_FEATURES>(0x2000d)))
        info->m_statBase3 =
            static_cast<int>(static_cast<float>(info->m_statBase3) * 1.5f);
    if (haveChampionFeature(features, static_cast<CHAMPION_FEATURES>(0x3000e)))
        info->m_statBase4 = static_cast<int>(
            static_cast<float>(info->m_statBase4) +
            static_cast<float>(info->m_statBase4));  // fadd st,st
    if (haveChampionFeature(features, static_cast<CHAMPION_FEATURES>(0x2ff02)))
        info->m_statBase12 =
            static_cast<int>(static_cast<float>(info->m_statBase12) * 2.5f);
    if (haveChampionFeature(features, static_cast<CHAMPION_FEATURES>(0x2ff03)))
        info->m_statBase14 =
            static_cast<int>(static_cast<float>(info->m_statBase14) * 2.5f);
    if (haveChampionFeature(features, static_cast<CHAMPION_FEATURES>(0x3000f)))
        info->m_statBase6 = static_cast<int>(
            static_cast<float>(info->m_statBase6) +
            static_cast<float>(info->m_statBase6));  // fadd st,st

    info->m_attr17Base[13] += 50;                    // +0xac
}

// ---------------------------------------------------------------------------
// STMonsterParameterCategory 访问器 — ORIG W 0x88ba480 / 0x88ba4a2。
// ---------------------------------------------------------------------------
int STMonsterParameterCategory::getValueInt() const
{
    return m_field8 / 100;
}

float STMonsterParameterCategory::getValueFloat() const
{
    return static_cast<float>(m_field8) / 100.0f;
}

// ---------------------------------------------------------------------------
// upgradeAbilityByMonsterParameterCategory — ORIG 0x88b7f36。
// 先扫描是否存在 id==13（hasCat13）；再逐条目：解析目标字段后——
//   hasCat13&&isChampion&&id==10 跳过；hasCat13&&!isChampion&&id==13 跳过；
//   m_field4!=0 → 目标 = trunc(target_f * getValueFloat())；
//   否则 → 目标 += getValueInt()（即 m_field8/100）。
// ---------------------------------------------------------------------------
void upgradeAbilityByMonsterParameterCategory(
    const std::vector<STMonsterParameterCategory>* categories,
    ActiveStaticInfo* info, bool isChampion)
{
    bool hasCat13 = false;
    for (std::vector<STMonsterParameterCategory>::size_type i = 0;
         i < categories->size(); ++i)
    {
        if ((*categories)[i].m_field0 == 13)
        {
            hasCat13 = true;
            break;
        }
    }

    for (std::vector<STMonsterParameterCategory>::size_type j = 0;
         j < categories->size(); ++j)
    {
        const STMonsterParameterCategory& cat = (*categories)[j];
        // P167：ORIG 为内联 switch（无需独立 helper，§4.1），字段映射直接内联。
        int* target = 0;
        switch (cat.m_field0)
        {
        case 0:  target = &info->m_statBase4;       break; // +0x120
        case 1:  target = &info->m_statBase3;       break; // +0x118
        case 2:  target = &info->m_attack;          break; // +0x18
        case 3:  target = &info->m_magicAttack;     break; // +0x20
        case 4:  target = &info->m_defense;         break; // +0x28
        case 5:  target = &info->m_magicDefense;    break; // +0x30
        case 6:  target = &info->m_statBase11;      break; // +0x158
        case 7:  target = &info->m_statBase12;      break; // +0x160
        case 8:  target = &info->m_statBase13;      break; // +0x168
        case 9:  target = &info->m_statBase14;      break; // +0x170
        case 10: target = &info->m_hpBase;          break; // +0x04
        case 11: target = &info->m_statBase6;       break; // +0x130
        case 12: target = &info->m_attr17Base[13];  break; // +0xac
        case 13: target = &info->m_hpBase;          break; // +0x04
        default: target = 0;                         break;
        }
        if (target == 0)
            continue;
        if (hasCat13 && isChampion && cat.m_field0 == 10)
            continue;
        if (hasCat13 && !isChampion && cat.m_field0 == 13)
            continue;

        if (cat.m_field4 != 0)
            *target = static_cast<int>(static_cast<float>(*target) *
                                       cat.getValueFloat());
        else
            *target += cat.getValueInt();
    }
}

// ---------------------------------------------------------------------------
// upgradeAbilityByDifficultyOfDungeon — ORIG 0x88b813a（4 参版本；level 参数
// 在本重载中未使用）。table 为副本难度表，行访问除 +0x118 一处使用 operator[]
// 外均为 at()（保留 ORIG 的越界抛出语义）；BONUS 扁平索引见各注释。
// ---------------------------------------------------------------------------
void upgradeAbilityByDifficultyOfDungeon(
    int diff, ActiveStaticInfo* info, int level,
    const std::vector<std::vector<float> >& table)
{
    (void)level;  // ORIG 本体未读取 0x10(%ebp)
    if (table.empty())
        return;

    const float* bonus = &DIFFICULTY_OF_DUNGEON_BONUS[0][0];

    // +0x04 m_hpBase = trunc(hpBase*BONUS[d+0x14]*T[4][d])
    info->m_hpBase = scaleByBonus(static_cast<float>(info->m_hpBase),
                                  bonus[diff + 0x14],
                                  table.at(4).at(diff));
    // +0xac attr17Base[13] +=
    info->m_attr17Base[13] +=
        scaleByBonus(static_cast<float>(info->m_attr17Base[13]),
                     bonus[diff + 0x0f], table.at(3).at(diff));
    // +0x130 statBase6 +=
    info->m_statBase6 +=
        scaleByBonus(static_cast<float>(info->m_statBase6),
                     bonus[diff + 0x32], table.at(10).at(diff));
    // +0x78+i*4 attr17Base[i] +=（i=0..16；i==12/13 因容错映射 0x12 被跳过）
    for (int i = 0; i <= 0x10; ++i)
    {
        if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) ==
            0x12)
            continue;
        info->m_attr17Base[i] +=
            scaleByBonus(static_cast<float>(info->m_attr17Base[i]),
                         bonus[diff + 0x23], table.at(7).at(diff));
    }
    // +0x120 statBase4 =
    info->m_statBase4 = scaleByBonus(static_cast<float>(info->m_statBase4),
                                     bonus[diff + 0x19],
                                     table.at(5).at(diff));
    // +0x118 statBase3 += （非零守卫；此行 ORIG 用 ix/ix 无检查访问）
    if (info->m_statBase3 != 0)
    {
        info->m_statBase3 +=
            scaleByBonus(static_cast<float>(info->m_statBase3),
                         bonus[diff + 0x1e], table[6][diff]);
    }
    // +0x158 statBase11 = / +0x168 statBase13 =（同一行 T[8]、同一 BONUS 槽）
    info->m_statBase11 = scaleByBonus(static_cast<float>(info->m_statBase11),
                                      bonus[diff + 0x28],
                                      table.at(8).at(diff));
    info->m_statBase13 = scaleByBonus(static_cast<float>(info->m_statBase13),
                                      bonus[diff + 0x28],
                                      table.at(8).at(diff));
    // +0x160 statBase12 = / +0x170 statBase14 =（同一行 T[9]、同一 BONUS 槽）
    info->m_statBase12 = scaleByBonus(static_cast<float>(info->m_statBase12),
                                      bonus[diff + 0x2d],
                                      table.at(9).at(diff));
    info->m_statBase14 = scaleByBonus(static_cast<float>(info->m_statBase14),
                                      bonus[diff + 0x2d],
                                      table.at(9).at(diff));
}

// ---------------------------------------------------------------------------
// upgradeAbilityByDifficultyOfDungeon — ORIG 0x88b8c37（2 参版本）：
// 仅按 DIFFICULTY_OF_DUNGEON_BONUS 全局表（扁平 [d+off]）提升，无行乘。
// ---------------------------------------------------------------------------
void upgradeAbilityByDifficultyOfDungeon(int diff, ActiveStaticInfo* info)
{
    // ORIG 0x88b8c37：全局表直接索引（无 bonus 指针变量）。
    // +0x04 m_hpBase = trunc(hpBase*BONUS[d+0x14])
    info->m_hpBase = static_cast<int>(
        static_cast<float>(info->m_hpBase) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x14]);
    // +0xac attr17Base[13] += BONUS[d+0x0f]
    info->m_attr17Base[13] += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x0f]);
    // +0x130 statBase6 += BONUS[d+0x32]
    info->m_statBase6 += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x32]);
    // +0x78+i*4 attr17Base[i] +=（i=0..16；容错映射 0x12 跳过；
    // ORIG 0x88b8c37 用 setne+je 形式，即 if (x != 0x12) 正写）
    for (int i = 0; i <= 0x10; ++i)
    {
        if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) != 0x12)
            info->m_attr17Base[i] +=
                static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x23]);
    }
    // +0x120 statBase4 = trunc(statBase4*BONUS[d+0x19])
    info->m_statBase4 = static_cast<int>(
        static_cast<float>(info->m_statBase4) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x19]);
    // +0x118 statBase3 += （非零守卫）
    if (info->m_statBase3 != 0)
        info->m_statBase3 += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x1e]);
    // +0x158/+0x168 statBase11/13 = trunc(*BONUS[d+0x28])
    info->m_statBase11 = static_cast<int>(
        static_cast<float>(info->m_statBase11) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28]);
    info->m_statBase13 = static_cast<int>(
        static_cast<float>(info->m_statBase13) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28]);
    // +0x160/+0x170 statBase12/14 = trunc(*BONUS[d+0x2d])
    info->m_statBase12 = static_cast<int>(
        static_cast<float>(info->m_statBase12) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d]);
    info->m_statBase14 = static_cast<int>(
        static_cast<float>(info->m_statBase14) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d]);
}

// ---------------------------------------------------------------------------
// upgradeAbilityByDifficultyOfDungeon — ORIG 0x88b85cf（3 参版本）：
// MONSTER_DIFFICULTY_BONUS[diff] 行表 size>=level 时按行乘提升，否则同 2 参版。
// ---------------------------------------------------------------------------
extern std::vector<std::vector<float> > MONSTER_DIFFICULTY_BONUS[5];

void upgradeAbilityByDifficultyOfDungeon(int diff, ActiveStaticInfo* info,
                                         int level)
{
    // ORIG 0x88b85cf：全局表直接索引（无 bonus 指针变量）。
    if (MONSTER_DIFFICULTY_BONUS[diff].size() >= static_cast<unsigned int>(level))
    {
        std::vector<float>& row =
            MONSTER_DIFFICULTY_BONUS[diff].at(level - 1);   // ORIG 非 const at

        info->m_hpBase = static_cast<int>(
            static_cast<float>(info->m_hpBase) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x14] * row.at(4));
        info->m_attr17Base[13] += static_cast<int>(
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x0f] * row.at(3));
        info->m_statBase6 += static_cast<int>(
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x32] * row.at(10));
        for (int i = 0; i <= 0x10; ++i)
        {
            if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) != 0x12)
                info->m_attr17Base[i] += static_cast<int>(
                    DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x23] * row.at(7));
        }
        info->m_statBase4 = static_cast<int>(
            static_cast<float>(info->m_statBase4) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x19] * row.at(5));
        if (info->m_statBase3 != 0)
            info->m_statBase3 += static_cast<int>(
                DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x1e] * row.at(6));
        info->m_statBase11 = static_cast<int>(
            static_cast<float>(info->m_statBase11) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28] * row.at(8));
        info->m_statBase13 = static_cast<int>(
            static_cast<float>(info->m_statBase13) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28] * row.at(8));
        info->m_statBase12 = static_cast<int>(
            static_cast<float>(info->m_statBase12) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d] * row.at(9));
        info->m_statBase14 = static_cast<int>(
            static_cast<float>(info->m_statBase14) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d] * row.at(9));
        return;
    }

    // ORIG 0x88b85cf：size<level 时 ORIG 内联展开 2 参版逻辑（非调用）。
    info->m_hpBase = static_cast<int>(
        static_cast<float>(info->m_hpBase) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x14]);
    info->m_attr17Base[13] += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x0f]);
    info->m_statBase6 += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x32]);
    for (int i = 0; i <= 0x10; ++i)
    {
        if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) != 0x12)
            info->m_attr17Base[i] +=
                static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x23]);
    }
    info->m_statBase4 = static_cast<int>(
        static_cast<float>(info->m_statBase4) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x19]);
    if (info->m_statBase3 != 0)
        info->m_statBase3 += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x1e]);
    info->m_statBase11 = static_cast<int>(
        static_cast<float>(info->m_statBase11) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28]);
    info->m_statBase13 = static_cast<int>(
        static_cast<float>(info->m_statBase13) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28]);
    info->m_statBase12 = static_cast<int>(
        static_cast<float>(info->m_statBase12) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d]);
    info->m_statBase14 = static_cast<int>(
        static_cast<float>(info->m_statBase14) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d]);
}

// 逐双字拷贝前 5 个字段（+0x14 m_field18 与 state 均不动）。
// ---------------------------------------------------------------------------
DungeonRidableObject& DungeonRidableObject::operator=(RidableObject other)
{
    m_x = other.m_x;
    m_y = other.m_y;
    m_field0c = other.m_field0c;
    m_field10 = other.m_field10;
    m_field14 = other.m_field14;
    return *this;
}

// ---------------------------------------------------------------------------
// CRidable::SetRidableObject — ORIG 0x82fe6d0（报告 SetRidableObject.md 全文）。
// script.count<=0 直接返回；写 +0x18 字节=script.m_4、+0x1c=script.m_0；
// count < 对象数：复制对象池，每次 random_shuffle 后取 back() 弹出插入
// map（key=第 3 字段）；否则顺序插入全部对象（注意循环条件是 size 而非 count）。
// state 统一置 4（ORIG 在 operator= 之后单独写 +0x18=4）。
// ---------------------------------------------------------------------------
void CRidable::SetRidableObject(RidableScript& script)
{
    if (script.m_0 <= 0)
        return;

    m_flag18 = static_cast<unsigned char>(script.m_4);
    m_count1c = script.m_0;

    const int count = script.m_0;
    const std::vector<RidableObject>::size_type objectCount =
        script.m_objects.size();

    if (count < static_cast<int>(objectCount))
    {
        std::vector<RidableObject> pool(script.m_objects);
        for (int i = 0; i < count; ++i)
        {
            std::random_shuffle(pool.begin(), pool.end());
            DungeonRidableObject obj;
            obj = pool.back();
            pool.pop_back();
            obj.m_state = 4;
            m_objects.insert(std::make_pair(obj.m_field0c, obj));
        }
    }
    else
    {
        for (std::vector<RidableObject>::size_type i = 0; i < objectCount; ++i)
        {
            DungeonRidableObject obj;
            obj = script.m_objects[i];
            obj.m_state = 4;
            m_objects.insert(std::make_pair(obj.m_field0c, obj));
        }
    }
}

// ===========================================================================
// OnlyOrigBatch12：upgradeAbilityBloodSystem / upgradeAbilityByParty /
// upgradeAbilityDeathTower（ORIG T 0x088b917b / 0x088b8e88 / 0x088b93bf）。
// 表：BLOOD_SYSTEM_BONUS（0x93f4b20，72 float）/ DEATH_TOWER_BONUS（0x93f4c40，
// 52 float）定义于 InitScriptProducers_4.cpp；DUNGEON_PARTY_BALANCE_TABLE
// （0x9494f5c）同。ActiveStaticInfo 槽位按反汇编偏移映射（+0x1c 起 17 槽
// 位型访问，同 setBasicAbilityByLevel 先例）。
// ===========================================================================
extern float BLOOD_SYSTEM_BONUS[13][5];   // ORIG D 0x93f4b20
extern float DEATH_TOWER_BONUS[13][4];    // ORIG D 0x93f4c40

// DungeonPartyBalance（InitScriptProducers_4.cpp 同布局：+0xd0/+0xd4 两 float，
// 前置 float[13][4] 线性区；ODR 同定义，仅本 TU 使用）。
struct DungeonPartyBalance
{
    float m_grid[13][4];   // +0x00
    float m_a;             // +0xd0
    float m_b;             // +0xd4
};
extern std::vector<DungeonPartyBalance> DUNGEON_PARTY_BALANCE_TABLE;

// upgradeAbilityBloodSystem —— ORIG 0x088b917b
void upgradeAbilityBloodSystem(int idx, ActiveStaticInfo* info)
{
    // ORIG 0x088b917b：全局表直接索引（无 tbl 指针变量）。
    info->m_hpBase = static_cast<int>(static_cast<float>(info->m_hpBase) *
                                      BLOOD_SYSTEM_BONUS[0][idx + 0x14]);
    info->m_attr17Base[13] += static_cast<int>(BLOOD_SYSTEM_BONUS[0][idx + 0xf]);
    info->m_statBase6 += static_cast<int>(BLOOD_SYSTEM_BONUS[0][idx + 0x32]);
    for (int i = 0; i <= 0x10; ++i)
    {
        if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) != 0x12)
            info->m_attr17Base[i] += static_cast<int>(BLOOD_SYSTEM_BONUS[0][idx + 0x23]);
    }
    info->m_statBase4 = static_cast<int>(static_cast<float>(info->m_statBase4) *
                                         BLOOD_SYSTEM_BONUS[0][idx + 0x19]);
    info->m_statBase3 += static_cast<int>(BLOOD_SYSTEM_BONUS[0][idx + 0x1e]);
    info->m_statBase11 = static_cast<int>(static_cast<float>(info->m_statBase11) *
                                          BLOOD_SYSTEM_BONUS[0][idx + 0x28]);
    info->m_statBase13 = static_cast<int>(static_cast<float>(info->m_statBase13) *
                                          BLOOD_SYSTEM_BONUS[0][idx + 0x28]);
    info->m_statBase12 = static_cast<int>(static_cast<float>(info->m_statBase12) *
                                          BLOOD_SYSTEM_BONUS[0][idx + 0x2d]);
    info->m_statBase14 = static_cast<int>(static_cast<float>(info->m_statBase14) *
                                          BLOOD_SYSTEM_BONUS[0][idx + 0x2d]);
}

// upgradeAbilityDeathTower —— ORIG 0x088b93bf
void upgradeAbilityDeathTower(int idx, ActiveStaticInfo* info)
{
    // ORIG 0x088b93bf：全局表直接索引（无 tbl 指针变量）。
    info->m_hpBase = static_cast<int>(static_cast<float>(info->m_hpBase) *
                                      DEATH_TOWER_BONUS[0][idx + 0x10]);
    info->m_attr17Base[13] += static_cast<int>(DEATH_TOWER_BONUS[0][idx + 0xc]);
    info->m_statBase6 += static_cast<int>(DEATH_TOWER_BONUS[0][idx + 0x28]);
    for (int i = 0; i <= 0x10; ++i)
    {
        if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) != 0x12)
            info->m_attr17Base[i] += static_cast<int>(DEATH_TOWER_BONUS[0][idx + 0x1c]);
    }
    info->m_statBase4 = static_cast<int>(static_cast<float>(info->m_statBase4) *
                                         DEATH_TOWER_BONUS[0][idx + 0x14]);
    info->m_statBase3 += static_cast<int>(DEATH_TOWER_BONUS[0][idx + 0x18]);
    info->m_statBase11 = static_cast<int>(static_cast<float>(info->m_statBase11) *
                                          DEATH_TOWER_BONUS[0][idx + 0x20]);
    info->m_statBase13 = static_cast<int>(static_cast<float>(info->m_statBase13) *
                                          DEATH_TOWER_BONUS[0][idx + 0x20]);
    info->m_statBase12 = static_cast<int>(static_cast<float>(info->m_statBase12) *
                                          DEATH_TOWER_BONUS[0][idx + 0x24]);
    info->m_statBase14 = static_cast<int>(static_cast<float>(info->m_statBase14) *
                                          DEATH_TOWER_BONUS[0][idx + 0x24]);
}

// upgradeAbilityByParty —— ORIG 0x088b8e88：job 0..3 截断；首个满足
// level>=m_a && level==m_b 的 balance 应用后立即返回。
void upgradeAbilityByParty(int job, ActiveStaticInfo* info, int level)
{
    if (info == 0)
        return;
    if (job < 0)
        job = 0;
    else if (job > 3)
        job = 3;
    for (std::vector<DungeonPartyBalance>::size_type i = 0;
         i < DUNGEON_PARTY_BALANCE_TABLE.size(); ++i)
    {
        const DungeonPartyBalance& bal = DUNGEON_PARTY_BALANCE_TABLE[i];
        if (static_cast<float>(level) < bal.m_a)
            continue;
        if (static_cast<float>(level) != bal.m_b)
            continue;
        const float* b = &bal.m_grid[0][0];
        info->m_hpBase = static_cast<int>(static_cast<float>(info->m_hpBase) *
                                          b[job + 0x10]);
        info->m_attr17Base[13] += static_cast<int>(b[job + 0xc]);
        info->m_statBase6 += static_cast<int>(b[job + 0x28]);
        for (int k = 0; k <= 0x10; ++k)
        {
            if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(k)) != 0x12)
                info->m_attr17Base[k] += static_cast<int>(b[job + 0x1c]);
        }
        info->m_statBase4 = static_cast<int>(static_cast<float>(info->m_statBase4) *
                                             b[job + 0x14]);
        if (info->m_statBase3 != 0)
            info->m_statBase3 += static_cast<int>(b[job + 0x18]);
        info->m_statBase11 = static_cast<int>(static_cast<float>(info->m_statBase11) *
                                              b[job + 0x20]);
        info->m_statBase13 = static_cast<int>(static_cast<float>(info->m_statBase13) *
                                              b[job + 0x20]);
        info->m_statBase12 = static_cast<int>(static_cast<float>(info->m_statBase12) *
                                              b[job + 0x24]);
        info->m_statBase14 = static_cast<int>(static_cast<float>(info->m_statBase14) *
                                              b[job + 0x24]);
        return;
    }
}
